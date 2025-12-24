📖 HƯỚNG DẪN SỬ DỤNG: HỆ THỐNG TỐI ƯU LỘ TRÌNH GIAO HÀNG (LTDT Project)
1. Giới thiệu
Đây là đồ án môn Lý thuyết Đồ thị (Graph Theory) kết hợp với Kỹ thuật Phần mềm. Ứng dụng giúp tìm ra lộ trình đi qua các điểm giao hàng sao cho tổng chi phí (quãng đường & thời gian) là thấp nhất.

Công nghệ sử dụng:

Ngôn ngữ: C++ (Standard 14).

Database: SQLite (Lưu trữ địa điểm & Lịch sử chạy).

Thuật toán: Genetic Algorithm (Di truyền) + 2-Opt Optimization.

Trực quan hóa: HTML5, Leaflet JS (Bản đồ), Chart.js (Biểu đồ hiệu năng).

2. Cài đặt & Chạy chương trình
Yêu cầu hệ thống
Trình biên dịch C++ (MinGW hoặc Visual Studio).

CMake (phiên bản 3.20 trở lên).

Trình duyệt web (Chrome/Edge) để xem kết quả bản đồ.

Các bước biên dịch (Build)
Dự án sử dụng CMake, bạn có thể build dễ dàng bằng terminal:

Clone dự án:

Bash

git clone https://github.com/Ten-Github-Cua-Ban/Delivery-Optimization.git
Mở bằng IDE: Khuyến khích dùng CLion hoặc VS Code.

Build & Run: Chạy file main.cpp.

3. Các chức năng chính (Menu)
Khi khởi động, chương trình sẽ hiển thị Menu chính như sau:

Plaintext

=== HE THONG GIAO HANG TAI TP.HCM ===
1. Tim lo trinh toi uu (GA)
2. Them dia diem (Thu cong)
3. Nhap du lieu tu file (input.txt)
4. Xoa dia diem
5. Xem danh sach dia diem
6. Xem lich su ket qua
7. Xoa lich su
8. Mo cong cu lay toa do (Map Picker)
0. Thoat
🛠 Chi tiết từng chức năng:
1️⃣ Tìm lộ trình tối ưu (Chạy thuật toán)
Tác dụng: Chạy thuật toán GA để tìm đường đi ngắn nhất qua tất cả các điểm hiện có trong Database.

Kết quả:

Hiển thị quãng đường (km) trên màn hình Console.

Xuất ra file ket_qua_ban_do.html.

Tự động lưu kết quả vào Lịch sử.

Cách xem: Mở file ket_qua_ban_do.html để xem bản đồ lộ trình và biểu đồ tối ưu hóa.

2️⃣ Thêm địa điểm (Thủ công)
Nhập tên, vĩ độ, kinh độ và mức độ kẹt xe bằng tay.

Mẹo: Có thể copy chuỗi dữ liệu từ công cụ số 8 để paste vào đây cho nhanh.

3️⃣ Nhập dữ liệu từ file
Tự động đọc file input.txt và nạp vào Database.

Định dạng file input.txt:

Plaintext

6
Cho_Ben_Thanh    10.772109  106.698285  3
Landmark_81      10.794967  106.721758  2
...
(Dòng 1: Số lượng địa điểm. Các dòng sau: Tên - Vĩ độ - Kinh độ - Mức độ kẹt xe)

4️⃣ Xóa địa điểm
Chương trình sẽ liệt kê danh sách kèm ID.

Nhập ID của địa điểm bạn muốn xóa khỏi Database.

5️⃣ Xem danh sách địa điểm
Hiển thị bảng dữ liệu hiện có trong Database (ID, Tên, Mức độ kẹt xe).

6️⃣ & 7️⃣ Quản lý Lịch sử
Xem lịch sử: Xem lại các lần chạy trước đó (Ngày giờ, Tổng quãng đường, Lộ trình tóm tắt).

Xóa lịch sử: Xóa sạch bảng History để làm mới dữ liệu báo cáo.

8️⃣ Công cụ lấy tọa độ (Map Picker) - ✨ Tính năng nổi bật
Đây là công cụ hỗ trợ nhập liệu thông minh.

Chọn menu số 8, chương trình sẽ mở file chon_dia_diem.html.

Trên trình duyệt, nhập tên địa điểm vào ô tìm kiếm (VD: "Dai hoc Su pham").

Web sẽ tự tìm tọa độ và điền thông tin.

Bấm nút "Sao chép vào Tool C++".

Quay lại chương trình C++, chọn Menu 2 và Paste dữ liệu vào.

4. Kịch bản sử dụng mẫu (Demo Scenario)
Để Demo đồ án cho giảng viên, bạn hãy làm theo trình tự sau:

Bước 1: Chọn 8 để mở bản đồ lấy tọa độ. Tìm một địa điểm mới (VD: "Bitexco") -> Copy dữ liệu.

Bước 2: Chọn 2 (Thêm địa điểm) -> Paste dữ liệu vừa copy -> Enter.

Bước 3: Chọn 5 để cho thấy địa điểm mới đã vào Database với ID mới.

Bước 4: Chọn 1 để chạy thuật toán. Chờ thanh loading chạy xong.

Bước 5: Mở file ket_qua_ban_do.html vừa sinh ra.

Chỉ vào Biểu đồ để thấy đường đi giảm dần thế nào.

Chỉ vào Bản đồ để thấy lộ trình thực tế.

Bước 6: Chọn 6 (Xem lịch sử) để chứng minh kết quả đã được lưu lại.

5. Cấu trúc thư mục
main.cpp: Chương trình chính, xử lý Menu.

Database/: Chứa CityDAO xử lý SQLite.

Algorithms/: Chứa thuật toán GeneticAlgo và Constants.

Models/: Chứa cấu trúc dữ liệu City.

Utils/: Chứa hàm xuất HTML và Map Picker.

MyCityData.db: File cơ sở dữ liệu (được tạo tự động).
