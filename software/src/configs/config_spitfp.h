/* brickletboot
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_spitfp.h: SPITFP protocol configuration
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

#ifndef CONFIG_SPITFP_H
#define CONFIG_SPITFP_H

#define SPITFP_SPI_MODULE             SERCOM0
#define SPITFP_SPI_SIGNALMUX_SETTING  SPI_SIGNAL_MUX_SETTING_I

#define SPITFP_PINMUX_PAD0            PINMUX_PA04D_SERCOM0_PAD0 // MOSI
#define SPITFP_PINMUX_PAD1            PINMUX_PA05D_SERCOM0_PAD1 // CLK
#define SPITFP_PINMUX_PAD2            PINMUX_PA06D_SERCOM0_PAD2 // SS
#define SPITFP_PINMUX_PAD3            PINMUX_PA07D_SERCOM0_PAD3 // MISO

#define SPITFP_PERIPHERAL_TRIGGER_TX  SERCOM0_DMAC_ID_TX
#define SPITFP_PERIPHERAL_TRIGGER_RX  SERCOM0_DMAC_ID_RX

#define SPITFP_RECEIVE_BUFFER_SIZE    1024

#endif
