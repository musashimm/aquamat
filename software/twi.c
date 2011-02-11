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
	
	$Id: twi.c 41 2009-02-23 23:01:59Z musashi@todryk.pl $
*/

/** @file twi.c
	@brief Obsługa interfejsu TWI.
*/

#include "aquamat.h"
#include <util/twi.h>
#include "twi.h"

uint8_t twi_byte;

void twi_init (void) {
#if defined(TWPS0)
	TWSR = 0;
#endif
#if F_CPU < 3600000UL
	TWBR = 10;
#else
	TWBR = (F_CPU / 100000UL - 16) / 2;
#endif	
}

uint8_t twi_send_start (void) {
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0);
	return TW_STATUS;
}

uint8_t twi_send_address (uint8_t sla) {
	TWDR = sla;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0);
	return TW_STATUS; 
}

uint8_t twi_read_byte(uint8_t send_ack) {
	if (send_ack) {
		TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	} else {
		TWCR = _BV(TWINT) | _BV(TWEN);
	}
	while ((TWCR & _BV(TWINT)) == 0);
	twi_byte = TWDR;
	return TW_STATUS;
}

uint8_t twi_write_byte (uint8_t byte) {
	TWDR = byte;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0);
	return TW_STATUS; 
}

void twi_send_stop(void) {
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}
