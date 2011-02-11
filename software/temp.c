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

#include "aquamat.h"
#include "one_wire.h"
#include "temp.h"
#include "outputs.h"
#include "timers.h"
#include "alarms.h"
#include "log.h"
#include "strings.h"
#include "usart.h"
#include "hd44780.h"

PROGMEM const uint8_t TEMP_CONVERT_DEC_TO_DS[] = {
	0,	//0
	2, 	//1
	4, 	//2
	5, 	//3
	7, 	//4
	8, 	//5
	10, //6
	12, //7
	13, //8
	15,	//9
};

PROGMEM const uint8_t TEMP_CONVERT_DS_TO_DEC[] = {
	0,	//0
	1, 	//1
	1, 	//2
	2, 	//3
	2, 	//4
	3, 	//5
	4, 	//6
	4, 	//7
	5, 	//8
	6,	//9
	6,	//A
	7,	//B
	7,	//C
	8,	//D
	9,	//E
	9	//F
};

/** @var struct TEMP_SENSOR temp_sensors[TEMP_SENSORS_NUM]
	Przechowuje struktury czujników.
*/
struct TEMP_SENSOR temp_sensors[TEMP_SENSORS_NUM];

uint8_t arr[9];
uint16_t temp_var;

/** Odczytuje dane ze pamięci czujnika.
	@param buffer bufor, do którego zapisywane są dane
*/
void temp_read_scratchpad(uint8_t* buffer) {
	char i;
	onewire_write_byte(DS18B20_READ_SCRATCHPAD);
	for(i=0; i<9; i++) {
		*buffer++= onewire_read_byte();
	}
}

/** Wysterowuje wyjścia w zależności od ustwień flag.
	@param s index sensora
*/
void temp_set_outs (uint8_t s) {
	if (temp_sensors[s].out_cooling != UINT8_T_DISABLED) {
		if (temp_sensors[s].flags & _BV(TEMP_SENSOR_COOLING)) {
			output_switch(temp_sensors[s].out_cooling,ON);
		} else {
			output_switch(temp_sensors[s].out_cooling,OFF);
		}
	}
	
	if (temp_sensors[s].out_heating != UINT8_T_DISABLED) {
		if (temp_sensors[s].flags & _BV(TEMP_SENSOR_HEATING)) {
			output_switch(temp_sensors[s].out_heating,ON);
		} else {
			output_switch(temp_sensors[s].out_heating,OFF);
		}
	}	
}

/** Konwertuje dzisiętne części temperatury z formatu ds na format dziesiętny.
	@param t zmienna przechowujaca temperaturę
	@return bajt z temperaturą dziesiętną
*/
uint8_t temp_ds_to_dec(TEMP t) {
	//return (625*(uint8_t)(t & 0x0F)/1000);
	return pgm_read_byte(&TEMP_CONVERT_DS_TO_DEC[(uint8_t)(t & 0x0F)]);
}

/** Konwertuje dzisiętne części temperatury z formatu dziesiętnego na format ds.
	@param dec zmienna przechowujaca temperaturę
	@return bajt z temperaturą ds
*/
uint8_t temp_dec_to_ds(uint8_t dec) {
	//return dec*1000/625;
	return pgm_read_byte(&TEMP_CONVERT_DEC_TO_DS[dec]);
}

/** Czyta temperaturę z czujnika
@param id indentyfikator czujnika
*/
void temp_read_temp_from_ds(uint8_t id) {
	if(onewire_present()==IS_PRESENT) {
		onewire_match_rom(&(temp_sensors[id].rom[0]));
		temp_read_scratchpad(&arr[0]);
		if (onewire_crc8(&arr[0],8)==arr[8] ) {			// crc poprawne
			if (temp_sensors[id].flags & _BV(TEMP_SENSOR_CONVERT)) {
				temp_var=arr[1];
				temp_var=temp_var<<8|arr[0];
				temp_sensors[id].temp = temp_var;
				temp_set_failure(id,OFF);
			}
			temp_set_next_convert(id);
		} else {
			temp_badreads(id);
		}
	} else {
		temp_set_failure(id,ON);
	}
}

/** Ustawia kolejna konwersję temperatury
@param id indentyfikator czujnika
*/
void temp_set_next_convert(uint8_t id) {
	if(onewire_present()==IS_PRESENT) {
		onewire_match_rom(&(temp_sensors[id].rom[0]));
		onewire_write_byte(DS18B20_ONEWIRE_CONVERT_T);
		temp_sensors[id].flags |= _BV(TEMP_SENSOR_CONVERT);
	} else {
		temp_set_failure(id,ON);
	}
}

/** Czyta temperaturę z czujników i składuje ją w strukturze \ref TEMP_SENSOR.
*/
void temp_read_temperature(void) {
	uint8_t i;
	for (i=0;i<TEMP_SENSORS_NUM;i++) {
		if (( temp_sensors[i].flags & TEMP_SENSOR_ROLE_MASK) != TEMP_SENSOR_ROLE_INACTIVE) {
			temp_read_temp_from_ds(i);
			if (temp_sensors[i].flags & _BV(TEMP_SENSOR_STATUS_OK)) {	// czujnik ok
				temp_set_daynight(i);		//ustaw dzienną lub nocną temperaturę
				temp_check_tresholds(i);   //sprawdź alarmy
				temp_control_outs(i);	   //ustaw flagi wyjść
			} else { 						// czujnik uszkodzony
				TEMP_CLEAR_OPER_FLAGS(i);
			}
		} else { 								//czujnik nieaktywny
			temp_set_failure(i,ON);
		}
		temp_set_outs(i);
	}
	temp_set_alarms();
}

/** Ustawia alarmy i loguje zdarzenia
*/
void temp_set_alarms(void) {
	uint8_t i,amin=0,amax=0,afail=0;
	for (i=0;i<TEMP_SENSORS_NUM;i++) {
		if (CHKF(i,TEMP_ALARM_MAX)) {		
			amax++;
		}
		if (CHKF(i,TEMP_ALARM_MIN)) {
			amin++;
		}
		if ((( temp_sensors[i].flags & TEMP_SENSOR_ROLE_MASK) != TEMP_SENSOR_ROLE_INACTIVE) && !(CHKF(i,TEMP_SENSOR_STATUS_OK))) {
			afail++;
		} else {
		
		}
	}

	if (amax) {
		alarmLog(ALARM_FLAG_TEMP_MAX,ON,UINT8_T_DISABLED,0);
	} else {
		alarmLog(ALARM_FLAG_TEMP_MAX,OFF,UINT8_T_DISABLED,0);
	}

	if (amin) {
		alarmLog(ALARM_FLAG_TEMP_MIN,ON,UINT8_T_DISABLED,0);
	} else {
		alarmLog(ALARM_FLAG_TEMP_MIN,OFF,UINT8_T_DISABLED,0);
	}

	if (afail) {
		alarmLog(ALARM_FLAG_TEMP_SENSOR_FAILURE,ON,UINT8_T_DISABLED,0);
	} else {
		alarmLog(ALARM_FLAG_TEMP_SENSOR_FAILURE,OFF,UINT8_T_DISABLED,0);
	}
		
}

/** Ustawia flagi wyjść
@param id indentyfikator czujnika
*/
void temp_control_outs(uint8_t id) {
	if ((temp_var != UINT16_T_DISABLED) && (temp_sensors[id].histeresis != UINT8_T_DISABLED)) {
		if ( temp_sensors[id].temp >= temp_var ) {
				temp_sensors[id].flags &= ~_BV(TEMP_SENSOR_HEATING);
		}
		if ( temp_sensors[id].temp >= temp_var + temp_sensors[id].histeresis ) {
			temp_sensors[id].flags |= _BV(TEMP_SENSOR_COOLING);
		}
		if ( temp_sensors[id].temp <= temp_var ) {
		temp_sensors[id].flags &= ~_BV(TEMP_SENSOR_COOLING);
		}
		if ( temp_sensors[id].temp <= temp_var - temp_sensors[id].histeresis ) {
			temp_sensors[id].flags |= _BV(TEMP_SENSOR_HEATING);
		}
	} else {
		TEMP_CLEAR_OPER_FLAGS(id);
	}
}
 
/** Ustawia flagi badreads
*/
void temp_badreads(int8_t id) {
	if (temp_sensors[id].bad_reads < TEMP_SENSOR_MAX_BAD_READS) {
		temp_sensors[id].bad_reads++;
	}
	if (temp_sensors[id].bad_reads > TEMP_SENSOR_BAD_READS_FAILURE) { // osiągnięto maksymalną liczbę błędnych odczytów
		temp_set_failure(id,ON);
	}
}

/** Wybiera temperaturę dzienną lub nocną
@param id identyfikator czujnika
*/
void temp_set_daynight(uint8_t id) {
	temp_var = temp_sensors[id].target;
  if (!(just_in_time(&daytime_from,&daytime_to))&&(temp_sensors[id].target_night != UINT16_T_DISABLED)) {
		temp_var = temp_sensors[id].target_night;
	}
}

void temp_set_failure(uint8_t id,uint8_t new_state) {
	if (new_state && (temp_sensors[id].flags & _BV(TEMP_SENSOR_STATUS_OK))) {
		TEMP_CLEAR_ALL_FLAGS(id);
		log_write_record(LOG_EVENT_ALARM_SENSOR_FAILURE, (0x80 | (id & LOG_EVENT_DEVICE_MASK)),0,0);
	} else if (!new_state && !(temp_sensors[id].flags & _BV(TEMP_SENSOR_STATUS_OK))) {
		temp_sensors[id].flags |= _BV(TEMP_SENSOR_STATUS_OK);
		temp_sensors[id].bad_reads = 0;
		log_write_record(LOG_EVENT_ALARM_SENSOR_FAILURE, id & LOG_EVENT_DEVICE_MASK,0,0);
	}
}

/** Sprawdza progi i wystawia alarm.
@param id identyfikator czujnika
*/
void temp_check_tresholds(uint8_t id) {

	if ((temp_sensors[id].max != UINT16_T_DISABLED) && CHKF(id,TEMP_SENSOR_STATUS_OK)) {
		if (temp_sensors[id].temp > temp_sensors[id].max && !(temp_sensors[id].flags & _BV(TEMP_ALARM_MAX))) {
			temp_sensors[id].flags |= _BV(TEMP_ALARM_MAX);
			log_write_record(LOG_EVENT_ALARM_MAX, (0x80 | (id & LOG_EVENT_DEVICE_MASK)),0,0);
		} else if (temp_sensors[id].temp <= temp_sensors[id].max && (temp_sensors[id].flags & _BV(TEMP_ALARM_MAX))) {
			log_write_record(LOG_EVENT_ALARM_MAX, (id & LOG_EVENT_DEVICE_MASK),0,0);
			temp_sensors[id].flags &= ~_BV(TEMP_ALARM_MAX);
		}
	}
	if ((temp_sensors[id].min != UINT16_T_DISABLED) && CHKF(id,TEMP_SENSOR_STATUS_OK)) {
		if (temp_sensors[id].temp < temp_sensors[id].min && !(temp_sensors[id].flags & _BV(TEMP_ALARM_MIN))) {
			temp_sensors[id].flags |= _BV(TEMP_ALARM_MIN);
			log_write_record(LOG_EVENT_ALARM_MIN, (0x80 | (id & LOG_EVENT_DEVICE_MASK)),0,0);
		} else if (temp_sensors[id].temp >= temp_sensors[id].min && (temp_sensors[id].flags & _BV(TEMP_ALARM_MIN))) {
			log_write_record(LOG_EVENT_ALARM_MIN, (id & LOG_EVENT_DEVICE_MASK),0,0);
			temp_sensors[id].flags &= ~_BV(TEMP_ALARM_MIN);
		}
	}
}
	
/** Wykrywa wszystkie wiszące na przewodzie czujniki przy pomocy procedury ROM_SEARCH.
*/
void temp_discover_sensors (void) {
	
	uint8_t i,j;
	uint8_t rom[8];
	
	last_discrepency = 0;
	last_device_flag = 0;
	for (i=0;i<TEMP_SENSORS_NUM;i++) {

	temp_sensors[i].flags &= ~(_BV(TEMP_SENSOR_STATUS_OK)|_BV(TEMP_SENSOR_CONVERT));
	
		if (onewire_search_next(&rom[0])) {
			for (j=0;j<8;j++) {
					temp_sensors[i].rom[j] = rom[j];
			}
		}
	}
}

/** Zapisuje do loga aktualną wartość temperatury z poszczególnych czujników
*/
void temp_register(void) {
	uint8_t i;
	for (i=0;i<TEMP_SENSORS_NUM;i++) {
		if (CHKF(i,TEMP_SENSOR_STATUS_OK)) {
			log_write_record(LOG_EVENT_TEMP_READ,i,(uint8_t)(temp_sensors[i].temp >> 8),(uint8_t)temp_sensors[i].temp);
		}
	}
}

/** @file temp.c
	@brief Implementacja kontroli temperatury

	Kontrola temperatury oparta jest o czujniki DS18B20. Teoretyczna ilość czujników jest ograniczona samą magistralą \a 1-wire , ponieważ wszystkie czujniki wiszą na jednym przewodzie. Rozróżniane są za pomocą adresu ROM. Dane czujników przechowywane są w strukturze \a TEMP_SENSOR .

	@section temp_control Kontrola temperatury
	@image html temp_control.png


Działania algorytmu strującego są nastepujące:
@verbatim
                                                                  HEATING     COOLING     ALARM
CURRENT_TEMP > MAX_TEMP                                     ---->                         MAX
CURRENT_TEMP > TARGET_TEMP(TARGET_NIGHT_TEMP) + HISTERESIS  ---->                1
CURRENT_TEMP > TARGET_TEMP(TARGET_NIGHT_TEMP)               ---->    0
CURRENT_TEMP < TARGET_TEMP(TARGET_NIGHT_TEMP)               ---->                0
CURRENT_TEMP < TARGET_TEMP(TARGET_NIGHT_TEMP) - HISTERESIS  ---->    1 
CURRENT_TEMP < MAX_TEMP                                     ---->                         MIN
@endverbatim

@subsection temp_format Format temperatury
Wrtość temperatury przechowywana jest w zmiennych 16-bitowych. Format przechowywania temperatury jest następujący:
@verbatim
FEDCBA98|76543210  - numer bitu
    XXXX XXXX      - dziesiątki i jedności wartości temperatury w postaci dziesiętnej 
             YYYY  - dziesiętna część wartości temperatury
     1     4   2   - temp 20.1 st. C 
@endverbatim

@subsection temp_sensors Czujnik temperatury

Jako czujnik temperatury wykorzystany jest DS18B20. Trzykońcówkowy czujnik podłączony jest do sterownika za pomocą wtyczki Jack stereo 3,5mm jak na ponizszym rysunku:

@image html jack_temp_sensor.jpg

@todo 
 - kalibracja czujników
 - nazewnictwo czujników zamiana miejscami
*/

