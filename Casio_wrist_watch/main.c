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
	//dates
		uint8_t date_day = 1;
		uint8_t month = 1;
	//entry
		uint8_t entry_flag = 0;
	//morgen oder nachmittag
		char *  AM_PM[] = {"AM","PM","24","ST","AL   ","SU"};
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
	//stopwatch
		uint8_t stopwatch_hundredths = 0;
		uint8_t stopwatch_seconds = 0;
		uint8_t stopwatch_minutes = 0;
		uint8_t stop_watch_mini_modes = 0;
	//kalender
		uint8_t kalender_modes = 0;
		uint8_t zwischenspeicher_sekunden = 0;
		uint8_t zwischenspeicher_minuten = 0;
		uint8_t zwischenspeicher_stunden = 0;
		uint8_t zwischenspeicher_monate = 1;
		uint8_t zwischenspeicher_datum = 0;
		uint8_t zwischenspeicher_wochentag = 0;
	//clock symbol anziege wecker
		uint8_t clock_symbol = 1;
		uint8_t stripes_symbol = 2;
		const uint8_t clock_symbol_bitarray[] = {	0b00100,
																			0b01110,
																			0b01110,
																			0b01110,
																			0b11111,
																			0b11111,
																			0b00100,
																			0b00000};
		const uint8_t stripes_symbol_bitarray[] = {	0b00000,
																			0b10101,
																			0b10101,
																			0b10101,
																			0b10101,
																			0b10101,
																			0b10101,
																			0b00000};
		lcdCreateCustomChar(clock_symbol,clock_symbol_bitarray);
		lcdCreateCustomChar(stripes_symbol,stripes_symbol_bitarray);
		uint8_t symbol_mode = 0;
	//wecker
		uint8_t wecker_modes = 0;
		uint8_t wecker_stunden = 0;
		uint8_t wecker_minuten = 0;
		uint8_t alarm_flag = 0;
		uint64_t timer_schutz = 60500;
		uint64_t inZeit_ms = 0;
	//blinken
		uint8_t blink_timer = 0; 
		uint8_t blink_flag = 0;
		
    while (1) {
//Eingabe
	buttons = PINL;
	buttons_alt = buttons_neu;
	buttons_neu = buttons;
	buttons_flanke = buttons_neu ^ buttons_alt;
	buttons_pos_flanke = buttons_flanke & buttons_neu;
	button_A = (buttons_pos_flanke & 0b10000000)>0;
	button_L = (buttons_pos_flanke & 0b0001)>0;
	button_c = (buttons_pos_flanke & 0b0010)>0;
//Verarbeitung
	//umrechnung
		//umrechnung zeiten
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
		//12h uhr
		if(hour_12h > 11){
			hour_12h = 0;
			daytime_flag = daytime_flag ^ 1;
		}
		//24h Uhr
		if(hour_24h > 23){
			hour_24h = 0;
			day_of_the_week = 0;
			date_day += 1;
		}
		//alle tage im monat vorbei
		if(!month | (month == 2)|(month==4)|(month==6)|(month==7)|(month==9)|(month==11) ){
			if(date_day > 31){
				date_day = 1;
				month += 1;
			}
		}
		else if((month==3)|(month==5)|(month==8)|(month==10)){
			if(date_day > 30){
				date_day = 1;
				month += 1;
			}
		}
		else if((month==1)){
			if(date_day > 28){
				date_day = 1;
				month += 1;
			}
		}
		//Monats overflow
		if(month>12){
			month = 1;
		}
		//wechsel zwischen 12 und 24h modus
		if(button_A && (!clock_modes)){
			twelve_twentyfour = !twelve_twentyfour;
		}
		//24 wird angezeigt
		if(!twelve_twentyfour){
			daytime = 2;
		}
		//AM oder PM je nach daytime flag wert wird eins der beiden angezeigt
		else{
			if(!daytime_flag){
				daytime = 0;
			}
		}
		//taster um das licht einzuschalten
		if(button_L){
			clock_light = clock_light ^ 1;
		}
		//anzeige der ersten reihe je nach modus
		if(!clock_modes){
			lcdWriteText(0,0,"%s %s",AM_PM[daytime],week_days[day_of_the_week]);
		}
		else if(clock_modes == 1){
			lcdWriteText(0,0,AM_PM[4]);
		}
		else if(clock_modes==2){
			lcdWriteText(0,0,AM_PM[3]);
		}
	//wecker aktivierung
		inZeit_ms = getSystemTimeMs();
		
		if(!symbol_mode | (symbol_mode==2)){
			if((wecker_stunden==hour_24h)&&(wecker_minuten==minutes)&&((inZeit_ms-timer_schutz)>=60500)){
				rgbWrite(255,0,0);
				alarm_flag = 1;
				timer_schutz = inZeit_ms;
			}
		}
		if(!symbol_mode | (symbol_mode ==3)){
			if((minutes == 0) && (seconds == 0)){
				rgbWrite(255,0,0);
				alarm_flag = 1;
				timer_schutz = inZeit_ms;
			}
		}
		if(alarm_flag && button_L){
			rgbWrite(0,0,0);
			clock_light = !clock_light;
			alarm_flag = 0;
		}
			if(!symbol_mode){
				lcdWriteText(2,0,"%c %c", stripes_symbol, clock_symbol);
			}
			else if(symbol_mode == 1){
				lcdWriteText(2,0,"   ");
			}
			else if(symbol_mode == 2){
				lcdWriteText(2,0,"%c",stripes_symbol);
			}
			else{
				lcdWriteText(2,0,"%c", clock_symbol);
			}
			
			if((inZeit_ms-blink_timer)>=100){
				blink_flag = !blink_flag;
			}
	//cases
	switch (clock_modes){
		case 0:
			//time	
			lcdWriteText(0,18,"%u",date_day);
			lcdWriteText(1,2,":%02u:%02u",minutes,seconds);
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
			if(button_A && (!wecker_modes)){
				symbol_mode += 1;
			}
			if(symbol_mode >3){
				symbol_mode = 0;
			}
			lcdWriteText(1,0,"%02u",wecker_stunden);
			lcdWriteText(1,3,"%02u ",wecker_minuten);
			lcdWriteText(1,6,"  ");
			switch (wecker_modes){
				case 0: //nichts
					
					if(button_L){
						wecker_modes += 1;
						clock_light = 1;
					}
				break;
				case 1: //stunden
					if(blink_flag){
						lcdWriteText(1,2,":");
					}
					else
					{
						lcdWriteText(1,2," ");
					}
					if(button_A){
						wecker_stunden += 1;
					}
					if(wecker_stunden>23){
						wecker_stunden = 0;
					}
					if(button_L){
						wecker_modes += 1;
						clock_light = 1;
					}
				break;
				case 2: // minutes

					if(blink_flag){
						lcdWriteText(1,5,":");
					}
					else
					{
						lcdWriteText(1,5," ");
					}
					if(button_A){
						wecker_minuten += 1;
					}
					if(wecker_minuten>59){
						wecker_minuten = 0;
					}	
				break;
			}
		break;
		
		case 2://Stopwatch
			switch(stop_watch_mini_modes){
				case 0: //normal mode no counting
					lcdWriteText(1,0,"%02u:%02u:%02u",stopwatch_minutes,stopwatch_seconds,stopwatch_hundredths);
					if(button_A){
						stop_watch_mini_modes = 1;
					}
				break;
				case 1://counting start button A was pressed
						lcdWriteText(1,0,"%02u:%02u:%02u",stopwatch_minutes,stopwatch_seconds,stopwatch_hundredths);
						if((getSystemTimeMs()-clock) >=10){
							stopwatch_hundredths += 1;
							clock = getSystemTimeMs();
						}
						if(stopwatch_hundredths>99){
							stopwatch_hundredths = 0;
							stopwatch_seconds += 1;
						}
						if(stopwatch_seconds>59){
							stopwatch_seconds = 0;
							stopwatch_minutes += 1;
						}
						if(button_A){
							stop_watch_mini_modes = 2;
						}
						if((stopwatch_hundredths == 99) && (stopwatch_seconds == 59) && (stopwatch_minutes == 59)){
							stop_watch_mini_modes = 3;
						}
						if(button_L){
							stop_watch_mini_modes = 4;
							clock_light = 1;
						}
				break;
				
				case 2: //counting stop
					if(button_A){
						stop_watch_mini_modes = 1;
					}
					if(button_L){
						stop_watch_mini_modes = 0;
						stopwatch_seconds = 0;
						stopwatch_minutes = 0;
						stopwatch_hundredths = 0;
						clock_light = 1;
					}
				break;
				
				case 3: //max amount of timer over
					if(button_L){
						stop_watch_mini_modes = 0;
						stopwatch_seconds = 0;
						stopwatch_minutes = 0;
						stopwatch_hundredths = 0;
						clock_light = 1;
					}					
				break;
				
				case 4: //stop time but still counting
						if((getSystemTimeMs()-clock) >=10){
							stopwatch_hundredths += 1;
							clock = getSystemTimeMs();
						}
						if(stopwatch_hundredths>99){
							stopwatch_hundredths = 0;
							stopwatch_seconds += 1;
						}
						if(stopwatch_seconds>59){
							stopwatch_seconds = 0;
							stopwatch_minutes += 1;
						}
						if(button_A){
							stop_watch_mini_modes = 2;
						}
						if((stopwatch_hundredths == 99) && (stopwatch_seconds == 59) && (stopwatch_minutes == 59)){
							stop_watch_mini_modes = 3;
						}
						if(button_L){
							stop_watch_mini_modes = 1;
							clock_light = 1;
						}
						if(button_A){
							stop_watch_mini_modes = 5;
							
						}
				break;
				
				case 5: //
					if(button_L){
						lcdWriteText(1,0,"%02u:%02u:%02u",stopwatch_minutes,stopwatch_seconds,stopwatch_hundredths);
						clock_light = 1;
						stop_watch_mini_modes = 6;
					}
				break;
				
				case 6:
					if(button_L){
						clock_light = 1;
						stop_watch_mini_modes = 0;
						stopwatch_seconds = 0;
						stopwatch_minutes = 0;
						stopwatch_hundredths = 0;
					}
				break;
			}
			if(button_c){
				clock_modes += 1;
			}
		break;
		
		case 3://Zeitänderungen
			daytime = 5;
			lcdWriteText(0,0,"%s",AM_PM[daytime]);
			if(!entry_flag){
				entry_flag = 1;
				lcdWriteText(0,3,"%s",week_days[day_of_the_week]);
				zwischenspeicher_sekunden = seconds;
				zwischenspeicher_minuten = minutes;
				zwischenspeicher_stunden = hour_24h;
				zwischenspeicher_datum = date_day;
				zwischenspeicher_wochentag = day_of_the_week;
				zwischenspeicher_monate = month;
			}
			if(kalender_modes<3){
				lcdWriteText(1,0,"%02u",zwischenspeicher_stunden);
				lcdWriteText(1,3,"%02u",zwischenspeicher_minuten);
				lcdWriteText(1,6,"%02u",zwischenspeicher_sekunden);
			}
			switch(kalender_modes){
				case 0: //seconds
							if(blink_flag){
								lcdWriteText(1,8,":");
								lcdWriteText(1,8,":");
								lcdWriteText(1,8,":");
							}
							else
							{
								lcdWriteText(1,8," ");
								lcdWriteText(1,8," ");
								lcdWriteText(1,8," ");
							}
					if(button_c){
						clock_modes = 0;
					}
					//nächster modus
					if(button_L){
						lcdWriteText(1,8," ");
						kalender_modes += 1;
						clock_light = 1;
					}
					if(button_A){
						zwischenspeicher_sekunden += 1;
					}
					if(zwischenspeicher_sekunden>59){
						zwischenspeicher_sekunden = 0;
					}
				break;
				case 1: //hours
							if(blink_flag){
								lcdWriteText(1,2,":");
								lcdWriteText(1,2,":");
								lcdWriteText(1,2,":");
							}
							else
							{
								lcdWriteText(1,2," ");
								lcdWriteText(1,2," ");
								lcdWriteText(1,2," ");
							}
					//nächster modus
					if(button_L){
						kalender_modes += 1;
						lcdWriteText(1,2,":");
						clock_light = 1;
					}
					if(button_A){
						zwischenspeicher_stunden += 1;
					}
					if(zwischenspeicher_stunden >23){
						zwischenspeicher_stunden = 0;
					}
				break;
				case 2: //minutes
							if(blink_flag){
								lcdWriteText(1,5,":");
								lcdWriteText(1,5,":");
								lcdWriteText(1,5,":");
							}
							else
							{
								lcdWriteText(1,5," ");
								lcdWriteText(1,5," ");
								lcdWriteText(1,5," ");
							}
					//nächster modus
					if(button_L){
						kalender_modes += 1;
						clock_light = 1;
						lcdWriteText(1,0,"                    ");
					}
					if(button_A){
						zwischenspeicher_minuten += 1;
					}
					if(zwischenspeicher_minuten > 59){
						zwischenspeicher_minuten = 0;
					}
				break;
				case 3: //month
							if(blink_flag){
								lcdWriteText(1,2,":");
								lcdWriteText(1,2,":");
								lcdWriteText(1,2,":");
								lcdWriteText(1,2,":");
							}
							else{
								lcdWriteText(1,2," ");
								lcdWriteText(1,2," ");
								lcdWriteText(1,2," ");
								lcdWriteText(1,2," ");
							}
					//nächster modus
					if(button_L){
						kalender_modes += 1;
						lcdWriteText(1,2," ");
						clock_light = 1;
					}
					if(button_A){
						zwischenspeicher_monate += 1;
					}
					if(zwischenspeicher_monate>12){
						zwischenspeicher_monate = 1;
					}
					lcdWriteZahl(1,0,zwischenspeicher_monate,2,0);
					
				break;
				case 4: //date
							if(blink_flag){
								lcdWriteText(0,17,":");
								lcdWriteText(0,17,":");
								lcdWriteText(0,17,":");
							}
							else
							{
								lcdWriteText(0,17," ");
								lcdWriteText(0,17," ");
							}
					//nächster modus
					if(button_L){
						lcdWriteText(0,17," ");
						kalender_modes += 1;
						clock_light = 1;
					}
					if(button_A){
						zwischenspeicher_datum += 1;
					}
					if(zwischenspeicher_datum >31){
						zwischenspeicher_datum = 1;

					}
					lcdWriteText(0,18,"%u ",zwischenspeicher_datum);
				break;
				case 5://weekday
							if(blink_flag){
								lcdWriteText(0,5,":");
								lcdWriteText(0,5,":");
								lcdWriteText(0,5,":");
							}
							else
							{
								lcdWriteText(0,5," ");
								lcdWriteText(0,5," ");
								lcdWriteText(0,5,":");
							}
					if(button_A){
						zwischenspeicher_wochentag += 1;
					}
					if(zwischenspeicher_wochentag >6){
						zwischenspeicher_wochentag = 0;
					}
					lcdWriteText(0,3,"%s",week_days[zwischenspeicher_wochentag]);
					if(button_c){
						kalender_modes = 0;
						seconds = zwischenspeicher_sekunden;
						minutes = zwischenspeicher_minuten;
						hour_24h = zwischenspeicher_stunden;
						if(zwischenspeicher_stunden>11){
							hour_12h = zwischenspeicher_stunden - 12;
						}
						else{
							hour_12h = zwischenspeicher_stunden;
						}
						month = zwischenspeicher_monate;
						date_day = zwischenspeicher_datum;
						day_of_the_week = zwischenspeicher_wochentag;
						clock_modes = 0;
						entry_flag = 0;
					}
				break;
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

