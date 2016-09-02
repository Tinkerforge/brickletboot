/* brickletboot
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * firmware_entry.c: Entry function for firmware
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

#include "bricklib2/bootloader/bootloader.h"

#include "dsu_crc32.h"
#include "bootloader_spitfp.h"

__attribute__ ((section(".firmware_entry_func"))) void firmware_entry(BootloaderFunctions *bf) {
	bf->spitfp_tick = spitfp_tick;
	bf->spitfp_send_ack_and_message = spitfp_send_ack_and_message;
	bf->spitfp_is_send_possible = spitfp_is_send_possible;
	bf->dsu_crc32_cal = dsu_crc32_cal;
}
