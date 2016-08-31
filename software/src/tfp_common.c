/* brickletboot
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * tfp_common.c: Tinkerforge Protocol (TFP) common messages
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

#include "tfp_common.h"

#include <string.h>

#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/logging/logging.h"

#define TFP_COMMON_FID_SET_BOOTLOADER_MODE 235
#define TFP_COMMON_FID_GET_BOOTLOADER_MODE 236
#define TFP_COMMON_FID_SET_WRITE_FIRMWARE_POINTER 237
#define TFP_COMMON_FID_WRITE_FIRMWARE 238
#define TFP_COMMON_FID_SET_STATUS_LED_CONFIG 239
#define TFP_COMMON_FID_GET_STATUS_LED_CONFIG 240
#define TFP_COMMON_FID_GET_PROTOCOL1_BRICKLET_NAME 241 // unused ?
#define TFP_COMMON_FID_GET_CHIP_TEMPERATURE 242
#define TFP_COMMON_FID_RESET 243
#define TFP_COMMON_FID_GET_ADC_CALIBRATION 250 // unused ?
#define TFP_COMMON_FID_ADC_CALIBRATE 251 // unused ?
#define TFP_COMMON_FID_CO_MCU_ENUMERATE 252
#define TFP_COMMON_FID_ENUMERATE_CALLBACK 253
#define TFP_COMMON_FID_ENUMERATE 254
#define TFP_COMMON_FID_GET_IDENTITY 255

#define TFP_COMMON_ENUMERATE_CALLBACK_UID_LENGTH 8
#define TFP_COMMON_ENUMERATE_CALLBACK_VERSION_LENGTH 3

#define TFP_COMMON_ENUMERATE_TYPE_AVAILABLE 0
#define TFP_COMMON_ENUMERATE_TYPE_ADDED     1
#define TFP_COMMON_ENUMERATE_TYPE_REMOVED   2

#define TFP_COMMON_STATUS_LED_OFF                       0
#define TFP_COMMON_STATUS_LED_ON                        1
#define TFP_COMMON_STATUS_LED_SHOW_COMMUNICATION_STATUS 2

typedef struct {
	TFPMessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) TFPCommonSetBootloaderMode;

typedef struct {
	TFPMessageHeader header;
	uint8_t status;
} __attribute__((__packed__)) TFPCommonSetBootloaderModeReturn;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) TFPCommonGetBootloaderMode;

typedef struct {
	TFPMessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) TFPCommonGetBootloaderModeReturn;

typedef struct {
	TFPMessageHeader header;
	uint32_t pointer;
} __attribute__((__packed__)) TFPCommonSetWriteFirmwarePointer;

typedef struct {
	TFPMessageHeader header;
	uint8_t data[64];
} __attribute__((__packed__)) TFPCommonWriteFirmware;

typedef struct {
	TFPMessageHeader header;
	uint8_t status;
} __attribute__((__packed__)) TFPCommonWriteFirmwareReturn;

typedef struct {
	TFPMessageHeader header;
	uint8_t config;
} __attribute__((__packed__)) TFPCommonSetStatusLEDConfig;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) TFPCommonGetStatusLEDConfig;

typedef struct {
	TFPMessageHeader header;
	uint8_t config;
} __attribute__((__packed__)) TFPCommonGetStatusLEDConfigReturn;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) TFPCommonGetChipTemperature;

typedef struct {
	TFPMessageHeader header;
	int16_t temperature;
} __attribute__((__packed__)) TFPCommonGetChipTemperatureReturn;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) TFPCommonReset;

typedef struct {
	TFPMessageHeader header;
	uint32_t uid;
} __attribute__((packed)) TFPCommonCoMCUEnumerateReturn;

typedef struct {
	TFPMessageHeader header;
} __attribute__((packed)) TFPCommonCoMCUEnumerate;

typedef struct {
	TFPMessageHeader header;
	char uid[TFP_COMMON_ENUMERATE_CALLBACK_UID_LENGTH];
	char connected_uid[TFP_COMMON_ENUMERATE_CALLBACK_UID_LENGTH];
	char position;
	uint8_t version_hw[TFP_COMMON_ENUMERATE_CALLBACK_VERSION_LENGTH];
	uint8_t version_fw[TFP_COMMON_ENUMERATE_CALLBACK_VERSION_LENGTH];
	uint16_t device_identifier;
	uint8_t enumeration_type;
} __attribute__((__packed__)) TFPCommonEnumerateCallback;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) TFPCommonEnumerate;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) TFPCommonGetIdentity;

typedef struct {
	TFPMessageHeader header;
	char uid[TFP_COMMON_ENUMERATE_CALLBACK_UID_LENGTH];
	char connected_uid[TFP_COMMON_ENUMERATE_CALLBACK_UID_LENGTH];
	char position;
	uint8_t version_hw[TFP_COMMON_ENUMERATE_CALLBACK_VERSION_LENGTH];
	uint8_t version_fw[TFP_COMMON_ENUMERATE_CALLBACK_VERSION_LENGTH];
	uint16_t device_identifier;
} __attribute__((__packed__)) TFPCommonGetIdentityReturn;

#define TFP_COMMON_RETURN_MESSAGE_LENGTH 80

uint32_t tfp_common_get_uid(void) {
	return 1234; // TODO: Get from ID register
}

uint32_t tfp_common_get_device_identifier(void) {
	return 276; // TODO: Get from flash
}

bool tfp_common_set_bootloader_mode(const TFPCommonSetBootloaderMode *data, void *_return_message, BootloaderStatus *bs) {
	logd("tfp_common_set_bootloader_mode\n\r");

	TFPCommonSetBootloaderModeReturn *sbmr = _return_message;
	sbmr->header = data->header;
	sbmr->header.length = sizeof(TFPCommonSetBootloaderModeReturn);

	// TODO: implement me

	return true;
}


bool tfp_common_get_bootloader_mode(const TFPCommonGetBootloaderMode *data, void *_return_message, BootloaderStatus *bs) {
	logd("tfp_common_get_bootloader_mode\n\r");

	TFPCommonGetBootloaderModeReturn *gbmr = _return_message;
	gbmr->header = data->header;
	gbmr->header.length = sizeof(TFPCommonGetBootloaderModeReturn);

	// TODO: implement me

	return true;
}

bool tfp_common_set_write_firmware_pointer(const TFPCommonSetWriteFirmwarePointer *data, void *_return_message, BootloaderStatus *bs) {
	logd("tfp_common_set_write_firmware_pointer\n\r");

	// TODO: implement me

	return false;
}

bool tfp_common_write_firmware(const TFPCommonWriteFirmware *data, void *_return_message, BootloaderStatus *bs) {
	logd("tfp_common_write_firmware\n\r");

	TFPCommonWriteFirmwareReturn *wfr = _return_message;
	wfr->header = data->header;
	wfr->header.length = sizeof(TFPCommonWriteFirmwareReturn);

	// TODO: implement me

	return true;
}

bool tfp_common_set_status_led_config(const TFPCommonSetStatusLEDConfig *data, void *_return_message, BootloaderStatus *bs) {
	logd("set led: %d\n\r", data->config);

	port_pin_set_output_level(bs->status_led_gpio_pin, bs->status_led_config == TFP_COMMON_STATUS_LED_OFF);
	return false;
}

bool tfp_common_get_status_led_config(const TFPCommonGetStatusLEDConfig *data, void *_return_message, BootloaderStatus *bs) {
	logd("get led\n\r");
	TFPCommonGetStatusLEDConfigReturn *gslcr = _return_message;
	gslcr->header = data->header;
	gslcr->header.length = sizeof(TFPCommonGetStatusLEDConfigReturn);

	gslcr->config = bs->status_led_config; // TODO: Set correct value

	return true;
}

bool tfp_common_get_chip_temperature(const TFPCommonGetChipTemperature *data, void *_return_message) {
	logd("tfp_common_get_chip_temperature\n\r");

	TFPCommonGetChipTemperatureReturn *gctr = _return_message;
	gctr->header = data->header;
	gctr->header.length = sizeof(TFPCommonGetChipTemperatureReturn);

	gctr->temperature = 42;
	// TODO: implement me

	return true;
}

bool tfp_common_reset(const TFPCommonReset *data, void *_return_message) {
	logd("tfp_common_reset\n\r");
	NVIC_SystemReset();

	// TODO: Only apply the reset after some time so we can return if response expected?

	return false;
}

bool tfp_common_enumerate(const TFPCommonEnumerate *data, void *_return_message) {
	logd("tfp_common_enumerate\n\r");
	// The function itself does not return anything, but we return the callback here instead.
	TFPCommonEnumerateCallback *ec = _return_message;
	ec->header.uid              = tfp_common_get_uid();
	ec->header.length           = sizeof(TFPCommonEnumerateCallback);
	ec->header.fid              = TFP_COMMON_FID_ENUMERATE_CALLBACK;
	ec->header.sequence_num     = 0; // Sequence number for callback is 0
	ec->header.return_expected  = 1;
	ec->header.authentication   = 0; // TODO
	ec->header.other_options    = 0;
	ec->header.error            = 0;
	ec->header.future_use       = 0;

	tfp_uid_uint32_to_base58(tfp_common_get_uid(), ec->uid);
	memset(ec->connected_uid, 0, TFP_COMMON_ENUMERATE_CALLBACK_UID_LENGTH);
	ec->position = 'X';
	ec->version_hw[0] = 1; ec->version_hw[0] = 0; ec->version_hw[0] = 0; // TODO: Set correct hardware version
	ec->version_fw[0] = 1; ec->version_fw[0] = 0; ec->version_fw[0] = 0; // TODO: Set correct firmware version
	ec->device_identifier = tfp_common_get_device_identifier();
	ec->enumeration_type = TFP_COMMON_ENUMERATE_TYPE_AVAILABLE;

	return true;
}

bool tfp_common_co_mcu_enumerate(const TFPCommonCoMCUEnumerate *data, void *_return_message) {
	logd("tfp_common_co_mcu_enumerate\n\r");

	tfp_common_enumerate((void*)data, _return_message);
	((TFPCommonEnumerateCallback*)data)->enumeration_type = TFP_COMMON_ENUMERATE_TYPE_ADDED;
	return true;
}

bool tfp_common_get_identity(const TFPCommonGetIdentity *data, void *_return_message) {
	logd("tfp_common_get_identity\n\r");

	TFPCommonGetIdentityReturn *gir = _return_message;
	gir->header = data->header;
	gir->header.length = sizeof(TFPCommonGetIdentityReturn);
	tfp_uid_uint32_to_base58(tfp_common_get_uid(), gir->uid);
	memset(gir->connected_uid, 0, TFP_COMMON_ENUMERATE_CALLBACK_UID_LENGTH);
	gir->position = 'X';
	gir->version_hw[0] = 1; gir->version_hw[0] = 0; gir->version_hw[0] = 0; // TODO: Set correct hardware version
	gir->version_fw[0] = 1; gir->version_fw[0] = 0; gir->version_fw[0] = 0; // TODO: Set correct firmware version
	gir->device_identifier = tfp_common_get_device_identifier();

	return true;
}

void tfp_common_handle_message(const void *message, const uint8_t length, BootloaderStatus *bs) {
	// TODO: Wait for ~1 second after startup for CoMCUEnumerate message. If there is none,
	//       send an "answer" to it anyway
	logd("tfp_common_handle_message: %d\n\r", length);
	const uint8_t message_length = tfp_get_length_from_message(message);
	if(length != message_length) {
		logw("Length mismatch: received %d, message length %d\n\r", length, message_length);
		// TODO: What do we do here? Send ACK or ignore message?
		return;
	}

	uint8_t return_message[TFP_COMMON_RETURN_MESSAGE_LENGTH];
	bool has_return_message = false;

	switch(tfp_get_fid_from_message(message)) {
		case TFP_COMMON_FID_SET_BOOTLOADER_MODE:        has_return_message = tfp_common_set_bootloader_mode(message, return_message, bs); break;
		case TFP_COMMON_FID_GET_BOOTLOADER_MODE:        has_return_message = tfp_common_get_bootloader_mode(message, return_message, bs); break;
		case TFP_COMMON_FID_SET_WRITE_FIRMWARE_POINTER: has_return_message = tfp_common_set_write_firmware_pointer(message, return_message, bs); break;
		case TFP_COMMON_FID_WRITE_FIRMWARE:             has_return_message = tfp_common_write_firmware(message, return_message, bs); break;
		case TFP_COMMON_FID_SET_STATUS_LED_CONFIG:      has_return_message = tfp_common_set_status_led_config(message, return_message, bs); break;
		case TFP_COMMON_FID_GET_STATUS_LED_CONFIG:      has_return_message = tfp_common_get_status_led_config(message, return_message, bs); break;
		case TFP_COMMON_FID_GET_CHIP_TEMPERATURE:       has_return_message = tfp_common_get_chip_temperature(message, return_message); break;
		case TFP_COMMON_FID_RESET:                      has_return_message = tfp_common_reset(message, return_message); break;
		case TFP_COMMON_FID_CO_MCU_ENUMERATE:           has_return_message = tfp_common_co_mcu_enumerate(message, return_message); break;
		case TFP_COMMON_FID_ENUMERATE:                  has_return_message = tfp_common_enumerate(message, return_message); break;
		case TFP_COMMON_FID_GET_IDENTITY:               has_return_message = tfp_common_get_identity(message, return_message); break;
		default: break; // TODO: Call firmware handle_message function if not in bootloader mode
	}

	if(has_return_message) {
		// TODO: Set UID here?
		spitfp_send_ack_and_message(&bs->st, return_message, tfp_get_length_from_message(return_message));
	} else {
		if(tfp_is_return_expected(message)) {
			// TODO: Send return expected stuff
		} else {
			spitfp_send_ack(&bs->st);
		}
	}
}
