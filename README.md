# RPiDataMonitor

`RPiDataMonitor` is a small project I worked on in my C Programming class to learn more about writing code for a self-contained device using a single-board Raspberry Pi and various sensors. The self-contained device that I used for this project includes the following specs:

- `Raspberry Pi 4 Model B` with the `Raspbian GNU/Linux 12` operating system
- `DHT11` humidity and temperature sensor
- `16x2 LCD Panel` (w/out i2c controller)


Here is a picture and a fritzing diagram that shows the hardware connections for the device:

![RPiDataMonitor picture](https://github.com/jayd-lee/resources/blob/main/RPiDataMonitor/picture.png)


![RPiDataMonitor fritzing diagram](https://github.com/jayd-lee/resources/blob/main/RPiDataMonitor/fritzing.png)


## Setup

Intall mariadb and mysqlclient

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

