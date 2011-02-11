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
	
	$Id: rtc.c 46 2009-03-19 21:19:39Z musashi@todryk.pl $
*/

/** @file rtc.c
	@brief Obsługa zegara czasu rzeczywistego RTC.
*/

#include <util/twi.h>
#include "aquamat.h"
#include "twi.h"
#include "twi_addresses.h"
#include "rtc.h"

extern uint8_t minutes;
extern uint8_t hours;
extern uint8_t temp;
extern uint8_t year;
extern uint8_t month;
extern uint8_t day;
extern uint8_t wday;

void rtc_set_time (uint8_t h, uint8_t m, uint8_t s) {
	twi_send_start();
	twi_write_byte(RTC_ADDR & 0xFE);
	twi_write_byte(RTC_SEC_OFFSET);
	twi_write_byte(((s/10) << 4) | (s%10));
	twi_write_byte(((m/10) << 4) | (m%10));
	twi_write_byte(((h/10) << 4) | (h%10));
	twi_send_stop();
}

void rtc_set_date (uint8_t y, uint8_t m, uint8_t d,uint8_t wd) {
	twi_send_start();
	twi_write_byte(RTC_ADDR & 0xFE);
	twi_write_byte(RTC_DAY_OFFSET);
	twi_write_byte(((d/10) << 4) | (d%10));
	twi_write_byte(wd);
	twi_write_byte(((m/10) << 4) | (m%10));
	twi_write_byte(((y/10) << 4) | (y%10));
	twi_send_stop();
}

void rtc_get_time (void) {
	twi_send_start();
	twi_write_byte(RTC_ADDR & 0xFE);
	twi_write_byte(RTC_SEC_OFFSET);
	twi_send_start();
	twi_write_byte(RTC_ADDR);
	twi_read_byte(TRUE);
	seconds=((twi_byte&0x7f)>>4)*10 + (twi_byte&0xf);
	twi_read_byte(TRUE);
	minutes=((twi_byte&0x7f)>>4)*10 + (twi_byte&0xf);
	twi_read_byte(FALSE);
	hours=((twi_byte&0x3f)>>4)*10 + (twi_byte&0xf);
	twi_send_stop();
}

void rtc_get_date (void) {
	twi_send_start();
	twi_write_byte(RTC_ADDR & 0xFE);
	twi_write_byte(RTC_DAY_OFFSET);
	twi_send_start();
	twi_write_byte(RTC_ADDR);
	twi_read_byte(TRUE);
	day=((twi_byte&0x3f)>>4)*10 + (twi_byte&0xf);
	twi_read_byte(TRUE);
	wday=twi_byte&0x07;
	twi_read_byte(TRUE);
	month=((twi_byte&0x1f)>>4)*10 + (twi_byte&0xf);
	twi_read_byte(FALSE);
	year=((twi_byte)>>4)*10 + (twi_byte&0xf);
	twi_send_stop();
}

