#ifndef CITYDAO_H
#define CITYDAO_H

#include <vector>
#include <string>
#include <iostream>
#include "../sqlite3.h"      // Trỏ về file sqlite3.h ở thư mục gốc
#include "../Models/City.h"  // Gọi class City

using namespace std;

class CityDAO {
private:
    const char* dbName; // Tên file database

public:
    // Constructor
    CityDAO(const char* databaseName);

    // 1. Tạo bảng và dữ liệu mẫu
    void initialize();

    // 2. Lấy toàn bộ danh sách thành phố
    vector<City> getAllCities();

    // 3. Thêm một thành phố mới
    bool addCity(City city);
};

#endif