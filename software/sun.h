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

#ifndef SUN_H
#define SUN_H

/** @file sun.h
	@brief Plik nagłówkowy - Implementacja scenariusza "SUN". Symulacja wschodu i zachodu słońca.
*/

#include "time.h"

struct SUN {
        struct MIT sunrise;
        struct MIT sunset;
        int max_pwm;
        int min_pwm;
        int step_period;
        int step_num;
};

#endif
