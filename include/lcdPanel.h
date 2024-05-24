#ifndef LCD_PANEL_H
#define LCD_PANEL_H

#include <lcd.h>
#include <wiringPi.h>

//WIRINGPI PIN NUMBERS
#define LCD_RS 25 //Register select pin
#define LCD_E 24 //Enable Pin
#define LCD_D4 23 //Data pin 4
#define LCD_D5 22 //Data pin 5
#define LCD_D6 21 //Data pin 6
#define LCD_D7 14 //Data pin 7

void lcdSetup();
void printLCD(double humidity, double temp);

#endif // LCD_PANEL_H 

