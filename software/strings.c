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

	$Id: com_menu.c 41 2009-02-23 23:01:59Z musashi@todryk.pl $
*/

/** @file strings.c
	@brief Definicje stringów używanych w programie.
*/

#include "aquamat.h"

PROGMEM char RTC_MON_STR[] ="Po";
PROGMEM char RTC_TUE_STR[] ="Wt";
PROGMEM char RTC_WED_STR[] ="Sr";
PROGMEM char RTC_THU_STR[] ="Cz";
PROGMEM char RTC_FRI_STR[] ="Pi";
PROGMEM char RTC_SAT_STR[] ="So";
PROGMEM char RTC_SUN_STR[] ="Ni";
PROGMEM char RTC_KAZ_STR[] ="Ka";

/** @var PGM_P RTC_WDAYS_STRS[] PROGMEM
	Dni tygodnia.
*/
PGM_P RTC_WDAYS_STRS[] PROGMEM =
{
	RTC_SUN_STR,
	RTC_MON_STR,
	RTC_TUE_STR,
	RTC_WED_STR,
	RTC_THU_STR,
	RTC_FRI_STR,
	RTC_SAT_STR,
	RTC_KAZ_STR
};

PROGMEM char AKT_S[] ="Akt";
PROGMEM char BL_S[] ="Bl";
PROGMEM char NZW_S[] ="Nzw";
PROGMEM char QB_S[] ="Qb";
// PROGMEM char ALARM_S[] ="Alarm: ";
// PROGMEM char CENTURY_S[] = "20";
PROGMEM char COLON_S[] = ":";
// PROGMEM char COOLING_S[] ="Chlodzenie: ";
// PROGMEM char DAYTIME_S[] = "Pora dnia: ";
PROGMEM char DOT_S[] = ".";
// PROGMEM char EVERYDAY_S[] = "Codziennie ";
PROGMEM char EXCLEMATION_S[] = "!";
// PROGMEM char FAILURE_S[] ="Uszkodzony ";
// PROGMEM char FROM_S[] = "Od: ";
// PROGMEM char GROUP_S[] ="Grupa: ";
// PROGMEM char HEATING_S[] ="Podgrzewanie: ";
PROGMEM char INTEMPSENS_S[] = " --.-";
// PROGMEM char INT_MIN_S[] = "min. ";
// PROGMEM char INT_OFF_S[] = "Wylaczony: ";
// PROGMEM char INT_ON_S[] = "Wlaczony: ";
PROGMEM char MAX_S[] ="Mx";
PROGMEM char MIN_S[] ="Mn";
PROGMEM char MINUS_S[] = "-";
PROGMEM char MINUSMINUS_S[] = "--";
PROGMEM char MINUSMINUSMINUS_S[] = "---";
// PROGMEM char NNN_S[] = "\n\n\n";
// PROGMEM char NN_S[] = "\n\n";
// PROGMEM char NONE_S[] ="Brak ";
// PROGMEM char NOTIFY_S[] = "Sygnalizacja alarmowa: ";
// PROGMEM char NPAR_S[] = "\n    ";
// PROGMEM char N_S[] = "\n";
// PROGMEM char OFF_S[] ="Wyl. ";
// PROGMEM char OK_S[] = "Ok";
// PROGMEM char ON_S[] ="Wl. ";
// PROGMEM char OUT_NONAME_S[] ="_____";
PROGMEM char OUT_S[] ="Wy";
PROGMEM char OUTPWM_S[] ="WyPWM";
PROGMEM char PAR_S[] = "    ";
PROGMEM char RO_S[] = "R";
PROGMEM char SLASH_S[] = "/";
PROGMEM char SPACE_S[] = " ";
// PROGMEM char START_S[] ="Start ";
// PROGMEM char STATUS_ACTIVE_S[] = "Aktywn(y/e) ";
// PROGMEM char STATUS_BAD_OUT_S[] = "Bledne wyjscie";
// PROGMEM char STATUS_BAD_TIME_S[] = "Bledne ustawienia czas(u/ow)";
// PROGMEM char STATUS_BAD_WDAY[] = "Nieakt. dzis";
// PROGMEM char STATUS_BLOCKED_S[] = "Zablokowan(y/e)";
// PROGMEM char STATUS_NOTACTIVE_S[] = "Niektywn(y/e) ";
// PROGMEM char STATUS_OUT_BLOCKED_S[] = "Wyjscie zablokowane";
// PROGMEM char STATUS_S[] ="Status: ";
// PROGMEM char STATUS_STATUS_ALREADY_BUSY_S[] = "Konflikt wyjsc";
// PROGMEM char TEMP_CURRENT_S[] ="biezaca: ";
// PROGMEM char TEMP_FLAGS_S[] =" Flagi: ";
// PROGMEM char TEMP_HISTERESIS_S[] ="Histereza: ";
// PROGMEM char TEMP_MAX_S[] ="maksymalna: ";
// PROGMEM char TEMP_MIN_S[] ="minimalna: ";
// PROGMEM char TEMP_SENSOR_S[] = " Czujnik: ";
// PROGMEM char TEMP_TARGET_NIGHT_S[] ="docelowa nocna: ";
// PROGMEM char TEMP_TARGET_S[] ="docelowa: ";
// PROGMEM char TEMP_TEMP_S[] ="Temperatura ";
// PROGMEM char TIMER_S[] ="Timer: ";
// PROGMEM char TO_S[] = "Do: ";
// PROGMEM char TOP_OFF_MAX_RUN_S[] = "Pompa ";
// PROGMEM char UINT8_BAD_S[] = "XX";
// PROGMEM char UNKNOWN_S[] = "Nieznan(y/e)";
// PROGMEM char WDAY_S[] = "Dzien tygodnia: ";
