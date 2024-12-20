#ifndef CONFIG_H
#define CONFIG_H
#define MAX_BUFFER 1024  // Đặt giá trị kích thước theo nhu cầu, ví dụ: 1024 bytes

// Đường dẫn tới database SQLite
#define DB_PATH "resources/database/project.db"

// Địa chỉ IP và cổng của server
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

// Số lượng kết nối tối đa
#define MAX_CONNECTIONS 10

// Các tham số khác của server (nếu cần)
#define LOG_FILE "server_logs.txt"

#endif // CONFIG_H
