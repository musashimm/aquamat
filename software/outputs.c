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
#include "outputs.h"
#include "io.h"
#include "twi_addresses.h"
#include "usart.h"
#include "strings.h"
#include "eeprom.h"
#include "temp.h"
#include "log.h"
#include "hd44780.h"

/** @var struct OUTPUT outputs[]
	Tablica przechowująca stan wyjść.
*/
struct OUTPUT outputs[OUTPUTS_NUM];

/** @var uint8_t outputs_flags
	Rejestr flag związanych z obsługą wyjść.
*/
uint8_t outputs_flags=UINT8_T_DISABLED;

/** @var const uint8_t PROGMEM OUTPUTS_DEVICES_ADDRS[]
	Przechowuje informacje o adresach TWI układów wyjściowych.
*/
PROGMEM const uint8_t OUTPUTS_DEVICES_ADDRS[] = {
	OUTPUTS_DEVICE_1_ADDR,
	OUTPUTS_DEVICE_2_ADDR
};

/** Inicjalizuje flagi wyjść. Wyłącza i blokuje wyjścia.
*/
void outputs_init(void) {
	uint8_t i;
	for (i=0;i<OUTPUTS_NUM;i++) {
		outputs[i].flags=0x8F;
	}
	output_name[EEPROM_OUTS_NAME_LENGTH]=0;
}

/** W przypadku kiedy flaga \ref OUTPUTS_CHANGE_FLAG w jest ustawiona należy zaktualizować stan wyjść.
*/
void outputs_update(void) {
	uint8_t i,j;
	uint8_t output_temp;
	if (OUTF(OUTPUTS_CHANGE_FLAG)) {
		for (i=0;i<OUTPUTS_DEVICES_NUM;i++) {
			output_temp = 0;
			for (j=0;j<OUTPUTS_NUM_PER_DEVICE;j++) {
				output_temp |= (outputs[i*OUTPUTS_NUM_PER_DEVICE+j].flags & _BV(OUTPUT_ACTIVE_FLAG)) >> (OUTPUTS_NUM_PER_DEVICE - 1 - j);
			}
			set_io(pgm_read_byte(&OUTPUTS_DEVICES_ADDRS[i]) & 0xFE,output_temp);
		}
	COUTF(OUTPUTS_CHANGE_FLAG);
	}
}

/** Sprawdza poszczególne flagi
	@param id numer wyjścia
	@param flag flaga
*/
uint8_t output_check_flag(uint8_t id,uint8_t flag) {
    if (id >= OUTPUTS_NUM) {
        return 1;
    } else {
        return outputs[id].flags & _BV(flag);
    }
}

/** Ustawia poszczególne flagi
	@param id numer wyjścia
	@param flag flaga
	@param state stan
*/
void output_set_flag(uint8_t id,uint8_t flag,uint8_t state) {
    if (id < OUTPUTS_NUM) {
        if (state) {
            outputs[id].flags |= _BV(flag);
        } else {
            outputs[id].flags &= ~_BV(flag);
        }
    }
}

/** Przełącza przeładowanie blokowania
	@param id numer wyjścia
*/

void output_block_override(uint8_t id) {
	if (output_check_flag(id,OUTPUT_CANCEL_FLAG)) {
		if (output_check_flag(id,OUTPUT_OVERRIDE_FLAG)) {
			output_set_flag(id,OUTPUT_OVERRIDE_FLAG,OFF);
			output_set_flag(id,OUTPUT_BLOCK_FLAG,OFF);
		}
		output_set_flag(id,OUTPUT_CANCEL_FLAG,OFF);
	} else {
		output_set_flag(id,OUTPUT_CANCEL_FLAG,ON);
		if (!(output_check_flag(id,OUTPUT_BLOCK_FLAG))) {
			output_set_flag(id,OUTPUT_OVERRIDE_FLAG,ON);
			output_set_flag(id,OUTPUT_BLOCK_FLAG,ON);
		}
	}
}

/** Włącza lub wyłącza wyjście
	@param id numer wyjścia
	@param state docelowy stan
*/
void output_set(uint8_t id,uint8_t state) {
	output_block_override(id);
	if (state) {
		output_set_flag(id,OUTPUT_ACTIVE_FLAG,OFF);
	} else {
		output_set_flag(id,OUTPUT_ACTIVE_FLAG,ON);
	}
	SOUTF(OUTPUTS_CHANGE_FLAG);
}

/** Zmienia stan wyjścia na przeciwny.
	@param id numer wyjścia
*/
void output_toggle(uint8_t id) {
	output_block_override(id);
	if (output_check_flag(id,OUTPUT_ACTIVE_FLAG)) {
		output_set_flag(id,OUTPUT_ACTIVE_FLAG,OFF);
	} else {
		output_set_flag(id,OUTPUT_ACTIVE_FLAG,ON);
	}
	SOUTF(OUTPUTS_CHANGE_FLAG);
}

/** Wyłącza/włącza wszystkie wyjścia.
*/
void outputs_all(uint8_t state) {
	uint8_t i;
	if (state) {
		for (i=0;i<OUTPUTS_NUM;i++) {
			output_set_flag(i,OUTPUT_ACTIVE_FLAG,OFF);
		}
	} else {
		for (i=0;i<OUTPUTS_NUM;i++) {
			output_set_flag(i,OUTPUT_ACTIVE_FLAG,ON);
		}
	}
	SOUTF(OUTPUTS_CHANGE_FLAG);
}

/** Blokuje odblokowuje wszystkie wyjścia.
*/
void outputs_block_all(uint8_t state) {
	uint8_t i;
	if (state) {
		for (i=0;i<OUTPUTS_NUM;i++) {
			output_set_flag(i,OUTPUT_BLOCK_FLAG,ON);
		}
	} else {
		for (i=0;i<OUTPUTS_NUM;i++) {
			output_set_flag(i,OUTPUT_BLOCK_FLAG,OFF);
		}
	}
}

/** Przyporządkowuje identyfikator wyjścia
*/
void outputs_assign(uint8_t *out,uint8_t id) {
	if (id >= OUTPUTS_NUM) {
		*out = OUTPUTS_NUM;
	} else {
		*out = id;
	}
}

/** Włącza lub wyłącza wyjście sprawdzając najpierw stan blokady.
	@param id numer wyjścia
	@param state operacja do wykonania \ref ON "ON" lub \ref OFF "OFF"
*/
uint8_t output_switch(uint8_t id,uint8_t state) {
	uint8_t new_state=0;
	if ((id >= OUTPUTS_NUM) || output_check_flag(id,OUTPUT_BLOCK_FLAG)) {
		return 0;
	}
	if ((state && output_check_flag(id,OUTPUT_ACTIVE_FLAG)) || (!state && !output_check_flag(id,OUTPUT_ACTIVE_FLAG))) {
		if (state) {
			new_state = _BV(LOG_EVENT_START_STOP_FLAG);
		} else {
			new_state = OFF;
		}
		log_write_record(LOG_EVENT_OUT_CHANGE,new_state|id,UINT8_T_DISABLED,UINT8_T_DISABLED);
		output_set_flag(id,OUTPUT_ACTIVE_FLAG,!state);
		SOUTF(OUTPUTS_CHANGE_FLAG);
		return 1;
	}
    return 0;
}

/** @file outputs.c
        @brief Implementacja obsługi wyjść.

        \section out_devices Układy wyjściowe.
        Sterowanie układami zewnętrznymi opiera się o magistralę I2C oraz układy
z rodziny PCF8574. Dla programu układy wyjściowe widoczne są jako numer
wyjścia (od 0 do 15). 12 wyjść wyprowadzone jest na złącza DB9. , 4
wyjscia wyprowadzone są na złącze IDC. Każde wyjscie można obciążyć do
500mA (ULN2803). Rozmieszczenie sygnałów w złączu DB9 jest następujący:

	@image html control_connector.jpg
*/
