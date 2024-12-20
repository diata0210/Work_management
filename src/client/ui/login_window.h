#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <gtk/gtk.h>

// Cấu trúc lưu trữ thông tin các widget cho cửa sổ login
typedef struct {
    GtkWidget *window;
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *login_button;
    GtkWidget *status_label; // Có thể thêm trạng thái hoặc thông báo cho người dùng
} LoginWindow;

// Hàm khởi tạo cửa sổ login
void create_login_window(LoginWindow *login_window);

// Hàm xử lý sự kiện nhấn nút login
void on_login_button_clicked(GtkWidget *widget, gpointer data);

// Hàm để hiển thị thông báo trạng thái (ví dụ: đăng nhập thành công hay thất bại)
void display_status_message(LoginWindow *login_window, const char *message);

#endif // LOGIN_WINDOW_H
