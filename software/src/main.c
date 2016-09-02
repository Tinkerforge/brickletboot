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
(in this case for 16kb of flash):

-- BOOTLOADER --------------------------------------------------------------
| brickletboot                                                             |
| 0000-2000                                                                |
----------------------------------------------------------------------------

-- FIRMWARE ----------------------------------------------------------------
| firmware      | firmware version      | device id    | firmware crc      |
| 2000-3ff4     | 3ff4-3ff8             | 3ff8-3ffc    | 3ffc-4000         |
----------------------------------------------------------------------------

*/

#include <stdio.h>

#include "port.h"
#include "clock.h"
#include "gclk.h"
#include "wdt.h"
#include "crc32.h"
#include "nvm.h"
#include "spi.h"
#include "bootloader_spitfp.h"
#include "boot.h"
#include "tfp_common.h"

#include "bricklib2/bootloader/tinydma.h"
#include "bricklib2/bootloader/bootloader.h"
#include "bricklib2/logging/logging.h"
#include "configs/config.h"

static void configure_nvm(void) {
	struct nvm_config config_nvm;

	nvm_get_config_defaults(&config_nvm);
	config_nvm.manual_page_write = false;
	nvm_set_config(&config_nvm);
}

static void configure_wdt(void) {
	// Enable watchdog
	struct wdt_conf wdt_config;
	wdt_get_config_defaults(&wdt_config);
	wdt_config.enable = false; // TODO: Remove me
	wdt_config.clock_source = GCLK_GENERATOR_2;
	wdt_config.timeout_period = WDT_PERIOD_16384CLK; // This should be ~1s
	wdt_set_config(&wdt_config);

//	wdt_reset_count();
}

static void configure_led(void) {
#if 0
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	port_pin_set_config(BOOTLOADER_STATUS_LED_PIN, &pin_conf);
	port_pin_set_output_level(BOOTLOADER_STATUS_LED_PIN, false);
#endif

	// Do above code by hand, saves 40 bytes

	PortGroup *const port = &PORT->Group[0];
	const uint32_t pin_mask = (1 << BOOTLOADER_STATUS_LED_PIN);

	const uint32_t lower_pin_mask = (pin_mask & 0xFFFF);
	const uint32_t upper_pin_mask = (pin_mask >> 16);

	// TODO: Decide if lower or upper pin_mask is needed with pre-processor
	//       to save a few bytes

	// Configure lower 16 bits (needed if lower_pin_mask != 0)
	port->WRCONFIG.reg = (lower_pin_mask << PORT_WRCONFIG_PINMASK_Pos) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_WRPINCFG;

	// Configure upper 16 bits  (needed if upper_pin_mask != 0)
	port->WRCONFIG.reg = (upper_pin_mask << PORT_WRCONFIG_PINMASK_Pos) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_HWSEL;

	// Direction to output
	port->DIRSET.reg = pin_mask;

	// Turn LED off
	port->OUTSET.reg = (1 << BOOTLOADER_STATUS_LED_PIN);
}

// Initialize everything that is needed for bootloader as well as firmware
void system_init(void) {
	system_clock_init();

	configure_wdt();
	configure_led();

	// The following can be enabled if needed by the firmware
	// Initialize EVSYS hardware
	//_system_events_init();

	// Initialize External hardware
	//_system_extint_init();

	// Initialize DIVAS hardware
	//_system_divas_init();
}

BootloaderStatus bootloader_status;
int main() {
	// Jump to firmware if we can
	const uint8_t can_jump_to_firmware = boot_can_jump_to_firmware();
	if(can_jump_to_firmware == TFP_COMMON_SET_BOOTLOADER_MODE_STATUS_OK) {
		boot_jump_to_firmware();
	}

#if LOGGING_LEVEL != LOGGING_NONE
	logging_init();
	logi("Starting brickletboot (version " BRICKLETBOOT_VERSION ")\n\r");
	logi("Compiled on " __DATE__ " " __TIME__ "\n\r");
#endif

	// We can't jump to firmware, so lets enter bootloader mode
	bootloader_status.boot_mode = BOOT_MODE_BOOTLOADER;
	bootloader_status.status_led_config = 1;
	bootloader_status.st.descriptor_section = tinydma_get_descriptor_section();
	bootloader_status.st.write_back_section = tinydma_get_write_back_section();

	configure_nvm();

	spitfp_init(&bootloader_status.st);

	while(true) {
		spitfp_tick(&bootloader_status);
	}
}
