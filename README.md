# 🚛 Hệ Thống Tối Ưu Lộ Trình Giao Hàng (Delivery Optimization System)

> **Đồ án môn học:** Lý thuyết Đồ thị & Kỹ thuật Phần mềm  
> **Sinh viên thực hiện:** Đăng  
> **Trường:** Đại học Sư phạm TP.HCM (HCMUE)

## 📖 Giới thiệu

Dự án này là một ứng dụng phần mềm giải quyết bài toán **Người du lịch (Traveling Salesman Problem - TSP)** áp dụng vào thực tế giao hàng tại TP.HCM. Hệ thống sử dụng **Thuật toán Di truyền (Genetic Algorithm)** để tìm ra lộ trình ngắn nhất đi qua danh sách các điểm giao hàng, giúp tiết kiệm chi phí xăng xe và thời gian di chuyển.

Điểm đặc biệt của dự án là sự kết hợp giữa thuật toán C++ hiệu năng cao và giao diện trực quan hóa sinh động bằng HTML5/Leaflet Maps.

## ✨ Tính năng nổi bật

* **🧬 Thuật toán thông minh:** Sử dụng Genetic Algorithm kết hợp tối ưu cục bộ 2-Opt để giải quyết bài toán TSP với độ chính xác cao.
* **🗺️ Bản đồ trực quan:** Xuất kết quả ra file HTML tương tác, hiển thị lộ trình chi tiết trên nền bản đồ OpenStreetMap.
* **📍 Công cụ Map Picker:** Tích hợp công cụ chọn địa điểm thông minh (Geocoding), cho phép tìm kiếm và lấy tọa độ chỉ bằng 1 cú click.
* **💾 Quản lý dữ liệu:** Lưu trữ danh sách địa điểm và lịch sử chạy thuật toán bằng cơ sở dữ liệu SQLite.
* **📊 Biểu đồ hiệu năng:** Theo dõi quá trình "học" của thuật toán qua biểu đồ đường (Line Chart) tích hợp trong báo cáo.
* **📂 Nhập liệu linh hoạt:** Hỗ trợ nhập tay hoặc nhập hàng loạt từ file text.

## 🛠️ Công nghệ sử dụng

* **Ngôn ngữ chính:** C++ (Standard 14 trở lên).
* **Cơ sở dữ liệu:** SQLite3 (Embedded Database).
* **Giao diện báo cáo:** HTML5, CSS3, JavaScript.
* **Thư viện bản đồ:** Leaflet.js & OpenStreetMap API.
* **Thư viện biểu đồ:** Chart.js.
* **Công cụ Build:** CMake.

## ⚙️ Cài đặt và Chạy chương trình

### Yêu cầu
* Trình biên dịch C++ (MinGW trên Windows hoặc GCC/Clang trên Linux/macOS).
* CMake (phiên bản 3.20 trở lên).

### Các bước thực hiện
1.  **Clone dự án về máy:**
    ```bash
    git clone [https://github.com/Ten-Github-Cua-Ban/Delivery-Optimization.git](https://github.com/Ten-Github-Cua-Ban/Delivery-Optimization.git)
    cd Delivery-Optimization
    ```

2.  **Build dự án bằng CMake:**
    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```

3.  **Chạy chương trình:**
    * Trên Windows: `LTDT.exe`
    * Trên Linux/Mac: `./LTDT`

*(Lưu ý: File cơ sở dữ liệu `MyCityData.db` sẽ tự động được tạo ra trong lần chạy đầu tiên)*

## 🚀 Hướng dẫn sử dụng

Khi khởi động, chương trình sẽ hiển thị Menu chính:

1.  **Tìm lộ trình tối ưu (GA):** Chạy thuật toán. Sau khi chạy xong, mở file `ket_qua_ban_do.html` để xem kết quả.
2.  **Thêm địa điểm (Thủ công):** Nhập Tên, Vĩ độ, Kinh độ, Mức độ kẹt xe.
3.  **Nhập dữ liệu từ file:** Tự động nạp danh sách từ file `input.txt`.
4.  **Xóa địa điểm:** Xóa một điểm giao hàng khỏi Database theo ID.
5.  **Xem danh sách địa điểm:** Liệt kê các điểm hiện có để lấy ID.
6.  **Xem lịch sử kết quả:** Xem lại các lần chạy trước (Quãng đường, Thời gian).
7.  **Xóa lịch sử:** Làm sạch dữ liệu lịch sử.
8.  **Mở công cụ lấy tọa độ:** * Chương trình sẽ mở file `chon_dia_diem.html`.
    * Nhập tên địa điểm cần tìm -> Bấm "Tìm kiếm".
    * Bấm "Sao chép vào Tool C++" và dán vào chức năng số 2.

## 📂 Cấu trúc thư mục

```text
Delivery-Optimization/
├── Algorithms/         # Mã nguồn thuật toán (GeneticAlgo, Constants)
├── Database/           # Xử lý SQLite (CityDAO)
├── Models/             # Các lớp đối tượng (City)
├── Utils/              # Các tiện ích (Xuất HTML, Map Picker)
├── main.cpp            # Điểm khởi chạy chương trình
├── CMakeLists.txt      # Cấu hình Build
├── input.txt           # Dữ liệu mẫu
└── style.css           # Giao diện báo cáo HTML
