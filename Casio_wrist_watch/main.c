/*
 * Casio_wrist_watch.c
 *
 * Created: 12.06.2026 11:08:43
 * Author : e4Menzi
 */ 

#include "ucBoardDriver.h"


int main(void){	
	initBoard(0);
	
	//uhr
		uint64_t clock = 0;
		uint8_t clock_modes = 0;
		uint8_t twelve_twentyfour = 0;
	//time anzeige
		uint8_t hour_24h = 0;
		uint8_t hour_12h = 0;
		uint8_t minutes = 0;
		uint8_t seconds = 0;
		uint8_t milisecond = 0;
	//time anziege stoppuhr
		uint8_t stopwatch_hundredths = 0;
		uint8_t stopwatch_seconds = 0;
		uint8_t stopwatch_minutes = 0;
	//dates
		uint8_t date_day = 1;
	//entry
		uint8_t entry_flag = 0;
	//morgen oder nachmittag
		char *  AM_PM[] = {"AM","PM","24"};
		uint8_t daytime = 0;
		uint8_t daytime_flag = 0;
	//weekdays
		char * week_days[] = {"MO","TU","WE","TH","FR","SA","SO"};
		uint8_t day_of_the_week = 0;
	//buttons
		uint8_t buttons = 0;
		uint8_t buttons_neu = 0;
		uint8_t buttons_alt = 0;
		uint8_t buttons_flanke = 0;
		uint8_t buttons_pos_flanke = 0;
		uint8_t button_A = 0;
		uint8_t button_L = 0;
		uint8_t button_c = 0;
	//clock light
		uint8_t clock_light = 0;
		
    while (1) {
//Eingabe
	buttons = PINL;
	buttons_alt = buttons_neu;
	buttons_neu = buttons;
	buttons_flanke = buttons_neu ^ buttons_alt;
	buttons_pos_flanke = buttons_flanke & buttons_neu;
	button_A = (buttons_pos_flanke & 0b10000000)>0;
	button_L = (buttons_pos_flanke & 0b0010)>0;
	button_c = (buttons_pos_flanke & 0b0001)>0;
//Verarbeitung
	//umrechnung
		if((getSystemTimeMs()-clock) >=1000){
			seconds += 1;
			clock = getSystemTimeMs();
		}
		if(minutes > 59){
			minutes = 0;
			hour_12h += 1;
			hour_24h += 1;
		}
		if(seconds > 59){
			minutes += 1;
			seconds = 0;
		}
		
		if(hour_12h > 11){
			hour_12h = 0;
			daytime_flag = daytime_flag ^ 1;
		}
		if(hour_24h > 23){
			hour_24h = 0;
			day_of_the_week = 0;
			date_day += 1;
		}
		if(button_A){
			twelve_twentyfour = !twelve_twentyfour;
		}
		if(!twelve_twentyfour){
			daytime = 2;
		}
		else{
			if(!daytime_flag){
				daytime = 0;
			}
		}
		if(button_L){
			clock_light = clock_light ^ 1;
		}
		
	//cases
	switch (clock_modes){
		case 0:
			//time
			lcdWriteText(0,0,"%s %s",AM_PM[daytime],week_days[day_of_the_week]);
			lcdWriteText(1,2,":%02u:%02u",minutes,seconds);
			lcdWriteText(0,6,"%s",date_day);
			if(!twelve_twentyfour){
				lcdWriteText(1,0,"%02u",hour_24h);
			}
			else{
				lcdWriteText(1,0,"%02u",hour_12h);
			}
			if(button_c){
				clock_modes += 1;
			}
		break;
		case 1://daily alarm
			if(button_c){
				clock_modes += 1;
			}
		break;
		case 2://Stopwatch
			if(button_c){
				clock_modes += 1;
			}
			
			lcdWriteText(1,0,"%02u:%02u:%02u",stopwatch_minutes,stopwatch_seconds,stopwatch_hundredths);
			
		break;
		case 3://Zeitänderungen
			if(button_c){
				clock_modes = 0;
			}
		break;
	}
//Ausgabe
	if(clock_light){
		lcdLight(255);
	}
	else{
		lcdLight(0);
	}
    }
}

