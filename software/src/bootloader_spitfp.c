/* brickletboot
 * Copyright (C) 2010 Olaf Lüke <olaf@tinkerforge.com>
 *
 * spitfp.c: Tinkerforge Protocol (TFP) over SPI implementation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


/*

---- SPITFP (Tinkerforge Protocol over SPI) details ----

SPITFP uses four data lines (standard SPI):
* MISO
* MOSI
* CLK
* SELECT

The protocol has three different kinds of packets:
 NoData Packet (length 1):
  * Byte 0: 0

 ACK Packet (length 3):
  * Byte 0: Length: 3
  * Byte 1: Sequence Number: 0 | (last_sequence_number_seen << 4)
  * Byte 2: Checksum

 Data Packet (length 3 + payload length):
  * Byte 0: Length
  * Byte 1: Sequence Numbers: current_sequence_number | (last_sequence_number_seen << 4)
  * Bytes 2-(n-1): Payload
  * Byte n: Checksum

Protocol information:
* Master polls with NoData Packet if no data is to send
* Slave answers with NoData packet if no data is to send
* Only send next packet if ACK was received
* Send timeout is 20ms (re-send if no ACK is received after 20ms)
* Increase sequence number if ACK was received
* SPI MISO/MOSI data is just written to ringbuffer (if possible directly through dma)
* Sequence number runs from 0x1 to 0xF (0 is for ACK Packet only)
* Compared to the SPI stack protocol, this protocol is made for slow SPI clock speeds

*/

#include "bootloader_spitfp.h"

#include <string.h>
#include <stdint.h>

#include "io.h"
#include "tfp_common.h"

#include "bricklib2/utility/pearson_hash.h"
#include "bricklib2/logging/logging.h"

#define SPITFP_MIN_TFP_MESSAGE_LENGTH 8
#define SPITFP_MAX_TFP_MESSAGE_LENGTH 80

#define SPITFP_DMA_BUFFER_COUNT(i) ((*(uint32_t*)(DMAC->WRBADDR.bit.WRBADDR + i*0x10)) >> 16)
static const uint8_t spitfp_dummy_tx_byte = 0;

#define DESCRIPTOR_SECTION_RX_INDEX 0
#define DESCRIPTOR_SECTION_TX_INDEX 1

COMPILER_ALIGNED(16)
static DmacDescriptor descriptor_section[CONF_MAX_USED_CHANNEL_NUM] SECTION_DMAC_DESCRIPTOR;

COMPILER_ALIGNED(16)
static DmacDescriptor write_back_section[CONF_MAX_USED_CHANNEL_NUM] SECTION_DMAC_DESCRIPTOR;


void DMAC_Handler(void) {
	logd("-D-\n\r");
	system_interrupt_enter_critical_section();

	// Get Pending channel
	const uint8_t active_channel =  DMAC->INTPEND.reg & DMAC_INTPEND_ID_Msk;

	// Select the active channel
	DMAC->CHID.reg = DMAC_CHID_ID(active_channel);

	// Calculate block transfer size of the DMA transfer
	// total_size = descriptor_section[resource->channel_id].BTCNT.reg;
	// write_size = write_back_section[resource->channel_id].BTCNT.reg;

	// DMA channel interrupt handler
	const uint8_t isr = DMAC->CHINTFLAG.reg;
	if(isr & DMAC_CHINTENCLR_TERR) {
		// Clear transfer error flag
		DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TERR;
	} else if(isr & DMAC_CHINTENCLR_TCMPL) {
		// Clear the transfer complete flag
		DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL;

		if(active_channel == DESCRIPTOR_SECTION_TX_INDEX) {
			if(descriptor_section[DESCRIPTOR_SECTION_TX_INDEX].DESCADDR.reg != (uint32_t)&descriptor_section[DESCRIPTOR_SECTION_TX_INDEX]) {
				descriptor_section[DESCRIPTOR_SECTION_TX_INDEX].DESCADDR.reg = (uint32_t)&descriptor_section[DESCRIPTOR_SECTION_TX_INDEX];
				DMAC->CHINTENCLR.reg = DMAC_CHINTENCLR_TCMPL;
			}
		}
	} else if(isr & DMAC_CHINTENCLR_SUSP) {
		// Clear channel suspend flag
		DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_SUSP;
	}

	system_interrupt_leave_critical_section();
}

void spitfp_init(SPITFP *st) {
	st->last_sequence_number_seen = 0;
	st->current_sequence_number = 1;

	// Configure ring buffer
	memset(&st->buffer_recv, 0, SPITFP_RECEIVE_BUFFER_SIZE);
	ringbuffer_init(&st->ringbuffer_recv, SPITFP_RECEIVE_BUFFER_SIZE, st->buffer_recv);
	st->descriptor_rx_loop = &descriptor_section[DESCRIPTOR_SECTION_RX_INDEX];
	st->descriptor_tx_loop = &descriptor_section[DESCRIPTOR_SECTION_TX_INDEX];

	// Configure and enable SERCOM SPI module
	struct spi_config spitfp_spi_config;
	spi_get_config_defaults(&spitfp_spi_config);

	spitfp_spi_config.mode = SPI_MODE_SLAVE;
	spitfp_spi_config.transfer_mode = SPI_TRANSFER_MODE_3;

	spitfp_spi_config.mode_specific.slave.preload_enable = true;
	spitfp_spi_config.mode_specific.slave.frame_format = SPI_FRAME_FORMAT_SPI_FRAME;

	spitfp_spi_config.mux_setting = SPITFP_SPI_SIGNALMUX_SETTING; // DOPO=2, DIPO=0
	spitfp_spi_config.pinmux_pad0 = SPITFP_PINMUX_PAD0;  // PA4, MOSI
	spitfp_spi_config.pinmux_pad1 = SPITFP_PINMUX_PAD1;  // PA5, CLK
	spitfp_spi_config.pinmux_pad2 = SPITFP_PINMUX_PAD2;  // PA6, SS
	spitfp_spi_config.pinmux_pad3 = SPITFP_PINMUX_PAD3;  // PA7, MISO

	spi_init(&st->spi_module, SPITFP_SPI_MODULE, &spitfp_spi_config);
	spi_enable(&st->spi_module);

	tinydma_init(descriptor_section, write_back_section);

	struct dma_resource_config spitfp_resource_config_rx;
	dma_get_config_defaults(&spitfp_resource_config_rx);
	spitfp_resource_config_rx.peripheral_trigger = SPITFP_PERIPHERAL_TRIGGER_RX;
	spitfp_resource_config_rx.trigger_action = DMA_TRIGGER_ACTION_BEAT;
	dma_allocate(&st->resource_rx, &spitfp_resource_config_rx);


	// Configure SPI tx and rx resources
	struct dma_resource_config spitfp_resource_config_tx;
	dma_get_config_defaults(&spitfp_resource_config_tx);
	spitfp_resource_config_tx.peripheral_trigger = SPITFP_PERIPHERAL_TRIGGER_TX;
	spitfp_resource_config_tx.trigger_action = DMA_TRIGGER_ACTION_BEAT;
	dma_allocate(&st->resource_tx, &spitfp_resource_config_tx);


	// Configure SPI rx descriptor
	struct dma_descriptor_config spitfp_descriptor_config_rx;
	dma_descriptor_get_config_defaults(&spitfp_descriptor_config_rx);
	spitfp_descriptor_config_rx.beat_size = DMA_BEAT_SIZE_BYTE;
	spitfp_descriptor_config_rx.src_increment_enable = false;
	spitfp_descriptor_config_rx.block_transfer_count = SPITFP_RECEIVE_BUFFER_SIZE;
	spitfp_descriptor_config_rx.destination_address = (uint32_t)(st->buffer_recv + SPITFP_RECEIVE_BUFFER_SIZE);
	spitfp_descriptor_config_rx.source_address = (uint32_t)(&st->spi_module.hw->SPI.DATA.reg);
	spitfp_descriptor_config_rx.next_descriptor_address = (uint32_t)st->descriptor_rx_loop;
	dma_descriptor_create(st->descriptor_rx_loop, &spitfp_descriptor_config_rx);

	// Configure SPI tx descriptor
	struct dma_descriptor_config spitfp_descriptor_config_tx;
	dma_descriptor_get_config_defaults(&spitfp_descriptor_config_tx);
	spitfp_descriptor_config_tx.beat_size = DMA_BEAT_SIZE_BYTE;
	spitfp_descriptor_config_tx.dst_increment_enable = false;
	spitfp_descriptor_config_tx.src_increment_enable = false;
	spitfp_descriptor_config_tx.block_transfer_count = 1;
	spitfp_descriptor_config_tx.block_action = DMA_BLOCK_ACTION_INT;
	spitfp_descriptor_config_tx.source_address = (uint32_t)&spitfp_dummy_tx_byte;
	spitfp_descriptor_config_tx.destination_address = (uint32_t)(&st->spi_module.hw->SPI.DATA.reg);
	spitfp_descriptor_config_tx.next_descriptor_address = (uint32_t)st->descriptor_tx_loop;
	dma_descriptor_create(st->descriptor_tx_loop, &spitfp_descriptor_config_tx);

	// Configure SPI ACK descriptor
	struct dma_descriptor_config spitfp_descriptor_config_ack;
	dma_descriptor_get_config_defaults(&spitfp_descriptor_config_ack);
	spitfp_descriptor_config_ack.beat_size = DMA_BEAT_SIZE_BYTE;
	spitfp_descriptor_config_ack.dst_increment_enable = false;
	spitfp_descriptor_config_ack.block_transfer_count = SPITFP_PROTOCOL_OVERHEAD;
	spitfp_descriptor_config_ack.source_address = (uint32_t)(st->buffer_send + SPITFP_PROTOCOL_OVERHEAD);
	spitfp_descriptor_config_ack.destination_address = (uint32_t)(&st->spi_module.hw->SPI.DATA.reg);
	spitfp_descriptor_config_ack.next_descriptor_address = (uint32_t)st->descriptor_tx_loop;
	dma_descriptor_create(&st->descriptor_tx, &spitfp_descriptor_config_ack);

	// Add "ring buffer style" rx descriptor
	dma_add_descriptor(&st->resource_rx, st->descriptor_rx_loop);
	dma_add_descriptor(&st->resource_tx, st->descriptor_tx_loop);

	// Start dma transfer for rx resource
	tinydma_start_transfer(DESCRIPTOR_SECTION_RX_INDEX);
	tinydma_start_transfer(DESCRIPTOR_SECTION_TX_INDEX);
}

uint32_t counter = 0;
void spitfp_update_ringbuffer_pointer(SPITFP *st) {
	int16_t new_end = SPITFP_RECEIVE_BUFFER_SIZE - SPITFP_DMA_BUFFER_COUNT(DESCRIPTOR_SECTION_RX_INDEX) - 1;
	if(new_end == -1) {
		new_end = SPITFP_RECEIVE_BUFFER_SIZE - 1;
	}

	if(st->ringbuffer_recv.end != new_end) {
		counter++;
		if(counter >= 100) {
			counter = 0;
		}
	}

	st->ringbuffer_recv.end = new_end;
}

uint8_t spitfp_get_sequence_byte(SPITFP *st, const bool increase) {
	if(increase) {
		st->current_sequence_number++;
		if(st->current_sequence_number > 0xF) {
			st->current_sequence_number = 1;
		}
	}

	return st->current_sequence_number | (st->last_sequence_number_seen << 4);
}

void spitfp_enable_tx_dma(SPITFP *st) {
	system_interrupt_enter_critical_section();
	DMAC->CHID.reg = DMAC_CHID_ID(DESCRIPTOR_SECTION_TX_INDEX); // Select tx channel
	DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL; // Clear pending interrupts
	DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL; // Enable transfer complete interrupt
	st->descriptor_tx_loop->DESCADDR.reg = (uint32_t)&st->descriptor_tx; // Set next descriptor to ACK
	system_interrupt_leave_critical_section();
}

void spitfp_send_ack_and_message(SPITFP *st, uint8_t *data, const uint8_t length) {
	//spitfp_write_ack_to_buffer(st);

	// TODO: Do we need the additional ACK here in front? Probably not!

	uint8_t checksum = 0;
	st->buffer_send[0] = length + SPITFP_PROTOCOL_OVERHEAD;
	PEARSON(checksum, st->buffer_send[0]);

	st->buffer_send[1] = spitfp_get_sequence_byte(st, true);
	PEARSON(checksum, st->buffer_send[1]);

	for(uint8_t i = 0; i < length; i++) {
		st->buffer_send[2+i] = data[i];
		PEARSON(checksum, st->buffer_send[2+i]);
	}

	st->buffer_send[length + SPITFP_PROTOCOL_OVERHEAD-1] = checksum;

	st->descriptor_tx.BTCNT.reg = st->buffer_send[0];
	st->descriptor_tx.SRCADDR.reg = (uint32_t)(st->buffer_send + st->buffer_send[0]);

/*	printf("data:");
	for(uint8_t i = 0; i < st->buffer_send[0]; i++) {
		if((i % 8) == 0) {
			printf("\n\r");
		}
		printf("%d ", st->buffer_send[i]);
	}
	printf("\n\r");*/

	spitfp_enable_tx_dma(st);
}

void spitfp_send_ack(SPITFP *st) {
	// Set new sequence number and checksum for ACK
	st->buffer_send[0] = SPITFP_PROTOCOL_OVERHEAD;
	st->buffer_send[1] = st->last_sequence_number_seen << 4;
	st->buffer_send[2] = pearson_permutation[pearson_permutation[st->buffer_send[0]] ^ st->buffer_send[1]];

	st->descriptor_tx.BTCNT.reg = SPITFP_PROTOCOL_OVERHEAD;
	st->descriptor_tx.SRCADDR.reg = (uint32_t)(st->buffer_send + SPITFP_PROTOCOL_OVERHEAD);

	spitfp_enable_tx_dma(st);
}

// TODO: Implement me!
bool spitfp_is_send_possible(SPITFP *st) {
	return descriptor_section[DESCRIPTOR_SECTION_TX_INDEX].DESCADDR.reg == (uint32_t)&descriptor_section[DESCRIPTOR_SECTION_TX_INDEX];
}

void spitfp_tick(BootloaderStatus *bootloader_status) {
	SPITFP *st = &bootloader_status->st;
	uint8_t message[SPITFP_MAX_TFP_MESSAGE_LENGTH] = {0};
	uint8_t message_position = 0;
	uint16_t num_to_remove_from_ringbuffer = 0;
	uint8_t checksum = 0;

	uint8_t data_sequence_number = 0;
	uint8_t data_length = 0;

	spitfp_update_ringbuffer_pointer(st);
	uint16_t used = ringbuffer_get_used(&st->ringbuffer_recv);
	uint16_t start = st->ringbuffer_recv.start;
	for(uint16_t i = start; i < start+used; i++) {
		const uint16_t index = i % SPITFP_RECEIVE_BUFFER_SIZE;
		const uint8_t data = st->buffer_recv[index];
		num_to_remove_from_ringbuffer++;

		switch(st->state) {
			case SPITFP_STATE_START: {
				checksum = 0;
				message_position = 0;

				if(data == SPITFP_PROTOCOL_OVERHEAD) {
					st->state = SPITFP_STATE_ACK_SEQUENCE_NUMBER;
				} else if(data >= SPITFP_MIN_TFP_MESSAGE_LENGTH && data <= SPITFP_MAX_TFP_MESSAGE_LENGTH) {
					st->state = SPITFP_STATE_MESSAGE_SEQUENCE_NUMBER;
				} else {
					ringbuffer_remove(&st->ringbuffer_recv, 1);
					num_to_remove_from_ringbuffer--;
					break;
				}

				data_length = data;
				PEARSON(checksum, data_length);

				break;
			}

			case SPITFP_STATE_ACK_SEQUENCE_NUMBER: {
				data_sequence_number = data;
				PEARSON(checksum, data_sequence_number);
				st->state = SPITFP_STATE_ACK_CHECKSUM;
				break;
			}

			case SPITFP_STATE_ACK_CHECKSUM: {
				// Whatever happens here, we will go to start again and remove
				// data from ringbuffer
				st->state = SPITFP_STATE_START;
				ringbuffer_remove(&st->ringbuffer_recv, num_to_remove_from_ringbuffer);
				num_to_remove_from_ringbuffer = 0;

				if(checksum != data) {
					break;
				}

				uint8_t last_sequence_number_seen_by_master = (data_sequence_number & 0xF0) >> 4;
				if(last_sequence_number_seen_by_master == st->current_sequence_number) {
					st->buffer_send_length = 0;
				}

				break;
			}

			case SPITFP_STATE_MESSAGE_SEQUENCE_NUMBER: {
				data_sequence_number = data;
				PEARSON(checksum, data_sequence_number);
				st->state = SPITFP_STATE_MESSAGE_DATA;
				break;
			}

			case SPITFP_STATE_MESSAGE_DATA: {
				message[message_position] = data;
				message_position++;

				PEARSON(checksum, data);

				if(message_position == data_length - SPITFP_PROTOCOL_OVERHEAD) {
					st->state = SPITFP_STATE_MESSAGE_CHECKSUM;
				}
				break;
			}

			case SPITFP_STATE_MESSAGE_CHECKSUM: {
				// Whatever happens here, we will go to start again
				st->state = SPITFP_STATE_START;

				if(checksum != data) {
					// If checksum is not correct, we remove the data
					// TODO: Should we empty the whole buffer here?
					ringbuffer_remove(&st->ringbuffer_recv, num_to_remove_from_ringbuffer);
					num_to_remove_from_ringbuffer = 0;
					break;
				}

				uint8_t last_sequence_number_seen_by_master = (data_sequence_number & 0xF0) >> 4;
				if(last_sequence_number_seen_by_master == st->current_sequence_number) {
					st->buffer_send_length = 0;
				}

				const uint8_t message_sequence_number = data_sequence_number & 0x0F;

				if(spitfp_is_send_possible(st)) {
					// If we can currently send a message, we can now definitely remove
					// the data from ring buffer.
					ringbuffer_remove(&st->ringbuffer_recv, num_to_remove_from_ringbuffer);
					num_to_remove_from_ringbuffer = 0;

					// If sequence number is new, we can handle the message.
					// Otherwise we only ACK the already handled message again.
					if(message_sequence_number != st->last_sequence_number_seen) {
						st->last_sequence_number_seen = message_sequence_number;
						// The handle message function will send an ACK for the message
						// if it can handle the message at the current moment.
						// Otherwise it return false. In that case the SPI master
						// will send the message again and we can handle it then.
						tfp_common_handle_message(message, message_position, bootloader_status);
					} else {
						spitfp_send_ack(st);
					}
				}

				break;
			}
		}
	}

	st->state = SPITFP_STATE_START;
}
