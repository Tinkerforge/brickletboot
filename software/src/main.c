/* brickletboot
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * main.c: Bricklet bootloader
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

For Bricklets with co-processor we use the following flash memory map
(in this case for 16kb of flash:

-- BOOTLOADER---------------------------------------------------------------
| brickletboot  | boot_info                                                |
| 0000-0ffc     | 0ffc-1000                                                |
----------------------------------------------------------------------------

-- FIRMWARE that is used ---------------------------------------------------
| firmware used | firmware used version | device id    | firmware used crc |
| 1000-27f4     | 27f4-27f8             | 27f8-27fc    | 27fc-2800         |
----------------------------------------------------------------------------

-- FIRMWARE to be flashed if flash bit in boot_info is set -----------------
| firmware new  | firmware new version  | device id    | firmware new crc  |
| 2800-3ff4     | 3ff4-3ff8             | 3ff8-3ffc    | 3ffc-4000         |
----------------------------------------------------------------------------

*/

#include <stdio.h>

#include "tinydma.h"
#include "port.h"
#include "clock.h"
#include "gclk.h"
#include "system.h"
#include "pinmux.h"
#include "wdt.h"
#include "crc32.h"
#include "nvm.h"
#include "spi.h"
#include "bootloader_spitfp.h"

#include "bricklib2/bootloader/bootloader.h"
#include "bricklib2/logging/logging.h"
#include "configs/config.h"

static BootloaderStatus bootloader_status;
const uint32_t bootloader_device_identifier = 0; // TODO: Set through compile flag


/*const uint32_t boot_info __attribute__ ((section(".boot_info"))) = 0;
const uint32_t *read_boot_info = (uint32_t*)(BOOT_INFO_POS_START);*/
typedef void (* firmware_start_func_t)(void);

static void jump_to_firmware() {
	register firmware_start_func_t firmware_start_func;
	const uint32_t stack_pointer_address = BL_FIRMWARE_START_POS;
	const uint32_t reset_pointer_address = BL_FIRMWARE_START_POS + 4;

	// Set stack pointer with the first word of the run mode program
	// Vector table's first entry is the stack pointer value
	__set_MSP((*(uint32_t *)stack_pointer_address));

	//set the NVIC's VTOR to the beginning of main app
	SCB->VTOR = stack_pointer_address& SCB_VTOR_TBLOFF_Msk;

	// Set the program counter to the application start address
	// Vector table's second entry is the system reset value
	firmware_start_func = * ((firmware_start_func_t *)reset_pointer_address);
	firmware_start_func();
}

static void reset() {
	NVIC_SystemReset();
}

static bool check_device_identifier(void) {
	return BL_FIRMWARE_CONFIGURATION_POSITION->device_identifier == bootloader_device_identifier;
}

static bool check_crc(void) {
	uint32_t crc = 0;
	crc32_calculate((void*)BL_FIRMWARE_START_POS, BL_FIRMWARE_SIZE - sizeof(uint32_t), &crc);
	return crc == BL_FIRMWARE_CONFIGURATION_POSITION->firmware_crc;
}

static bool copy_firmware(void) {/*
	__disable_irq();

	// Unlock flash region
    FLASHD_Unlock(FIRMWARE_USED_POS_START, FIRMWARE_USED_POS_END, 0, 0);

	for(uint32_t address = FIRMWARE_TEMP_POS_START; address < FIRMWARE_TEMP_POS_END; address+=IFLASH_PAGE_SIZE_SAM3) {
		uint8_t *buffer_flash = (uint8_t*)address;

		uint8_t buffer[IFLASH_PAGE_SIZE_SAM3] = {0};
		for(uint16_t i = 0; i < IFLASH_PAGE_SIZE_SAM3; i++) {
			buffer[i] = buffer_flash[i];
		}

	    FLASHD_Write(address-FIRMWARE_SIZE, buffer, IFLASH_PAGE_SIZE_SAM3);
	}

    // Lock flash and enable irqs again
    FLASHD_Lock(FIRMWARE_USED_POS_START, FIRMWARE_USED_POS_END, 0, 0);

    __enable_irq();

	return true;*/

	return false;
}

static void remove_bootloader_flag(void) {

}

static void configure_nvm(void) {
	struct nvm_config config_nvm;

	nvm_get_config_defaults(&config_nvm);
	config_nvm.manual_page_write = false;
	nvm_set_config(&config_nvm);
}

static bool is_bootloader_flag_set(void) {
	return true;
}

void bootloader_loop(void) {
	configure_nvm();

	while(true) {

	}
}

/*
int main(void) {
	logging_init();

	system_init();

	if(is_bootloader_flag_set()) {
		if(!check_device_identifier()) {
			remove_bootloader_flag();
			reset();
		} else if(!check_crc()) {
			remove_bootloader_flag();
			reset();
		}

		bootloader_loop();
	}

	jump_to_firmware();

	while(true);
}

*/



void system_board_init(void) {
	// Enable watchdog
	struct wdt_conf wdt_config;
	wdt_get_config_defaults(&wdt_config);
	wdt_config.enable = false; // TODO: Remove me
	wdt_config.clock_source = GCLK_GENERATOR_2;
	wdt_config.timeout_period = WDT_PERIOD_16384CLK; // This should be ~1s
	wdt_set_config(&wdt_config);

//	wdt_reset_count();
}

static void config_led(void) {
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	pin_conf.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(bootloader_status.status_led_gpio_pin, &pin_conf);
	port_pin_set_output_level(bootloader_status.status_led_gpio_pin, false);
}


int main(void) {
	bootloader_status.status_led_gpio_pin = LED0_PIN;
	bootloader_status.status_led_config = 1;

	system_init();
	logging_init();

	logi("Starting brickletboot (version " BRICKLET_BOOT_VERSION ")\n\r");
	logi("Compiled on %s %s\n\r", __DATE__, __TIME__);
	config_led();

	spitfp_init(&bootloader_status.st);

	while(true) {
		spitfp_tick(&bootloader_status);
	}
}
