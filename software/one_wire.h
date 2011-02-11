/*
    Aquamat - Sterownik akwariowy Open Source
    Copyright (C) 2009 Wojciech Todryk (wojciech@todryk.pl)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	$Id: one_wire.h 41 2009-02-23 23:01:59Z musashi@todryk.pl $
*/

/** @file one_wire.h
	@brief Plik nagłówkowy - Obsługa interfejsu 1-wire.
*/

#define ONEWIRE_PORT C
#define ONEWIRE_PIN PORT2

#define ONEWIRE_PORTOUT PORT(ONEWIRE_PORT)
#define ONEWIRE_PORTIN  PIN(ONEWIRE_PORT)
#define ONEWIRE_DDR     DDR(ONEWIRE_PORT)

#define onewire_0() ONEWIRE_DDR |= _BV(ONEWIRE_PIN)
#define onewire_1() ONEWIRE_DDR &= ~_BV(ONEWIRE_PIN)
#define onewire_get() ONEWIRE_PORTIN & _BV(ONEWIRE_PIN)

#define ONEWIRE_READ_ROM_COMMAND 0x33
#define ONEWIRE_SKIP_ROM_COMMAND 0xCC
#define ONEWIRE_SEARCH_ROM_COMMAND 0xF0
#define ONEWIRE_MATCH_ROM_COMMAND 0x55

void onewire_init(void);
uint8_t onewire_present (void);
void onewire_write_slot(uint8_t);
uint8_t onewire_read_slot(void);
void onewire_write_byte(uint8_t);
uint8_t onewire_read_byte(void);
uint8_t onewire_crc8(uint8_t*,uint8_t);
void onewire_read_rom(uint8_t*);
void onewire_skip_rom(void);
void onewire_match_rom(uint8_t *);
uint8_t onewire_search_first (uint8_t*);
uint8_t onewire_search_next(uint8_t*);
uint8_t onewire_search(uint8_t*);

extern uint8_t last_discrepency;
extern uint8_t last_device_flag;
