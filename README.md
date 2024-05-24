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

```
## Structure

The project is structured as follows:

```txt
├── build                       intermediate build files e.g. *.o (created by make)
├── include                     header files
├── src                         C source files
│   ├── main.c                  Entry point for the CLI
│   ├── db.c                    Functions for db operations
│   ├── humidityTempSensor.c    Record DHT11 data
│   ├── lcd.c                   Outputs data to LCD
│   └── gui.c                   incomplete GUI for the program
├── .gitignore
├── Makefile
└── README.md
```

## Setup

There are three dependencies that is required before building and running this project

Install WiringPi (required for GPIO interfacing)

```shell
git clone https://github.com/WiringPi/WiringPi
```

Intall mariadb and mysqlclient (or any choice of database)

```shell
sudo apt-get install mariadb-server
sudo apt-get install default-libmysqlclient-dev
```

Start MariaDB, and log in as root

```shell
sudo systemctl start mariadb
sudo mysql
```

Create a user (choose a `username` and `password` to replace {username} & {password}, without the curly braces)

```shell
create user '{username}'@'localhost' identified by '{password}';
```

## Build

Clone repository

```shell
git clone https://github.com/jayd-lee/RPiDataMonitor
```

Build executable

```shell
make
```

## Running the program

```shell
./build/main
```

