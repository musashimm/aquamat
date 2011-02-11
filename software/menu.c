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

/** @file menu.c
	@brief Funkcje obsługi menu.
*/

#include <avr/pgmspace.h>
#include "hd44780.h"
#include "commonDefs.h"
#include "aquamat.h"
#include "menu.h"
#include "temp.h"

struct MENU_STATE ms;

uint8_t menu_phase(void) {
	return ms.mod_phase;
}

uint8_t menu_unit(void) {
	return ms.unit_id;
}

void menu_unit_next (enum MENU_KEY key,uint8_t max_id) {
	if (key_pressed == key ) {
		if ( ++ms.unit_id == max_id) {
			ms.unit_id = 0;
		}
		menu_reset_phase();
	}
}

void menu_unit_prev (enum MENU_KEY key,uint8_t max_id) {
	if (key_pressed == key ) {
		if ( --ms.unit_id == UINT8_T_DISABLED ) {
			ms.unit_id = max_id - 1;
		}
		menu_reset_phase();
	}
}

void menu_next (enum MENU_KEY key) {
	if (key_pressed == key ) {
		if ( ++ms.menu_id == pgm_read_byte(&MENU_LENGTH[0]) ) {
			ms.menu_id = 0;
		}
		ms.submenu_id = 0;
		ms.mod_phase = 0;
	}
}

void menu_prev (enum MENU_KEY key) {
	if (key_pressed == key ) {
		if ( ms.menu_id == 0 ) {
			ms.menu_id = pgm_read_byte(&MENU_LENGTH[0]) - 1;
		} else {
			ms.menu_id--;
		}
		ms.submenu_id = 0;
		ms.mod_phase = 0;
	}
}

void menu_top (enum MENU_KEY key) {
	if (key_pressed == key) {
		menu_set(0,0,0,0);
		hd44780_cursor_off();
	}
}

void menu_submenu_top (enum MENU_KEY key) {
	if (key_pressed == key) {      
		ms.submenu_id = 0;
		ms.unit_id = 0;
		menu_reset_phase();
	}
}

void menu_submenu_next (enum MENU_KEY key) {
	if (key_pressed == key) {
		if ( ++ms.submenu_id == pgm_read_byte(&MENU_LENGTH[ms.menu_id+1]) ) {
			ms.submenu_id = 1;
		}
		menu_reset_phase();
	}
}

void menu_submenu_prev (enum MENU_KEY key) {
	if (key_pressed == key) {  
		if ( ms.submenu_id == 1 ) {
			ms.submenu_id = pgm_read_byte(&MENU_LENGTH[ms.menu_id+1]) - 1;
		} else {
			ms.submenu_id--;
		} 
		menu_reset_phase();
	}
}

void menu_reset_phase(void) {
	ms.mod_phase = 0;
	hd44780_cursor_off();
	MENU_CF(MENU_VARS_LOADED_FLAG);
}

void menu_set_phase(void) {
	hd44780_cursor_blink();
	menu_phase_up();
}

void menu_reset_phase_key(enum MENU_KEY key) {
	if (key_pressed == key) {
		menu_reset_phase();
	}
}

void menu_dummy(void) {
	key_pressed = NONE;
}

uint8_t menu_func_index() {
        uint8_t p = 0;
        uint8_t i;
        for(i=0; i < ms.menu_id; i++)
        {
                p=p+pgm_read_byte(&MENU_LENGTH[i+1]);
        }
        p=p+ms.submenu_id;
        return p;
}

void menu_set(uint8_t mn,uint8_t sb,uint8_t ph,uint8_t unit) {
	ms.menu_id = mn;
	ms.submenu_id = sb;
	ms.mod_phase = ph;
	ms.unit_id = unit;
}

uint8_t menu_menuid(void) {
	return ms.menu_id;
}

void menu_phase_change (enum MENU_KEY key,uint8_t pidx) {
	if (key_pressed == key ) {
		ms.mod_phase = pidx;
	}
}

void menu_key_phase_up (enum MENU_KEY key) {
	if (key_pressed == key ) {
		ms.mod_phase++;
	}
}

void menu_phase_up(void) {
	ms.mod_phase++;
}

void menu_standard_top(void) {
	menu_submenu_top(ESC);
	menu_submenu_next(UP);
	menu_submenu_prev(DOWN);
	menu_key_phase_up(SET);
}

void menu_standard_unit(uint8_t unit_num) {
	menu_submenu_top(ESC);
	menu_unit_next(UP,unit_num);
	menu_unit_prev(DOWN,unit_num);
	menu_key_phase_up(SET);
}

void menu_standard_phase(void) {
	menu_reset_phase_key(ESC);
	menu_submenu_next(UP);
	menu_submenu_prev(DOWN);
	menu_key_phase_up(SET);
}

//######################################################## funkcje zmiany parametrow

void menu_inc_uint8 (enum MENU_KEY key,uint8_t* var,uint8_t max,uint8_t reload) {
	if (key_pressed == key ) {
		if (*var < max) (*var)++;
		if (reload) {
			MENU_CF(MENU_VARS_LOADED_FLAG);
		}
	}
}

void menu_inc_temp (enum MENU_KEY key,TEMP* var) {
	uint8_t units,tens;
	if (key_pressed == key ) {
		if (*var < TEMP_MAXIMUM_VALUE) {
			units = pgm_read_byte(&TEMP_CONVERT_DS_TO_DEC[(uint8_t)(*var & 0x0F)]);
			tens = *var >> 4;
			units++;
			if(units < 10) {
				*var = (TEMP)(tens << 4) | pgm_read_byte(&TEMP_CONVERT_DEC_TO_DS[units]);
			} else {
				tens++;
				*var = (TEMP)(tens << 4);
			}
		}
	}
}

void menu_dec_temp (enum MENU_KEY key,TEMP* var) {
	uint8_t units,tens;
	if (key_pressed == key ) {
		if (*var) {
			units = pgm_read_byte(&TEMP_CONVERT_DS_TO_DEC[(uint8_t)(*var & 0x0F)]);
			tens = *var >> 4;
			if(units) {
				units--;
				*var = (TEMP)(tens << 4) | pgm_read_byte(&TEMP_CONVERT_DEC_TO_DS[units]);
			} else {
				tens--;
				*var = (TEMP)(tens << 4) | pgm_read_byte(&TEMP_CONVERT_DEC_TO_DS[9]);
			}
		}
	}
}

void menu_toggle_bit (enum MENU_KEY key,uint8_t* var,uint8_t bit) {
	if (key_pressed == key ) {
		*var ^= _BV(bit);
	}
}

void menu_dec_uint8 (enum MENU_KEY key,uint8_t* var,uint8_t min,uint8_t reload) {
	if (key_pressed == key ) {
		if (*var > min) (*var)--;
		if (reload) {
			MENU_CF(MENU_VARS_LOADED_FLAG);
		}
	}
}

void menu_zero_uint8 (enum MENU_KEY key,uint8_t* var) {
	if (key_pressed == key ) {
	 	(*var)=0;
	}
}

void menu_mod_temp(uint8_t posx,uint8_t posy, TEMP *var) {
	menu_reset_phase_key(ESC);
	hd44780_goto(posx,posy);
	menu_inc_temp(UP,var);
	menu_dec_temp(DOWN,var);
	menu_key_phase_up(SET);
}

void menu_mod_uint8(uint8_t posx,uint8_t posy, uint8_t *var, uint8_t min, uint8_t max, uint8_t reload) {
	menu_reset_phase_key(ESC);
	hd44780_goto(posx,posy);
	menu_inc_uint8(UP,var,max,reload);
	menu_dec_uint8(DOWN,var,min,reload);
	menu_key_phase_up(SET);
}

void menu_mod_bit(uint8_t posx,uint8_t posy,uint8_t *var,uint8_t bit) {
	menu_reset_phase_key(ESC);
	hd44780_goto(posx,posy);
	menu_toggle_bit(UP,var,bit);
	menu_toggle_bit(DOWN,var,bit);
	menu_key_phase_up(SET);
}

void menu_mod_char(uint8_t posx,uint8_t posy,char *var,uint8_t base) {
	menu_reset_phase_key(ESC);
	hd44780_goto(posx,posy+ms.mod_phase-base);
	menu_inc_uint8(UP,(uint8_t*)(var+ms.mod_phase-base),OUTPUTS_NAME_MAX_CHAR,FALSE); 
	menu_dec_uint8(DOWN,(uint8_t*)(var+ms.mod_phase-base),OUTPUTS_NAME_MIN_CHAR,FALSE);
	menu_key_phase_up(SET);
}
