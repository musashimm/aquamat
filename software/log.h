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

#ifndef LOG_MODULE_H
#define LOG_MODULE_H

/** @file log.h
	@brief Plik nagłówkowy - Implementacja loga systemowego.
*/

/** Wartość przy ilu rekordach wyzerować patrzypiesa.
*/
#define LOG_RECORD_CLEAR_MAX 25

/** Liczba dostępnych rekordów w logu. Dla pamieci 24c64 jest to 1023 plus rekord kontrolny. 
*/
#define LOG_LAST_RECORD_NUMBER 1023
#define LOG_FIRST_RECORD_NUMBER 1
#define LOG_CONTROL_RECORD_NUMBER 0
#define LOG_BYTES_PER_RECORD 8
#define LOG_CONTROL_POINTER_LSB 6
#define LOG_CONTROL_POINTER_MSB 7
#define LOG_WRITE_DELAY 10

#define FIRST_WORD(addr) ((uint8_t)((addr*LOG_BYTES_PER_RECORD)>>8))
#define SECOND_WORD(addr) ((uint8_t)(addr*LOG_BYTES_PER_RECORD))

uint16_t log_read_pointer(void);
void log_read_record_at_pointer(uint16_t,uint8_t *);
void log_clear_record (uint16_t);
void log_clear(void);
void log_write_pointer(uint16_t);
void log_write_record(uint8_t,uint8_t,uint8_t, uint8_t);
void log_write_record_at_pointer(uint16_t,uint8_t,uint8_t,uint8_t,uint8_t);

extern uint8_t logbuff[8];

#endif
