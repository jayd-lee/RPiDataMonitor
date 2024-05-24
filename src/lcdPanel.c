#include "lcdPanel.h"
#include <stdio.h>
#include <stdlib.h>

int lcd;

void lcdSetup() {
	lcd = lcdInit(2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
	if (lcd == -1) {
		fprintf(stderr, "Error: Unable to initialize LCD.\n");
		exit(1);
	}
}


void printLCD(double humidity, double temp) {
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "Humidity: %lf %% ", humidity);

        // loading "..." animation
        static int dots_counter = 0;
        lcdPosition(lcd, 0, 1);
        switch (dots_counter)
        {
                case 0:
                        lcdPrintf(lcd, "Temp: %.1lf C    ", temp);
                        break;
                case 1:
                        lcdPrintf(lcd, "Temp: %.1lf C .  ", temp);
                        break;
                case 2:
                        lcdPrintf(lcd, "Temp: %.1lf C .. ", temp);
                        break;
                case 3:
                        lcdPrintf(lcd, "Temp: %.1lf C ...", temp);
                        break;
        }
        dots_counter = (dots_counter + 1) % 4;
}

