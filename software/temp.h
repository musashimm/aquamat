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
	
	$Id: temp.h 41 2009-02-23 23:01:59Z musashi@todryk.pl $
*/

/** @file temp.h
	@brief Plik nagłówkowy - Implementacja kontroli temperatury.
*/

#define TEMP_SENSOR_ROLE_0 0
#define TEMP_SENSOR_ROLE_1 1
#define TEMP_SENSOR_STATUS_OK 2
#define TEMP_SENSOR_CONVERT 3
#define TEMP_SENSOR_HEATING 4
#define TEMP_SENSOR_COOLING 5
#define TEMP_ALARM_MIN 6
#define TEMP_ALARM_MAX 7
#define TEMP_ALARM_MASK (_BV(TEMP_ALARM_MIN)|_BV(TEMP_ALARM_MAX))

#define TEMP_CLEAR_ALL_FLAGS(a)  (temp_sensors[a].flags &= ~(_BV(TEMP_SENSOR_HEATING)|_BV(TEMP_SENSOR_COOLING)|_BV(TEMP_ALARM_MAX)|_BV(TEMP_ALARM_MIN)|_BV(TEMP_SENSOR_STATUS_OK)|_BV(TEMP_SENSOR_CONVERT)))
#define TEMP_CLEAR_OPER_FLAGS(a) (temp_sensors[a].flags &= ~(_BV(TEMP_SENSOR_HEATING)|_BV(TEMP_SENSOR_COOLING)|_BV(TEMP_ALARM_MAX)|_BV(TEMP_ALARM_MIN)));

#define CHKF(a,f) (temp_sensors[a].flags & _BV(f))

#define TEMP_SENSOR_BAD_READS_FAILURE 3
#define TEMP_SENSOR_MAX_BAD_READS 0xFF
#define TEMP_SENSOR_READ_INTERVAL 15

/** Liczba obsługiwanych czujników
*/
#define TEMP_SENSORS_NUM 4

#define TEMP_SENSOR_ROLE_MASK (_BV(TEMP_SENSOR_ROLE_0)|_BV(TEMP_SENSOR_ROLE_1))

#define TEMP_SENSOR_ROLE_INACTIVE 3

#define DS18B20_READ_SCRATCHPAD		0xBE
#define DS18B20_ONEWIRE_CONVERT_T	0x44
#define IS_PRESENT 1


/** @struct TEMP_SENSOR
	@brief Struktura przechowywująca dane o czujnikach.
*/
struct TEMP_SENSOR {
	uint8_t rom[8]; 		/**< Adres rom.*/
	TEMP temp;			/**< Aktualna temperatura.*/
	TEMP target;		/**< Docelowa temperatura.*/
	TEMP target_night;	/**< Docelowa nocna temperatura.*/
	TEMP max;			/**< Maksymalna dopuszczalna temperatura.*/
	TEMP min;			/**< Minimalna dopuszczalna temperatura.*/
	uint8_t histeresis;		/**< Histereza.*/
	/** Flagi.
	@verbatim
7654|3210
    |  XX - stan czujnika
    |  11 - nieaktywny (TEMP_SENSOR_ROLE_INACTIVE)
    | X   - uszkodzony czyjnik ?
    | 0   - uszkodzony
    | 1   - nieuszkodzony
    |X    - czy byl zrobiony ds_convert?
    |0    - nie byl
    |1    - byl
   X|     - grzanie wlaczone?
   0|     - wyl.
   1|     - wl.
  X |     - chlodzenie wlaczone?
  0 |     - wyl.
  1 |     - wl.
 X  |     - alarm min ?
 0  |     - nie ma
 1  |     - jest
X   |     - alarm max ?
0   |     - nie ma
1   |     - jest
	@endverbatim
	*/
	uint8_t flags;			
	uint8_t bad_reads;		/**< Licznik nieudanych odczytów.*/
	uint8_t out_heating;	/**< Przypisane wyjscie do ogrzewania.*/
	uint8_t out_cooling;	/**< Przypisane wyjście do chłodzenia.*/
};

void temp_read_scratchpad(uint8_t*);
void temp_read_temperature(void);
void temp_set_heating(uint8_t,uint8_t);
void temp_set_cooling(uint8_t,uint8_t);
void temp_discover_sensors (void);
uint8_t temp_ds_to_dec(TEMP);
uint8_t temp_dec_to_ds(uint8_t);
void temp_register(void);
void temp_set_next_convert(uint8_t);
void temp_badreads(int8_t);
void temp_check_tresholds(uint8_t);
void temp_set_daynight(uint8_t);
void temp_set_outs_alarms(void);
void temp_control_outs(uint8_t);
void temp_set_alarms(void);
void temp_set_failure(uint8_t,uint8_t);

extern struct TEMP_SENSOR temp_sensors[];
