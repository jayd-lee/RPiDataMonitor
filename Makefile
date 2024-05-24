INCLUDE_DIR := ./include
SRC_DIR := ./src
BUILD_DIR := ./build
TARGET = $(BUILD_DIR)/main

CC = gcc
CFLAGS = -I$(INCLUDE_DIR) -Wall -Wextra `mysql_config --cflags`
LDFLAGS = -lwiringPi -lwiringPiDev `mysql_config --libs`

# List of source files
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Generate list of object files from source files
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean

