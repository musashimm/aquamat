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
	
	$Id: rtc.h 46 2009-03-19 21:19:39Z musashi@todryk.pl $
*/

/** @file rtc.h
	@brief Plik nagłówkowy - Obsługa zegara czasu rzeczywistego RTC.
*/

#define RTC_SEC_OFFSET	0x02
#define RTC_DAY_OFFSET	0x05

void rtc_set_time(uint8_t,uint8_t,uint8_t);
void rtc_set_date(uint8_t,uint8_t,uint8_t,uint8_t);
void rtc_get_time(void);
void rtc_get_date(void);



