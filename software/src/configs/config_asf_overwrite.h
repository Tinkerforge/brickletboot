/* brickletboot
 * Copyright (C) 2016 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_interrupt.h: Interrupt handling configuration
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

#ifndef CONFIG_INTERRUPT_H
#define CONFIG_INTERRUPT_H

// In bootloader we can't use recursive irq handling,
// since it needs global RAM.
// All of the clock configuration in bootloader is done right at the startup
// when there is no relevant IRQ anyways.
#define DISABLE_RECURSIVE_IRQ_HANDLING

// In bootloader mode, the sercom generator was not set before.
// We don't need to save this info in a global variable
#define SERCOM_CONFIG_GENERATOR_NEVER_SET

#endif
