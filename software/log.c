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
	
	$Id$
*/

#include <avr/wdt.h>
#include "aquamat.h"
#include "twi.h"
#include "twi_addresses.h"
#include "log.h"
#include "strings.h"
#include "usart.h"
#include "hd44780.h"

extern uint8_t minutes;
extern uint8_t hours;
extern uint8_t temp;
extern uint8_t year;
extern uint8_t month;
extern uint8_t day;
extern uint8_t wday;

/** @var uint8_t logbuff[8] 
	@brief Bufor do przechowania zdarzenia z logu
*/
uint8_t logbuff[8];

/** Zwraca wskaźnik
* @return wartość wskaźnika
*/
uint16_t log_read_pointer(void) {
	uint16_t logp;
	twi_send_start();
	twi_write_byte(EXT_EEPROM_ADDR & 0xFE);
	twi_write_byte(FIRST_WORD(LOG_CONTROL_RECORD_NUMBER) + LOG_CONTROL_POINTER_LSB);
	twi_write_byte(SECOND_WORD(LOG_CONTROL_RECORD_NUMBER));
	twi_send_start();
	twi_write_byte(EXT_EEPROM_ADDR);
	twi_read_byte(TRUE);
	logp = twi_byte;
	twi_read_byte(FALSE);
	logp |= ((uint16_t)twi_byte) << 8;
	return logp;
}

/** Ustawia wskaźnik wpisu
* @param pointer wartość wskaźnika
*/
void log_write_pointer(uint16_t pointer) {
	twi_send_start();
	twi_write_byte(EXT_EEPROM_ADDR & 0xFE);
	twi_write_byte(FIRST_WORD(LOG_CONTROL_RECORD_NUMBER) + LOG_CONTROL_POINTER_LSB);
	twi_write_byte(SECOND_WORD(LOG_CONTROL_RECORD_NUMBER));
	twi_write_byte((uint8_t)pointer);
	twi_write_byte((uint8_t)(pointer >> 8));
	twi_send_stop();
	_delay_ms(LOG_WRITE_DELAY);
}

/** Wpisuje kolejne zdarzenie do logu.
* @param type typ wpisu
* @param val1 warotść 1
* @param val2 warotść 2
* @param val3 warotść 3
*/
void log_write_record(uint8_t type,uint8_t val1,uint8_t val2, uint8_t val3) {
	uint16_t logp = log_read_pointer();
	if (logp > LOG_LAST_RECORD_NUMBER || !logp ) {	
		logp = LOG_FIRST_RECORD_NUMBER;
	}
	log_write_record_at_pointer(logp,type,val1,val2,val3);
	log_write_pointer(++logp);
}

/** Wpisuje kolejne zdarzenie do logu we wskazanym przez wskaźnik miejscu.
* @param logp wskaźnik rekordu
* @param type typ wpisu
* @param val1 warotść 1
* @param val2 warotść 2
* @param val3 warotść 3
*/
void log_write_record_at_pointer(uint16_t logp,uint8_t type,uint8_t val1,uint8_t val2, uint8_t val3) {
	twi_send_start();
	twi_write_byte(EXT_EEPROM_ADDR & 0xFE);
	twi_write_byte(FIRST_WORD(logp));
	twi_write_byte(SECOND_WORD(logp));
	twi_write_byte(year);
	twi_write_byte( month << 4 | (type & 0x0F) );
	twi_write_byte(day);
	twi_write_byte(hours);
	twi_write_byte(minutes);
	twi_write_byte(val1);
	twi_write_byte(val2);
	twi_write_byte(val3);
	twi_send_stop();
	_delay_ms(LOG_WRITE_DELAY);
}

/** Czyści rekord w logu
* @param logp wartość wskaźnika
*/
void log_clear_record (uint16_t logp) {
	twi_send_start();
	twi_write_byte(EXT_EEPROM_ADDR & 0xFE);
	twi_write_byte(FIRST_WORD(logp));
	twi_write_byte(SECOND_WORD(logp));
	twi_write_byte(UINT8_T_DISABLED);
	twi_send_stop();
	_delay_ms(LOG_WRITE_DELAY);
}

/** Czyści cały log
*/
void log_clear(void) {
	uint16_t i;
	uint8_t j = 0;
	for (i=LOG_FIRST_RECORD_NUMBER;i<=LOG_LAST_RECORD_NUMBER;i++) {
		hd44780_home();
		hd44780_outstrn_P(PSTR(" Czyszcz. loga  "));
		hd44780_goto(2,1);
		hd44780_outstrn_P(PSTR(" Rek.  "));
		hd44780_out16dec(i);
		hd44780_outstrn_P(PSTR("/"));
		hd44780_out16dec(LOG_LAST_RECORD_NUMBER);
		if (++j > LOG_RECORD_CLEAR_MAX) {
			j=0;
			wdt_reset();
		}
		log_clear_record(i);
	}
	log_write_pointer(LOG_CONTROL_RECORD_NUMBER);
}

/** Czyta wpis z loga
* @param logp wartość wskaźnika
* @param *pbuff wskaźnik do bufora, gdzie zostanie zapisany wpis
*/
void log_read_record_at_pointer(uint16_t logp,uint8_t *pbuff) {
	
	uint8_t i;
	
	twi_send_start();
	twi_write_byte(EXT_EEPROM_ADDR & 0xFE);
	twi_write_byte(FIRST_WORD(logp));
	twi_write_byte(SECOND_WORD(logp));
	twi_send_start();
	twi_write_byte(EXT_EEPROM_ADDR);
	for(i=0;i<LOG_BYTES_PER_RECORD-1;i++) { 	// 7 bajtów z potwierdzeniem
		twi_read_byte(TRUE);
		*pbuff++ = twi_byte;
	}
	twi_read_byte(FALSE);						// 1 bajt bez potwierdzenia
	*pbuff = twi_byte;
	twi_send_stop();
}

/** @file log.c
@brief Implementacja loga systemowego.
 
Cała dostępna pamięć loga podzielona jest na 8 bajtowe rekordy. Przykładowo dla pamięci 24c64 (o pojemności 64k) możliwe jest dokonanie 1024 rekordów do logu od numeru 0 do 1023. Rekord o numerze 0 jest rekordem kontrolnym.

Wskaźnik 16-bitowy do wpisu przechowywany jest domyślnie w 6 i 7 bajcie rekordu kontrolnego (\ref LOG_CONTROL_POINTER_LSB i \ref LOG_CONTROL_POINTER_MSB).

@verbatim
 0| 1| 2| 3| 4| 5| 6| 7| - pierwsze 8 bajtów w pamięci
                  XX     - LOG_CONTROL_POINTER_LSB (mniej znacząca cześć wskaźnika)
                     XX  - LOG_CONTROL_POINTER_MSB (bardziej znacząca cześć wskaźnika)
@endverbatim

Rekord przechowujący zdarzenia:

@verbatim
 00|01|02|03|04|05|06|07| - rekord (wpis w logu)   8 bajtów
 XX                       - rok
    X                     - miesiąc
     X                    - typ                    możliwe 16 typów zdarzeń
       XX                 - dzień
          XX              - godzina
             XX           - minuty
                X         - wartość1/2             bit 7 określa czy zdarzenie się rozpoczyna czy kończy
                 X        - wartość2/2             zawiera identyfikator elementu wywołujacego zdarzenie
                   XX     - wartosc2               dodatkowe informacje
                      XX  - wartosc3               dodatkowe informacje
@endverbatim
Typy zdarzeń wymienione są w pliku ze wspólnymi definicjami \ref commonDefs.h
*/
