project/
│
├── bin/ # Chứa file thực thi
│ ├── client # File thực thi client
│ └── server # File thực thi server
│
├── src/ # Mã nguồn chính
│ ├── client/ # Mã nguồn cho client
│ │ ├── include/ # Chứa các header file cho client
│ │ │ ├── client.h # Định nghĩa cấu trúc và hàm chính cho client
│ │ │ ├── auth.h # Xử lý đăng nhập/đăng ký người dùng
│ │ │ └── project.h # Quản lý thông tin dự án
│ │ ├── main.c # Điểm khởi đầu cho client
│ │ │
│ │ ├── ui/ # Mã nguồn cho giao diện người dùng
│ │ │ ├── main_window.c # Giao diện cửa sổ chính
│ │ │ ├── login_window.c # Giao diện đăng nhập
│ │ │ ├── project_view.c # Hiển thị thông tin dự án
│ │ │ ├── chat_window.c # Giao diện chat
│ │ │ ├── view_call_window.c # Giao diện video call
│ │ │ ├── task_view.c # Giao diện công việc
│ │ │ ├── gantt_view.c # Hiển thị biểu đồ Gantt
│ │ │ ├── attachment_handler.c # Xử lý file đính kèm
│ │ │
│ │ ├── network/ # Xử lý kết nối mạng
│ │ │ ├── socket.c # Xử lý kết nối socket
│ │ │ └── webrtc.c # Xử lý WebRTC cho video call
│ │ │
│ │ └── media/ # Xử lý media (âm thanh và video)
│ │ ├── audio.c # Xử lý âm thanh
│ │ └── video.c # Xử lý video
│ │
│ ├── server/ # Mã nguồn cho server
│ │ ├── include/ # Header files cho server
│ │ │ ├── server.h # Định nghĩa cấu trúc và hàm chính cho server
│ │ │ └── session.h # Quản lý phiên làm việc của người dùng
│ │ │
│ │ ├── database/ # Thao tác với cơ sở dữ liệu
│ │ │ ├── db_init.c # Khởi tạo cơ sở dữ liệu
│ │ │ ├── user_dao.c # Thao tác với bảng user
│ │ │ ├── project_dao.c # Thao tác với bảng project
│ │ │ └── task_dao.c # Thao tác với bảng task
│ │ │
│ │ ├── network/ # Xử lý kết nối mạng
│ │ │ ├── socket_handler.c # Xử lý kết nối socket
│ │ │ └── connection_pool.c # Quản lý pool connections
│ │ │
│ │ └── logger/ # Ghi log và xử lý lỗi
│ │ ├── logger.c # Ghi log hệ thống
│ │ └── error_handler.c # Xử lý lỗi
│ │
│ │ └── main.c # Điểm khởi đầu cho server
│ │
│ └── common/ # Các thành phần dùng chung giữa client và server
│ ├── protocol.h # Định nghĩa giao thức giao tiếp
│ ├── structures.h # Các cấu trúc dữ liệu dùng chung
│ ├── utils.c # Các hàm tiện ích
│ ├── validation.h # Hàm kiểm tra tính hợp lệ
│ └── config.h # Cấu hình mạng, cơ sở dữ liệu, media, bảo mật
│
├── docs/ # Tài liệu dự án
│ ├── API.md # Tài liệu về các API được cung cấp
│ ├── DATABASE.md # Thiết kế cơ sở dữ liệu
│ └── SETUP.md # Hướng dẫn cài đặt và cấu hình
│
├── tests/ # Thư mục kiểm thử
│ ├── client_tests/ # Unit test cho client
│ └── server_tests/ # Unit test cho server
│
├── lib/ # Các thư viện bên thứ ba cần thiết
│ ├── sqlite/ # Thư viện SQLite cho cơ sở dữ liệu
│ ├── gstreamer/ # Thư viện GStreamer cho media
│ └── gtk/ # Thư viện GTK cho giao diện
│
├── resources/ # Các tài nguyên bổ sung
│ ├── icons/ # Biểu tượng cho giao diện người dùng
│ ├── sounds/ # Âm thanh thông báo
│ ├── database/ # Database
│ │ ── project.db
│ └── config/ # File cấu hình cho client/server
│ │ ── client_config.json
│ │ ── server_config.json
│
├── Makefile # Tập tin build script cho project
├── README.md # Hướng dẫn tổng quan về project
└── .gitignore # Danh sách file và thư mục bị bỏ qua trong git
