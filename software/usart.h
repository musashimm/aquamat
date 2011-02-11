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

	$Id: usart.h 48 2009-03-27 21:37:50Z musashi@todryk.pl $
*/

/** @file usart.h
	@brief Plik nagłówkowy - Obsuga interfejsu szeregowego.
*/

/** Szybko transmisji
*/
#define BAUDRATE 19200

/** Warto rejestru UBRR
*/
#define UBRR_VALUE ((F_CPU/(BAUDRATE*16UL))-1)

void usart_init(unsigned int);
void usart_transmit(unsigned char);
unsigned char usart_receive (void);
void usart_outstrn_P(const char*);
int usart_receive_buffer_full(void);
void usart_out8dec(const uint8_t i);
void usart_out16dec(const uint16_t i);
void usart_out4dec(const uint8_t);
void usart_out4hex(const uint8_t);
void usart_out8hex(const uint8_t);

//void usart_format_time(struct MIT);
//void usart_formated_temp (uint16_t);
//void usart_formated_log_entry(uint8_t*);
//void usart_outstrn_P_par(const char *);

