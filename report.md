# RPiDataMonitor

`RPiDataMonitor` is a small project I worked on in my C Programming class to learn more about writing code for a self-contained device using a single-board Raspberry Pi and various sensors. The self-contained device that I used for this project includes the following specs:

- `Raspberry Pi 4 Model B` with the `Raspbian GNU/Linux 12` operating system
- `DHT11` humidity and temperature sensor
- `16x2 LCD Panel` (w/out i2c controller)

Here is a picture and a fritzing diagram that shows the hardware connections for the device:

<p align="center">
  <img src="https://github.com/jayd-lee/resources/blob/main/RPiDataMonitor/picture.png" alt="RPiDataMonitor picture" width="45%" style="display: inline-block; margin-right: 10px;"/>
  <img src="https://github.com/jayd-lee/resources/blob/main/RPiDataMonitor/fritzing.png" alt="RPiDataMonitor fritzing diagram" width="45%" style="display: inline-block;"/>
</p>

## Structure

The project is structured as follows:

```txt
├── build               	intermediate build files e.g. *.o (created by make)
├── include             	header files
├── src                 	C source files
│   ├── main.c          	Entry point for the CLI
│   ├── db.c            	Functions for db operations
│   ├── humidityTempSensor.c    Record DHT11 data
│   ├── lcd.c    		Outputs data to LCD
│   └── gui.c			incomplete GUI for the program
├── .gitignore
├── Makefile
└── README.md
```

## Code analysis

RPiDataMonitor is a very simple program that records humidity and temperature data, prints it out to the LCD panel, and records it to the database, while allowing the user to interact with the data. Here is a rundown explaining how the code works step by step:

#### 0. Table of contents

* [1. Setup & init](#### 1. Setup & init)
* [2. Start the data collection thread](#### 2. Start the data collection thread)
* [3. CLI application for the user in the main thread](#### 3. CLI application for the user in the main thread)
* [4. Calling the database](#### 4. Calling the database)
* [5. Summary](#### 5. Summary)


#### 1. Setup & init

Here are the first few lines of code for the main function of the project

```c
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
// ... rest of the code

```

The database is initialized using the credentials specified in the first four lines of the main function. Within the `db.c` file, the `dbConnect` and `dbInit` functions manage the database connection and ensure proper SQL table configuration. The `wiringPiSetup` function is called to check for any errors before using the sensor and the LCD panel.

#### 2. Start the data collection thread

```c
	SensorData data;	
	data.humidity = INITIAL_VALUE;
	data.temperature = INITIAL_VALUE;

	// start a thread to collect data in the background
	pthread_t thread1;
	pthread_create(&thread1, NULL, recordData, &data);

```
Here, `data` is initialized as a SensorData struct, which is a C structure that contains humidity (double) and temperature (double) members. This struct encapsulates the two variables, which allows them to be passed as a single parameter to `pthread_create`, which accepts only one argument for the function parameter (the 4th argument). The `data.humidity` and `data.temperature` is initialized to a typedef of `INITIAL_VALUE`, which is set to INFINITY. This is done to avoid entering initially declared values into the database, later in the code.

With the initialization of `thread1` and the execution of the `pthread_create` function, the `recordData` function now runs on a separate thread:

```c
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

```

The `recordData` function sets up the LCD panel, and begins collecting the humidity and temperature data with an infinite while loop. This will run indefinitely until the user exits the program, on a separate thread. 

The function records new data with the `dht11_read_val` function from `humidityTempSensor.c`, and prints the most recent data to the LCD panel using the `printLCD` function from `lcd.c`.

Then, the program runs a few safety checks before adding the humidity & temperature data to the database. First, a mutex lock is implemented through the `pthread_mutex_lock` function, which ensures that the main thread and the recordData thread isn't accessing the database at the same time. Next, the if statement checks whether the data is INFINITY or not, so that only the newly collected data is recorded into the database (in case the sensor has yet to detect any data).

After adding the data to the database, the mutex unlocks, and the loop repeats after a delay of 3 seconds.

#### 3. CLI application for the user in the main thread

Back at the main thread, the user is prompted with a displayMenu in the command line:

```c
	printf("\nWelcome to the RPiDataMonitor\n");
	while(1) {
		displayMenu();
		handleUserInput();
	}
```

```c
void displayMenu() {
	printf("Please select an option:\n");
	printf("1. Show statistics (min, max, average) for all data \n");
	printf("2. Show statistics (min, max, average) for time range \n");
	printf("3. Exit\n\n");
	printf("Enter your choice: ");
}

```

The handleUserInput function processes user input and executes the corresponding action based on the user's choice. For the third option, or **3. Exit**, the program gets aborted and stops. For option 1 and 2, the program mutex locks and calls the function `getAllStats` from `db.c`, which makes queries to the database to return the statistics.

```c
//handlerUserInput function
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


			// ... same code for scanning and checking the endDate


			pthread_mutex_lock(&mutex);
			if (getAllStats(startTime, endTime, &minHumidity, &maxHumidity, &averageHumidity,
						&minTemp, &maxTemp, &averageTemp) == 0) {

			// ... rest of the code
```

For option 1, the first two arguments are passed in as NULL, which makes the getAllStats function use the default values for the start and end time ranges. For option 2, the user enters the date range, and `isValidDate` function is called to prevent the user from entering bad input to cause SQL injections or Seg faults.

#### 4. Calling the database

When the `getAllStats` is finally called, the program makes queries to the database through other helper functions in `db.c` to return a statistics report about the humidity and temperature data:


```c
// db.c

int getAllStats(const char *startTime, const char *endTime,
		double *minHumidity, double *maxHumidity, double *averageHumidity,
		double *minTemp, double *maxTemp, double *averageTemp) {
	char defaultStartTime[] = "1970-01-01 00:00:00";  // Default start time
	char defaultEndTime[] = "9999-12-31 23:59:59";    // Default end time

	// If start time and end time are not provided, use default values
	startTime = startTime ? startTime : defaultStartTime;
	endTime = endTime ? endTime : defaultEndTime;

	if (getMin("humidity_percent", startTime, endTime, minHumidity) != 0) return 1;
	if (getMax("humidity_percent", startTime, endTime, maxHumidity) != 0) return 1;
	if (getAverage("humidity_percent", startTime, endTime, averageHumidity) != 0) return 1;

	if (getMin("temperature_C", startTime, endTime, minTemp) != 0) return 1;
	if (getMax("temperature_C", startTime, endTime, maxTemp) != 0) return 1;
	if (getAverage("temperature_C", startTime, endTime, averageTemp) != 0) return 1;

	return 0;
}

```

As stated before, the `defaultStartTime` and `defaultEndTime` is set for option 1, which returns the statistics for all data in the database. 

The `getMin`, `getMax`, and `getAverage` helper functions make their corresponding database queries, to both humidity and temperature data.

```c
// implementation of the getAverage function in db.c
int getAverage(const char *column, const char *startTime, const char *endTime, double *avgValue) {
	char query[200];
	sprintf(query, "SELECT AVG(%s) FROM rpiData WHERE DATE(time) >= '%s' AND DATE(time) <= '%s';", column, startTime, endTime);

	if (mysql_query(conn, query) != 0) {
		fprintf(stderr, "SELECT AVG(%s) failed. Error: %s\n", column, mysql_error(conn));
		return 1;
	}
	MYSQL_RES *res = mysql_store_result(conn);
	if (res == NULL) {
		fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
		return 1;
	}
	MYSQL_ROW row = mysql_fetch_row(res);
	*avgValue = row ? atof(row[0]) : 0.0;
	mysql_free_result(res);
	return 0;
}
```


#### 5. Summary

And there, the program is able to safely collect data and also run a CLI application at the same time, utilizing many technologies and methodologies to create a cohesive experience for the user.

This project was not too difficult to implement. However, the multi-component nature of this program was what made it exciting to work with, and it was one of the most fun projects that I have made!



