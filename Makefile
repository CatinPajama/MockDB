CC = gcc
CFLAGS = -g

BUILD_DIR := ./objs
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=$(BUILD_DIR)/%.o)
TARGET := mockdb

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR) $(TARGET)