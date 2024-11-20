# Biến cấu hình
CC = gcc
CFLAGS = -Wall -Wextra -g -pthread -I./src/server/include
LDFLAGS = -lsqlite3
BIN_DIR = ./bin

# Tệp thực thi server
SERVER_BIN = $(BIN_DIR)/server

# Thư mục mã nguồn
SERVER_SRC_DIR = ./src/server

# Các tệp nguồn và object cho server
SERVER_SRC = $(SERVER_SRC_DIR)/main.c \
             $(SERVER_SRC_DIR)/network/socket_handler.c \
             $(SERVER_SRC_DIR)/database/db_init.c \
             $(SERVER_SRC_DIR)/database/task_dao.c \
             $(SERVER_SRC_DIR)/database/project_dao.c \
             $(SERVER_SRC_DIR)/database/user_dao.c \
             $(SERVER_SRC_DIR)/logger/logger.c \
             $(SERVER_SRC_DIR)/message_handlers/chat_message_handler.c \
             $(SERVER_SRC_DIR)/message_handlers/control_message_handler.c \
             $(SERVER_SRC_DIR)/message_handlers/data_message_handler.c \
             $(SERVER_SRC_DIR)/message_handlers/video_message_handler.c

SERVER_OBJ = $(SERVER_SRC:.c=.o)

# Quy tắc mặc định
all: $(BIN_DIR) $(SERVER_BIN)

# Biên dịch server
$(SERVER_BIN): $(SERVER_OBJ)
	@echo "Linking server binary..."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Server build complete!"

# Biên dịch các tệp .o từ .c
%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Tạo thư mục bin nếu chưa tồn tại
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Chạy server
run: $(SERVER_BIN)
	@echo "Running server..."
	./$(SERVER_BIN)

# Dọn dẹp các tệp biên dịch
clean:
	@echo "Cleaning up server build files..."
	@rm -rf $(SERVER_OBJ) $(SERVER_BIN)
	@echo "Clean complete!"
