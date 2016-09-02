/* gps-v2-bricklet
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_logging.c: Logging configuration for GPS Bricklet 2.0
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

#ifndef CONFIG_LOGGING_H
#define CONFIG_LOGGING_H

//#define LOGGING_SERIAL
//#define LOGGING_LEVEL LOGGING_DEBUG
#define LOGGING_LEVEL LOGGING_NONE

#include "spi.h" // PINMUX_UNUSED


#define LOGGING_BAUDRATE    115200
#define LOGGING_MUX_SETTING USART_RX_0_TX_2_XCK_3
#define LOGGING_PINMUX_PAD0 PINMUX_UNUSED
#define LOGGING_PINMUX_PAD1 PINMUX_UNUSED
#define LOGGING_PINMUX_PAD2 PINMUX_PA16C_SERCOM1_PAD2
#define LOGGING_PINMUX_PAD3 PINMUX_UNUSED
#define LOGGING_SERCOM      SERCOM1

#endif
