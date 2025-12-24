#ifndef CITYDAO_H
#define CITYDAO_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip> // Để format ngày tháng
#include <ctime>   // Lấy thời gian hiện tại
#include "../sqlite3.h"
#include "../Models/City.h"

using namespace std;

// [MỚI] Struct để chứa thông tin một lần chạy
struct TripLog {
    int id;
    string date;
    double distance;
    double time;
    string route; // Lưu tóm tắt lộ trình (VD: A -> B -> C)
};

class CityDAO {
private:
    const char* dbName;

    // Hàm phụ trợ: Lấy ngày giờ hiện tại dạng chuỗi
    string getCurrentDateTime() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char buffer[80];
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", ltm);
        return string(buffer);
    }

public:
    CityDAO(const char* databaseName);
    void initialize();

    // CRUD Locations (Cũ)
    vector<City> getAllCities();
    bool addCity(City city);
    bool deleteCity(int id);
    void importFromCSV(const char* csvFilePath);

    // [MỚI] Các hàm xử lý Lịch sử (History)
    bool saveTrip(double distance, double time, string routeDetail); // Lưu kết quả
    vector<TripLog> getTripHistory(); // Lấy danh sách lịch sử
    void clearHistory(); // Xóa lịch sử (nếu cần)
};

#endif