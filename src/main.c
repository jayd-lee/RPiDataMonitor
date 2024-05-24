#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <math.h> 
#include "humidityTempSensor.h"
#include "lcdPanel.h"
#include "db.h"

#define INITIAL_VALUE INFINITY

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	double humidity;
	double temperature;
} SensorData;

void *recordData(void *arg);
void displayMenu();
void handleUserInput();
bool isValidDate(const char *date);

int main(int argc, char **argv) {
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
	data.humidity = INITIAL_VALUE;
	data.temperature = INITIAL_VALUE;

	// start a thread to collect data in the background
	pthread_t thread1;
	pthread_create(&thread1, NULL, recordData, &data);

	printf("\nWelcome to the RPiDataMonitor\n");
	while(1) {
		displayMenu();
		handleUserInput();
	}

	// this will never happen, user will abort the program 
	// before joining the first thread back into the main thread
	pthread_join(thread1, NULL);

	return 0;
}

void *recordData(void *arg) {
	SensorData *data = (SensorData *)arg;
	lcdSetup();

	while (1) {
		dht11_read_val(&(data->humidity), &(data->temperature));
		// printf("humidity = %lf, temperature = %lf\n", data->humidity, data->temperature);
		printLCD(data->humidity, data->temperature);
		
		// implement mutexes to prevent race conditions / DB read & write errors
		// (AKA Prof Miller's "Porta Potty")
		pthread_mutex_lock(&mutex);

		// shouldn't put initial values (infinity) into DB
		if (!isinf(data->humidity) && !isinf(data->temperature)) {
			addData(data->humidity, data->temperature);
		}

		pthread_mutex_unlock(&mutex);

		delay(3000);
	}
}

void displayMenu() {
	printf("Please select an option:\n");
	printf("1. Show statistics (min, max, average) for all data \n");
	printf("2. Show statistics (min, max, average) for time range \n");
	printf("3. Exit\n\n");
	printf("Enter your choice: ");
}

void handleUserInput() {
	int choice;
	static double minHumidity, maxHumidity, averageHumidity;
	static double minTemp, maxTemp, averageTemp;
	char startDate[11], endDate[11];

	printf("Enter your choice: ");
	scanf("%d", &choice);

	switch (choice) {
                case 1:
			pthread_mutex_lock(&mutex);
			if (getAllStats(NULL, NULL, &minHumidity, &maxHumidity, &averageHumidity,
						&minTemp, &maxTemp, &averageTemp) == 0) {
				printf("\nHumidity - Min: %.2lf, Max: %.2lf, Avg: %.2lf\n", minHumidity, maxHumidity, averageHumidity);
				printf("Temperature - Min: %.2lf, Max: %.2lf, Avg: %.2lf\n\n", minTemp, maxTemp, averageTemp);
			} else {
				printf("Error fetching statistics.\n");
			}
			pthread_mutex_unlock(&mutex);
			break;
		case 2:
			printf("Enter start date (YYYY-MM-DD): ");
			scanf(" %s", startDate); // Add space before %s to consume newline

			// User input validation to prevent SQL injections or segfaults
			while (!isValidDate(startDate)) {
				printf("Invalid date format. Please enter the date in YYYY-MM-DD format: ");
				scanf(" %s", startDate);
			}

			printf("Enter end date (YYYY-MM-DD): ");
			scanf(" %s", endDate); 

			while (!isValidDate(endDate)) {
				printf("Invalid date format. Please enter the date in YYYY-MM-DD format: ");
				scanf(" %s", endDate);
			}

			// Convert date strings to time range with 00:00:00 to 23:59:59
			char startTime[20], endTime[20];
			snprintf(startTime, sizeof(startTime), "%s 00:00:00", startDate);
			snprintf(endTime, sizeof(endTime), "%s 23:59:59", endDate);

			pthread_mutex_lock(&mutex);
			if (getAllStats(startTime, endTime, &minHumidity, &maxHumidity, &averageHumidity,
						&minTemp, &maxTemp, &averageTemp) == 0) {
				printf("\nHumidity - Min: %.2lf, Max: %.2lf, Avg: %.2lf\n", minHumidity, maxHumidity, averageHumidity);
				printf("Temperature - Min: %.2lf, Max: %.2lf, Avg: %.2lf\n\n", minTemp, maxTemp, averageTemp);
			} else {
				printf("Error fetching statistics.\n");
			}
			pthread_mutex_unlock(&mutex);
			break;
		case 3:
			printf("Exiting program.\n");
			closeDB();
			exit(0);
		default:
			printf("Invalid choice. Please try again.\n");
	}
}


bool isValidDate(const char *date) {
	int i = 0;

	// Check if the length of the date string is exactly 10 characters
	if (strlen(date) != 10) {
		return false;
	}

	// Validate each character
	while (date[i] != '\0') {
		if (i == 4 || i == 7) {
			// Check for hyphens at positions 4 and 7
			if (date[i] != '-') {
				return false;
			}
		} else {
			// Check if the characters are digits
			if (date[i] < '0' || date[i] > '9') {
				return false;
			}
		}
		i++;
	}

	return true;
}

