/* gps-v2-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * main.c: Initialization for GPS Bricklet 2.0
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

#include <stdio.h>

#include "port.h"
#include "clock.h"
#include "gclk.h"
#include "system.h"
#include "pinmux.h"
#include "wdt.h"
#include "bricklib2/logging/logging.h"

#include "configs/config.h"

/* SPI COMMUNICATION TODO
MISO
MOSI
CLK
SELECT

No Data:
Byte X: 0

Ack Packet:
Byte 0: Length: 3
Byte 1: Sequence Number: 0 | (last_sequence_number_seen << 4)
Byte 2: Checksum

Data Packet:
Byte 0: Length
Byte 1: Sequence Numbers: current_sequence_number | (last_sequence_number_seen << 4)
Bytes 2-(n-1): Data
Byte n: Checksum


Protocol:
* Only send if ACK was received
* Send timeout 25ms (re-send if no ACK is received after 25ms)
* Increase sequence number if ACK was received
* SPI MISO/MOSI data is just written to ringbuffer (if possible directly through dma)
*/


static void config_led(void) {
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	pin_conf.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED0_PIN, &pin_conf);
	port_pin_set_output_level(LED0_PIN, false);
}

void system_board_init(void) {
	// Enable watchdog
	struct wdt_conf wdt_config;
	wdt_get_config_defaults(&wdt_config);
	wdt_config.clock_source = GCLK_GENERATOR_2;
	wdt_config.timeout_period = WDT_PERIOD_16384CLK; // This should be ~1s
	wdt_set_config(&wdt_config);
}

int main(void) {
	system_init();
	logging_init();

	/*Configure system tick to generate periodic interrupts */
//	SysTick_Config(system_gclk_gen_get_hz(GCLK_GENERATOR_2));

	config_led();

	for(uint32_t i = 0; i < 200000; i++) {
		__NOP();
	}

	uint8_t i = 0;
	while (true) {
		logd("%d ", i++);
		port_pin_toggle_output_level(LED0_PIN);
		wdt_reset_count();

		for(uint32_t i = 0; i < 50000; i++) {
			__NOP();
		}
	}
}
