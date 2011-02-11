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
	
	$Id: one_wire.c 41 2009-02-23 23:01:59Z musashi@todryk.pl $
*/

/** @file one_wire.c
	@brief Obsługa interfejsu 1-wire.
*/

#include "aquamat.h"
#include <util/delay.h>
#include "one_wire.h"

uint8_t last_discrepency;
uint8_t last_device_flag;

void onewire_init(void) {
	ONEWIRE_DDR &= ~_BV(ONEWIRE_PIN);
	ONEWIRE_PORTOUT &= ~_BV(ONEWIRE_PIN);
}

uint8_t onewire_present (void) {
	char i=0;
	onewire_0();	//stan niski
	_delay_us(90);
	_delay_us(90);
	_delay_us(90);
	_delay_us(90);
	_delay_us(90);
	_delay_us(30);
	onewire_1();  //stan wysoki
	_delay_us(65);
	if (!(onewire_get())) {
		i++;
	}
	_delay_us(90);
	_delay_us(90);
	_delay_us(90);
	_delay_us(90);
	_delay_us(90);
	_delay_us(30);
	if (!(onewire_get())) {
		i++;
	}
	return i;
}

void onewire_write_slot(uint8_t bit) {
   if(bit)
   {
      onewire_0();
      _delay_us(5);
      onewire_1();
      _delay_us(60);
   }
   else
   {
      onewire_0();
      _delay_us(60);
      onewire_1();
   }
	_delay_us(5);
}

uint8_t onewire_read_slot(void) {
	
	uint8_t bit=0;
   
	onewire_0();
    _delay_us(2);
    onewire_1();
    _delay_us(8);
	bit = onewire_get();
	_delay_us(65);
   	return bit; 
}

void onewire_write_byte(uint8_t byte) {
   uint8_t i;
   for(i=0; i<8; i++)
   {
      onewire_write_slot( byte & 0x01 );
      byte >>= 1;
   }
}

uint8_t onewire_read_byte(void)
{
   uint8_t i;
   uint8_t byte=0;
   
   for(i=1; i!=0; i<<=1)
   {                     
      	if( onewire_read_slot() ) {
			byte |= i;
		}
   }
   return byte;
}

void onewire_read_rom(uint8_t *buffer) {
	char i;
	onewire_write_byte(ONEWIRE_READ_ROM_COMMAND);
	for(i=0; i<8; i++) {
		*buffer++= onewire_read_byte();
	}
}

void onewire_skip_rom(void) {
	onewire_write_byte(ONEWIRE_SKIP_ROM_COMMAND);
}

void onewire_match_rom(uint8_t *buffer) {
	
	uint8_t i;

	onewire_write_byte(ONEWIRE_MATCH_ROM_COMMAND);

	for (i=0;i<8;i++) {
		onewire_write_byte(*buffer++);
	}	
}

uint8_t onewire_crc8(uint8_t *buffer, uint8_t len) {
   uint8_t crc=0;
   while (len--) {
      crc ^= *buffer++;
      crc ^= (crc << 3) ^ (crc << 4) ^ (crc << 6);
      crc ^= (crc >> 4) ^ (crc >> 5);
   }
   return crc;
}

uint8_t onewire_search_first (uint8_t *buffer) {
        last_discrepency = 0;
		last_device_flag = 0;
        return onewire_search(buffer);
}

uint8_t onewire_search_next(uint8_t *buffer) {
        return onewire_search(buffer);
}

uint8_t onewire_search(uint8_t *buffer) {
    
	uint8_t *orig_buffer = buffer;
	uint8_t id_bit_number = 1;
	uint8_t rom_byte_mask = 1;
	uint8_t last_zero = 0;
	uint8_t rom_byte_number = 0 ;
	uint8_t search_result,search_direction;
	uint8_t id_bit,cmp_id_bit;

	if (last_device_flag) {
		return FALSE;
	}
	
	if (onewire_present()!=1) {
		last_discrepency=0;
		last_device_flag=FALSE;
		return FALSE;
	}

	onewire_write_byte(ONEWIRE_SEARCH_ROM_COMMAND);
	
	do {
		id_bit =  onewire_read_slot();
		cmp_id_bit =  onewire_read_slot();

		if ((id_bit) && (cmp_id_bit)) {
			break;
		} else {
			if (id_bit != cmp_id_bit) {
					search_direction = id_bit;
			} else {
				if (id_bit_number < last_discrepency) {
					search_direction = ((*buffer & rom_byte_mask) > 0);
				} else {
					search_direction = (id_bit_number == last_discrepency);
				}
				
				if (search_direction == 0) {
					last_zero = id_bit_number;
				}
			}
			
			if (search_direction) {
				*buffer |= rom_byte_mask;
			} else {
				*buffer &= ~rom_byte_mask;
			}
			onewire_write_slot(search_direction);
			id_bit_number++;
			rom_byte_mask <<=1;
			
			if (rom_byte_mask == 0) {
				buffer++;
				rom_byte_number++;
				rom_byte_mask = 1;
			}	
		}
	} while (rom_byte_number < 8);

	if (!(id_bit_number < 65 )) {
		last_discrepency = last_zero;
		if (last_discrepency == 0) {
				last_device_flag = TRUE;
		}
		search_result = TRUE;
	}

	if (!search_result || !(*orig_buffer)) {
		last_discrepency = 0;
		last_device_flag = FALSE;
		search_result = FALSE;
	}

	return search_result;
}
