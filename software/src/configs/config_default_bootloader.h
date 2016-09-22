/* brickletboot
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config.c: Default bootloader configurations (for test compilation)
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

#ifndef CONFIG_DEFAULT_BOOTLOADER_H
#define CONFIG_DEFAULT_BOOTLOADER_H

// --- Basic defines ---
#define BOOTLOADER_STATUS_LED_PIN      PIN_PA16
#define BOOTLOADER_DEVICE_IDENTIFIER   29999
#define BOOTLOADER_HW_VERSION_MAJOR    1
#define BOOTLOADER_HW_VERSION_MINOR    0
#define BOOTLOADER_HW_VERSION_REVISION 0




// --- SPITFP ---
#define SPITFP_SPI_MODULE             SERCOM0
#define SPITFP_SPI_SIGNALMUX_SETTING  SPI_SIGNAL_MUX_SETTING_I

#define SPITFP_PINMUX_PAD0            PINMUX_PA04D_SERCOM0_PAD0 // MOSI
#define SPITFP_PINMUX_PAD1            PINMUX_PA05D_SERCOM0_PAD1 // CLK
#define SPITFP_PINMUX_PAD2            PINMUX_PA06D_SERCOM0_PAD2 // SS
#define SPITFP_PINMUX_PAD3            PINMUX_PA07D_SERCOM0_PAD3 // MISO

#define SPITFP_PERIPHERAL_TRIGGER_TX  SERCOM0_DMAC_ID_TX
#define SPITFP_PERIPHERAL_TRIGGER_RX  SERCOM0_DMAC_ID_RX

#define SPITFP_RECEIVE_BUFFER_SIZE    1024



// --- TINYDMA ---
#define TINYDMA_MAX_USED_CHANNEL 2

// Note: If you use the external descriptors, you need an external handler too.
//       It is possible to use external handler with internal descriptors.
#define TINYDNA_MINIMAL_INTERRUPT_HANDLER
#define TINYDMA_USE_INTERNAL_DESCRIPTORS


// --- BOOTLOADER FUNCTIONS ---

// define bootloader functions to put them into the BootloaderFunctions struct
// These functions are in the bootloader code and can be used from the firmware


// Available defines:
#if 0
#define BOOTLOADER_FUNCTION_SPITFP_TICK
#define BOOTLOADER_FUNCTION_SEND_ACK_AND_MESSAGE
#define BOOTLOADER_FUNCTION_SPITFP_IS_SEND_POSSIBLE
#define BOOTLOADER_FUNCTION_DSU_CRC32_CAL
#define BOOTLOADER_FUNCTION_SPI_INIT
#define BOOTLOADER_FUNCTION_TINYDMA_GET_CHANNEL_CONFIG_DEFAULTS
#define BOOTLOADER_FUNCTION_TINYDMA_INIT
#define BOOTLOADER_FUNCTION_TINYDMA_START_TRANSFER
#define BOOTLOADER_FUNCTION_TINYDMA_DESCRIPTOR_GET_CONFIG_DEFAULTS
#define BOOTLOADER_FUNCTION_TINYDMA_DESCRIPTOR_INIT
#define BOOTLOADER_FUNCTION_TINYDMA_CHANNEL_INIT
#define BOOTLOADER_FUNCTION_AEABI_IDIV
#define BOOTLOADER_FUNCTION_AEABI_UIDIV
#define BOOTLOADER_FUNCTION_AEABI_IDIVMOD
#define BOOTLOADER_FUNCTION_AEABI_UIDIVMOD
#endif

// --- CLOCKS ---

#define SYSTEM_CLOCK_SOURCE_XOSC_FREQUENCY 0
#define SYSTEM_CLOCK_SOURCE_XOSC32K_FREQUENCY 0
#define SYSTEM_CLOCK_SOURCE_DPLL_FREQUENCY 0


#define SYSCTRL_FUSES_OSC32K_ADDR   (NVMCTRL_OTP4 + 4)
#define SYSCTRL_FUSES_OSC32K_Pos    6
#define SYSCTRL_FUSES_OSC32K_Msk    (0x7Fu << SYSCTRL_FUSES_OSC32K_Pos)
#define SYSCTRL_FUSES_OSC32K(value) ((SYSCTRL_FUSES_OSC32K_Msk & ((value) << SYSCTRL_FUSES_OSC32K_Pos)))

// System clock bus configuration
#define CONF_CLOCK_CPU_CLOCK_FAILURE_DETECT     false
#define CONF_CLOCK_FLASH_WAIT_STATES            2 // See 46.11.9. We need 2 FWS at 48Mhz
#define CONF_CLOCK_CPU_DIVIDER                  SYSTEM_MAIN_CLOCK_DIV_1
#define CONF_CLOCK_APBA_DIVIDER                 SYSTEM_MAIN_CLOCK_DIV_1
#define CONF_CLOCK_APBB_DIVIDER                 SYSTEM_MAIN_CLOCK_DIV_1
#define CONF_CLOCK_APBC_DIVIDER                 SYSTEM_MAIN_CLOCK_DIV_1

// SYSTEM_CLOCK_SOURCE_OSC8M configuration - Internal 8MHz oscillator
#define CONF_CLOCK_OSC8M_PRESCALER              SYSTEM_OSC8M_DIV_1
#define CONF_CLOCK_OSC8M_ON_DEMAND              true
#define CONF_CLOCK_OSC8M_RUN_IN_STANDBY         false

// SYSTEM_CLOCK_SOURCE_XOSC configuration - External clock/oscillator
#define CONF_CLOCK_XOSC_ENABLE                  false
#define CONF_CLOCK_XOSC_EXTERNAL_CRYSTAL        SYSTEM_CLOCK_EXTERNAL_CRYSTAL
#define CONF_CLOCK_XOSC_EXTERNAL_FREQUENCY      12000000UL
#define CONF_CLOCK_XOSC_STARTUP_TIME            SYSTEM_XOSC_STARTUP_32768
#define CONF_CLOCK_XOSC_AUTO_GAIN_CONTROL       true
#define CONF_CLOCK_XOSC_ON_DEMAND               true
#define CONF_CLOCK_XOSC_RUN_IN_STANDBY          false

// SYSTEM_CLOCK_SOURCE_XOSC32K configuration - External 32KHz crystal/clock oscillator
#define CONF_CLOCK_XOSC32K_ENABLE               false
#define CONF_CLOCK_XOSC32K_EXTERNAL_CRYSTAL     SYSTEM_CLOCK_EXTERNAL_CRYSTAL
#define CONF_CLOCK_XOSC32K_STARTUP_TIME         SYSTEM_XOSC32K_STARTUP_65536
#define CONF_CLOCK_XOSC32K_AUTO_AMPLITUDE_CONTROL  false
#define CONF_CLOCK_XOSC32K_ENABLE_1KHZ_OUPUT    false
#define CONF_CLOCK_XOSC32K_ENABLE_32KHZ_OUTPUT  true
#define CONF_CLOCK_XOSC32K_ON_DEMAND            true
#define CONF_CLOCK_XOSC32K_RUN_IN_STANDBY       false

// SYSTEM_CLOCK_SOURCE_OSC32K configuration - Internal 32KHz oscillator
#define CONF_CLOCK_OSC32K_ENABLE                true
#define CONF_CLOCK_OSC32K_STARTUP_TIME          SYSTEM_OSC32K_STARTUP_130
#define CONF_CLOCK_OSC32K_ENABLE_1KHZ_OUTPUT    true
#define CONF_CLOCK_OSC32K_ENABLE_32KHZ_OUTPUT   true
#define CONF_CLOCK_OSC32K_ON_DEMAND             true
#define CONF_CLOCK_OSC32K_RUN_IN_STANDBY        false

// SYSTEM_CLOCK_SOURCE_DFLL configuration - Digital Frequency Locked Loop
#define CONF_CLOCK_DFLL_ENABLE                  true
#define CONF_CLOCK_DFLL_LOOP_MODE               SYSTEM_CLOCK_DFLL_LOOP_MODE_OPEN
#define CONF_CLOCK_DFLL_ON_DEMAND               true

// DFLL open loop mode configuration
#define CONF_CLOCK_DFLL_FINE_VALUE              (512)

// DFLL closed loop mode configuration
#define CONF_CLOCK_DFLL_SOURCE_GCLK_GENERATOR   GCLK_GENERATOR_1
#define CONF_CLOCK_DFLL_MULTIPLY_FACTOR         (48000000 / 32768)
#define CONF_CLOCK_DFLL_QUICK_LOCK              true
#define CONF_CLOCK_DFLL_TRACK_AFTER_FINE_LOCK   true
#define CONF_CLOCK_DFLL_KEEP_LOCK_ON_WAKEUP     true
#define CONF_CLOCK_DFLL_ENABLE_CHILL_CYCLE      true
#define CONF_CLOCK_DFLL_MAX_COARSE_STEP_SIZE    (0x1f / 4)
#define CONF_CLOCK_DFLL_MAX_FINE_STEP_SIZE      (0xff / 4)

// SYSTEM_CLOCK_SOURCE_DPLL configuration - Digital Phase-Locked Loop
#define CONF_CLOCK_DPLL_ENABLE                  false
#define CONF_CLOCK_DPLL_ON_DEMAND               true
#define CONF_CLOCK_DPLL_RUN_IN_STANDBY          false
#define CONF_CLOCK_DPLL_LOCK_BYPASS             false
#define CONF_CLOCK_DPLL_WAKE_UP_FAST            false
#define CONF_CLOCK_DPLL_LOW_POWER_ENABLE        false

#define CONF_CLOCK_DPLL_LOCK_TIME               SYSTEM_CLOCK_SOURCE_DPLL_LOCK_TIME_DEFAULT
#define CONF_CLOCK_DPLL_REFERENCE_CLOCK         SYSTEM_CLOCK_SOURCE_DPLL_REFERENCE_CLOCK_XOSC32K
#define CONF_CLOCK_DPLL_FILTER                  SYSTEM_CLOCK_SOURCE_DPLL_FILTER_DEFAULT

#define CONF_CLOCK_DPLL_REFERENCE_FREQUENCY     32768
#define CONF_CLOCK_DPLL_REFERENCE_DIVIDER       1
#define CONF_CLOCK_DPLL_OUTPUT_FREQUENCY        48000000

// DPLL GCLK reference configuration */
#define CONF_CLOCK_DPLL_REFERENCE_GCLK_GENERATOR  GCLK_GENERATOR_1
// DPLL GCLK lock timer configuration */
#define CONF_CLOCK_DPLL_LOCK_GCLK_GENERATOR     GCLK_GENERATOR_1

// Set this to true to configure the GCLK when running clocks_init. If set to
// false, none of the GCLK generators will be configured in clocks_init().
#define CONF_CLOCK_CONFIGURE_GCLK               true

// Configure GCLK generator 0 (Main Clock)
#define CONF_CLOCK_GCLK_0_ENABLE                true
#define CONF_CLOCK_GCLK_0_RUN_IN_STANDBY        true
#define CONF_CLOCK_GCLK_0_CLOCK_SOURCE          SYSTEM_CLOCK_SOURCE_DFLL
#define CONF_CLOCK_GCLK_0_PRESCALER             1
#define CONF_CLOCK_GCLK_0_OUTPUT_ENABLE         false

// Configure GCLK generator 1
#define CONF_CLOCK_GCLK_1_ENABLE                false
#define CONF_CLOCK_GCLK_1_RUN_IN_STANDBY        false
#define CONF_CLOCK_GCLK_1_CLOCK_SOURCE          SYSTEM_CLOCK_SOURCE_XOSC32K
#define CONF_CLOCK_GCLK_1_PRESCALER             1
#define CONF_CLOCK_GCLK_1_OUTPUT_ENABLE         false

// Configure GCLK generator 2 (RTC)
#define CONF_CLOCK_GCLK_2_ENABLE                true
#define CONF_CLOCK_GCLK_2_RUN_IN_STANDBY        false
#define CONF_CLOCK_GCLK_2_CLOCK_SOURCE          SYSTEM_CLOCK_SOURCE_OSC32K
#define CONF_CLOCK_GCLK_2_PRESCALER             2
#define CONF_CLOCK_GCLK_2_OUTPUT_ENABLE         false

// Configure GCLK generator 3
#define CONF_CLOCK_GCLK_3_ENABLE                false
#define CONF_CLOCK_GCLK_3_RUN_IN_STANDBY        false
#define CONF_CLOCK_GCLK_3_CLOCK_SOURCE          SYSTEM_CLOCK_SOURCE_OSC8M
#define CONF_CLOCK_GCLK_3_PRESCALER             1
#define CONF_CLOCK_GCLK_3_OUTPUT_ENABLE         false

// Configure GCLK generator 4
#define CONF_CLOCK_GCLK_4_ENABLE                false
#define CONF_CLOCK_GCLK_4_RUN_IN_STANDBY        false
#define CONF_CLOCK_GCLK_4_CLOCK_SOURCE          SYSTEM_CLOCK_SOURCE_OSC8M
#define CONF_CLOCK_GCLK_4_PRESCALER             1
#define CONF_CLOCK_GCLK_4_OUTPUT_ENABLE         false

// Configure GCLK generator 5
#define CONF_CLOCK_GCLK_5_ENABLE                false
#define CONF_CLOCK_GCLK_5_RUN_IN_STANDBY        false
#define CONF_CLOCK_GCLK_5_CLOCK_SOURCE          SYSTEM_CLOCK_SOURCE_OSC8M
#define CONF_CLOCK_GCLK_5_PRESCALER             1
#define CONF_CLOCK_GCLK_5_OUTPUT_ENABLE         false

#endif
