# Compiler và flags
CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lpthread

# Thư mục
SRC_DIR = .
UI_DIR = $(SRC_DIR)/ui
NETWORK_DIR = $(SRC_DIR)/network
MESSAGE_DIR = $(SRC_DIR)/message_handlers
LOGGER_DIR = $(SRC_DIR)/logger
BUILD_DIR = ../../build/client
BIN_DIR = ../../bin

# Tệp nguồn và đối tượng
SRCS = $(SRC_DIR)/main.c \
       $(NETWORK_DIR)/socket.c \
       $(UI_DIR)/login_window.c \
       $(UI_DIR)/main_window.c \
       $(MESSAGE_DIR)/chat_message_handler.c \
       $(MESSAGE_DIR)/control_message_handler.c \
       $(MESSAGE_DIR)/data_message_handler.c \
       $(LOGGER_DIR)/logger.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Tệp thực thi
CLIENT_BIN = $(BIN_DIR)/client

# Quy tắc mặc định
all: dirs $(CLIENT_BIN)

# Biên dịch client
$(CLIENT_BIN): $(OBJS)
	@echo "Linking client..."
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete!"

# Biên dịch các tệp .o từ .c
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Tạo các thư mục build và bin
dirs:
	@mkdir -p $(BUILD_DIR)/ui
	@mkdir -p $(BUILD_DIR)/network
	@mkdir -p $(BUILD_DIR)/message_handlers
	@mkdir -p $(BUILD_DIR)/logger
	@mkdir -p $(BIN_DIR)

# Dọn dẹp
clean:
	@echo "Cleaning up build files..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)/client
	@echo "Clean complete!"

# Chạy chương trình
run: $(CLIENT_BIN)
	@echo "Running client..."
	./$(CLIENT_BIN)

.PHONY: all clean run dirs
