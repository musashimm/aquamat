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

	$Id:$
*/

/** @file twi_addresses.h
@brief Definicje adresów elementów magistrali TWI.

Adresy elementów interfejsu TWI podawane są w postaci do odczutu (bit najmniej znaczący równy 1).
Żeby zapisać dane należy ostatni bit w adresie wyzerować (& 0xFE).

@verbatim

Funkcja     Układ       Adres wzorzec      Adres w sprzęcie
Wyjście 1   PCF8574AP	0b0111-A2A1A0(R/W) 0b01110011/0x73
Wyjście 2   PCF8574AP   0b0111-A2A1A0(R/W) 0b01110101/0x75
RTC         PCF8563     0b1010-001(R/W)    0b10100011/0xA3
Zew. EEPROM AT24C64     0b1010-A2A1A0(R/W) 0b10101001/0xA9

@endverbatim
*/

#define OUTPUTS_DEVICE_1_ADDR 0x73
#define OUTPUTS_DEVICE_2_ADDR 0x75
#define RTC_ADDR 0xA3
#define EXT_EEPROM_ADDR 0xA9
