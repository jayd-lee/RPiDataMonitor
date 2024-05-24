#include "humidityTempSensor.h"
#include <stdio.h>
#include <wiringPi.h>

int dht11_val[5] = {0, 0, 0, 0, 0};

void dht11_read_val(double *humidity, double *temperature)
{
	uint8_t lststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;
	for (i = 0; i < 5; i++)
		dht11_val[i] = 0;
	pinMode(DHT11PIN, OUTPUT);
	digitalWrite(DHT11PIN, LOW);
	delay(18);
	digitalWrite(DHT11PIN, HIGH);
	delayMicroseconds(40);
	pinMode(DHT11PIN, INPUT);
	for (i = 0; i < MAX_TIME; i++)

	{
		counter = 0;
		while (digitalRead(DHT11PIN) == lststate)
		{
			counter++;
			delayMicroseconds(1);
			if (counter == 255)
				break;
		}
		lststate = digitalRead(DHT11PIN);
		if (counter == 255)
			break;
		// top 3 transistions are ignored
		if ((i >= 4) && (i % 2 == 0))
		{
			dht11_val[j / 8] <<= 1;
			if (counter > 25)
				dht11_val[j / 8] |= 1;
			j++;
		}
	}
	// verify cheksum and update the verified data
	if ((j >= 40) && (dht11_val[4] == ((dht11_val[0] + dht11_val[1] + dht11_val[2] + dht11_val[3]) & 0xFF)))
	{
		*humidity = dht11_val[0] + dht11_val[1] * 0.1;
		*temperature = dht11_val[2] + dht11_val[3] * 0.1;
	}
}


