#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "logger.h"

// Đường dẫn file log
#define LOG_FILE_PATH "/home/parallels/Desktop/Work-managment/Work_management/src/server/logger/application.log"

// Hàm ghi log chung vào file
void write_log_to_file(const char *level, const char *format, va_list args) {
    FILE *log_file = fopen(LOG_FILE_PATH, "a");
    if (log_file == NULL) {
        perror("Could not open log file");
        return;
    }

    // Lấy thời gian hiện tại
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    // Ghi log vào file
    fprintf(log_file, "[%s] [%s] ", time_str, level);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");

    fclose(log_file);
}

// Ghi log thông tin (INFO)
void log_info(const char *format, ...) {
    va_list args;

    // Ghi log ra console
    va_start(args, format);
    printf("[INFO] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);

    // Ghi log vào file
    va_start(args, format);
    write_log_to_file("INFO", format, args);
    va_end(args);
}

// Ghi log lỗi (ERROR)
void log_error(const char *format, ...) {
    va_list args;

    // Ghi log ra console
    va_start(args, format);
    printf("[ERROR] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);

    // Ghi log vào file
    va_start(args, format);
    write_log_to_file("ERROR", format, args);
    va_end(args);
}
