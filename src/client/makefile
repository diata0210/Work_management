# Makefile cho client

# Compiler và flags
CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lsqlite3 -lpthread

# Thư mục
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRCS = $(wildcard ui/*.c) \
       $(wildcard message_handlers/*.c) \
       client.c

# Object files
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

# Include paths
INCLUDES = -I./include

# Target chính
all: dirs client

# Tạo thư mục cần thiết
dirs:
	@mkdir -p $(BUILD_DIR)/ui
	@mkdir -p $(BUILD_DIR)/message_handlers
	@mkdir -p $(BIN_DIR)

# Build client
client: $(OBJS)
	$(CC) $(OBJS) -o $(BIN_DIR)/client $(LDFLAGS)

# Pattern rule để build object files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run
run: client
	./$(BIN_DIR)/client

.PHONY: all dirs clean run
