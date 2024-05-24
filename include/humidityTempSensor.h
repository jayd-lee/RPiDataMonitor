#ifndef HUMIDITYTEMPSENSOR_H
#define HUMIDITYTEMPSENSOR_H

#include <stdint.h>
#include <wiringPi.h>

#ifndef DHT11PIN
#define DHT11PIN 7
#endif

#ifndef MAX_TIME
#define MAX_TIME 85
#endif

// Function prototype
void dht11_read_val(double *humidity, double *temperature);

#endif // HUMIDITYTEMPSENSOR_H

