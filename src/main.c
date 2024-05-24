#include <stdio.h>
#include <pthread.h>
#include "humidityTempSensor.h"
#include "lcdPanel.h"
#include "db.h"

typedef struct {
	double humidity;
	double temperature;
} SensorData;

void *recordData(void *arg);

int main() {
	char *server = "localhost";
	char *user = "jayden";
	const char *password = "password";
	char *database = "RPiDB";

	if (dbConnect(server, user, password, database) != 0) {
		fprintf(stderr, "Failed to connect to database.\n");
		return 0;
	}

	if (dbInit() != 0) {
		fprintf(stderr, "Failed to initialize database.\n");
		return 0;
	}

	printf("setting up wiringPi and printing humidity & temperature\n");
	if (wiringPiSetup() == -1) {
		fprintf(stderr, "Error: Unable to initialize WiringPi.\n");
		return 1;
	}

	SensorData data;	
	data.humidity = 0.0;
	data.temperature = 0.0;

	pthread_t thread1;
	pthread_create(&thread1, NULL, recordData, &data);

	// run user program
	while (1) {
		
	}

	// this will never happen, user will abort the program 
	// before joining back to main thread
	pthread_join(thread1, NULL);
	
	return 0;
}

void *recordData(void *arg) {
	SensorData *data = (SensorData *)arg;
	lcdSetup();

	while (1) {
		dht11_read_val(&(data->humidity), &(data->temperature));
		printf("humidity = %lf, temperature = %lf\n", data->humidity, data->temperature);
		printLCD(data->humidity, data->temperature);
		addData(data->humidity, data->temperature);
		delay(3000);
	}
}
