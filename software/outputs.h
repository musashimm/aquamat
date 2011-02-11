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

#define OUTPUTS_DEVICES_NUM 2
#define OUTPUTS_NUM_PER_DEVICE 8
#define OUTPUTS_NUM (OUTPUTS_DEVICES_NUM * OUTPUTS_NUM_PER_DEVICE)
#define OUTPUTS_CHANGE_FLAG 7

#define SOUTF(x) (outputs_flags |= _BV(x))
#define COUTF(x) (outputs_flags &= ~_BV(x))
#define OUTF(x) (outputs_flags & _BV(x))

#define OUTPUT_CONFIG_MASK 0x0F
#define OUTPUT_ACTIVE_FLAG 7
#define OUTPUT_CANCEL_FLAG 6
#define OUTPUT_OVERRIDE_FLAG 5
#define OUTPUT_BLOCK_FLAG 3

/** @struct OUTPUT
	@brief Struktura przechowywująca dane o wyjściu.
*/
struct OUTPUT {
/** Flagi.
@verbatim
7654|3210
x   |        - wyjście aktywne? (OUTPUT_ACTIVE_FLAG)
0   |        - aktywne
1   |        - nieaktywne
 X  |        - zmiana stanu? (OUTPUT_CANCEL_FLAG)
 0  |        - początkowe
 1  |        - wtórne
  X |        - nadpisany był block? (OUTPUT_BLOCK_OVERRIDE_FLAG)
  0 |        - nie
  1 |        - tak
    |X       - zablokowane? (OUTPUT_BLOCK_FLAG)
    |0       - nie
    |1       - tak
    | XXX    - Numer QB (Quick Button) klawisz szybkiego dostępu
    | 0-5    - numer klawisza
    | 111    - klawisz nieprzypisany
@endverbatim
	*/
    uint8_t flags;
};

/** @file outputs.h
	@brief Plik nagłówkowy - Implementacja obsługi wyjść.
	@section outfunctions Podział funkcji

- pracujące na grupie wyjść:
	- outputs_init
	- outputs_update
	- outputs_all
	- outputs_block_all
- pracujace na pojedynczym wyjściu:
	- output_block_override
	- output_set
	- output_toggle
	- output_switch
- pracujace na flagach wyjścia:
	- output_check_flag
	- output_set_flag
*/

void outputs_init(void);
void outputs_update(void);
void outputs_all(uint8_t state);
void outputs_block_all(uint8_t state);

uint8_t output_check_flag(uint8_t,uint8_t);
void output_set_flag(uint8_t,uint8_t,uint8_t);

void output_block_override(uint8_t);
void output_set(uint8_t id,uint8_t state);
void output_toggle(uint8_t id);
uint8_t output_switch(uint8_t id,uint8_t state);

void outputs_assign(uint8_t *,uint8_t);

extern struct OUTPUT outputs[OUTPUTS_NUM];
extern uint8_t outputs_flags;

