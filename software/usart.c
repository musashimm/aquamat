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

	$Id: usart.c 48 2009-03-27 21:37:50Z musashi@todryk.pl $
*/

/** @file usart.c
	@brief Obsługa interfejsu szeregowego.
*/

#include "aquamat.h"
#include "usart.h"
#include "strings.h"
#include "outputs.h"
#include "log.h"
#include "temp.h"

void usart_init(unsigned int ubrr) {
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	UCSRB = _BV(RXEN)|_BV(TXEN)|_BV(RXCIE);
	//UCSRB = _BV(RXEN)|_BV(TXEN);
	UCSRC = _BV(URSEL)|_BV(UCSZ1)|_BV(UCSZ0);
}

void usart_transmit( unsigned char data ) {
	while( !(UCSRA & _BV(UDRE)) );
	UDR = data;
}

unsigned char usart_receive (void) {
	while(!(UCSRA & _BV(RXC)));
	return UDR;
}

int usart_receive_buffer_full(void) {
	return UCSRA & _BV(RXC);
}

void usart_outstrn_P(const char *s)
{
	register char c;

	while ((c = pgm_read_byte(s++))) {
		if (c == '\n') {
			usart_transmit(0x0a);
			usart_transmit(0x0d);
			continue;
		}
		usart_transmit(c);
	}
}

// void usart_outstrn_P_par(const char *s) {
// 	usart_outstrn_P(PAR_S);
// 	usart_outstrn_P(s);
// }

void usart_out8dec(const uint8_t i) {
	usart_transmit((i/10)+0x30);
	usart_transmit((i%10)+0x30);
}

void usart_out16dec(const uint16_t i) {
	usart_out8dec(i/100);
	usart_out8dec(i%100);
}

void usart_out4dec(const uint8_t i) {
	usart_transmit(i+0x30);
}

void usart_out4hex(const uint8_t i) {
	i > 9 ? usart_transmit((i)-10+0x41) : usart_transmit((i)+0x30);
}

void usart_out8hex(const uint8_t i) {
	usart_out4hex(i >> 4);
	usart_out4hex(i & 0x0f);
}

// void usart_format_time(struct MIT mit) {
// 	usart_outstrn_P(SPACE_S);
// 	if (mit.hours < 10) {
// 		usart_outstrn_P(SPACE_S);
// 		usart_out4dec(mit.hours);
// 	} else if (mit.hours > 23) {
// 		usart_outstrn_P(UINT8_BAD_S);
// 	} else {
// 		usart_out8dec(mit.hours);
// 	}
// 	usart_outstrn_P(COLON_S);
// 	if (mit.minutes > 59) {
// 		usart_outstrn_P(UINT8_BAD_S);
// 	} else {
// 		usart_out8dec(mit.minutes);
// 	}
// 	usart_outstrn_P(SPACE_S);
// }

// void usart_formated_temp (uint16_t t) {
//
//         uint8_t temp;
//
// 		if (t == UINT16_T_DISABLED) {
// 			return usart_outstrn_P(STATUS_NOTACTIVE_S);
// 		}
//
//         temp = (uint8_t)(t >> 4) ;
//
//         if (temp/10) {
//                 usart_out4hex(temp/10);
//         } else {
//                 usart_outstrn_P(SPACE_S);
//         }
//         usart_out4hex(temp%10);
//         usart_outstrn_P(DOT_S);
//
//         usart_out4hex(temp_ds_to_dec(t));
//         usart_outstrn_P(PSTR(" st. C "));
// }

// void usart_formated_log_entry(uint8_t* pbuff) {
//
// 	uint8_t type;
//
// 	usart_outstrn_P(CENTURY_S);
// 	usart_out8dec(*pbuff);
// 	usart_outstrn_P(SLASH_S);
//
// 	pbuff++;
// 	usart_out8dec((*pbuff & 0xF0) >> 4);
// 	type = *pbuff & 0x0F;
// 	usart_outstrn_P(SLASH_S);
//
// 	pbuff++;
// 	usart_out8dec(*pbuff);
// 	usart_outstrn_P(SPACE_S);
//
// 	pbuff++;
// 	usart_out8dec(*pbuff);
// 	usart_outstrn_P(COLON_S);
//
// 	pbuff++;
// 	usart_out8dec(*pbuff);
// 	usart_outstrn_P(SPACE_S);
//
// 	switch (type) {
// 		case LOG_EVENT_START: 				usart_outstrn_P(START_S);
// 											break;
// 		case LOG_EVENT_ALARM_MAX:			usart_outstrn_P(ALARM_S);
// 											usart_outstrn_P(MAX_S);
// 											break;
// 		case LOG_EVENT_ALARM_MIN:			usart_outstrn_P(ALARM_S);
// 											usart_outstrn_P(MIN_S);
// 											break;
// 		case LOG_EVENT_ALARM_SENSOR_FAILURE:usart_outstrn_P(ALARM_S);
// 											usart_outstrn_P(FAILURE_S);
// 											break;
// 		case LOG_EVENT_TOP_OFF_MAX_RUN:usart_outstrn_P(ALARM_S);
// 											usart_outstrn_P(TOP_OFF_MAX_RUN_S);
// 											break;
// 		default:							usart_outstrn_P(UNKNOWN_S);
// 	}
// 	pbuff++;
// 	if (*pbuff) {
// 		usart_outstrn_P(ON_S);
// 	} else {
// 		usart_outstrn_P(OFF_S);
// 	}
// 	usart_outstrn_P(N_S);
// }
