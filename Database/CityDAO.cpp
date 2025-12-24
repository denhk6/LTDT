#include "CityDAO.h"
#include <fstream>
#include <sstream>

CityDAO::CityDAO(const char* databaseName) {
    this->dbName = databaseName;
}

void CityDAO::initialize() {
    sqlite3* db;
    char* errMsg = 0;

    if (sqlite3_open(dbName, &db)) {
        cerr << "[Loi] Khong the mo database!" << endl;
        return;
    }

    // Tạo bảng Locations (Có cột id tự tăng)
    const char* sqlTable =
        "CREATE TABLE IF NOT EXISTS Locations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "latitude REAL NOT NULL,"
        "longitude REAL NOT NULL,"
        "traffic_level INTEGER DEFAULT 1);";

    sqlite3_exec(db, sqlTable, 0, 0, &errMsg);

    //Bảng History (Lưu lịch sử chạy)
    const char* sqlTableHist =
        "CREATE TABLE IF NOT EXISTS History ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "run_date TEXT,"
        "total_distance REAL,"
        "total_time REAL,"
        "route_summary TEXT);";
    sqlite3_exec(db, sqlTableHist, 0, 0, &errMsg);

    // Kiểm tra dữ liệu rỗng để nạp mẫu
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT count(*) FROM Locations", -1, &stmt, 0);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count == 0) {
        cout << "[INFO] Nap du lieu mau..." << endl;
        const char* sqlInsert =
            "INSERT INTO Locations (name, latitude, longitude, traffic_level) VALUES "
            "('Cho_Ben_Thanh', 10.772109, 106.698285, 3),"
            "('Landmark_81', 10.794967, 106.721758, 2),"
            "('San_Bay_TSN', 10.818507, 106.658824, 3),"
            "('Bitexco', 10.771661, 106.704467, 1),"
            "('Thao_Cam_Vien', 10.787627, 106.705138, 1),"
            "('DH_Hutech', 10.801642, 106.714493, 2);";
        sqlite3_exec(db, sqlInsert, 0, 0, &errMsg);
    }
    sqlite3_close(db);
}

vector<City> CityDAO::getAllCities() {
    vector<City> list;
    sqlite3* db;
    sqlite3_stmt* stmt;

    if (sqlite3_open(dbName, &db)) return list;

    // [QUAN TRỌNG] Phải SELECT id thì mới xóa chính xác được
    const char* sql = "SELECT id, name, latitude, longitude, traffic_level FROM Locations";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0); // Lấy cột ID
            string name = (const char*)sqlite3_column_text(stmt, 1);
            double lat = sqlite3_column_double(stmt, 2);
            double lon = sqlite3_column_double(stmt, 3);
            int traffic = sqlite3_column_int(stmt, 4);

            // Sử dụng Constructor có ID
            list.push_back(City(id, name, lat, lon, traffic));
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return list;
}

bool CityDAO::addCity(City city) {
    sqlite3* db;
    char* errMsg = 0;

    if (sqlite3_open(dbName, &db)) return false;

    char* sql = sqlite3_mprintf("INSERT INTO Locations (name, latitude, longitude, traffic_level) VALUES ('%q', %f, %f, %d);",
                                city.getName().c_str(), city.getLat(), city.getLon(), city.getTrafficLevel());

    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    sqlite3_free(sql);
    sqlite3_close(db);

    if (rc != SQLITE_OK) {
        cerr << "[Loi SQL] " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// [MỚI] Hàm xóa địa điểm theo ID
bool CityDAO::deleteCity(int id) {
    sqlite3* db;
    char* errMsg = 0;

    if (sqlite3_open(dbName, &db)) return false;

    char* sql = sqlite3_mprintf("DELETE FROM Locations WHERE id = %d;", id);

    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    sqlite3_free(sql);
    sqlite3_close(db);

    if (rc != SQLITE_OK) {
        cerr << "[Loi SQL] " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// [SỬA LẠI] Hàm nhập file phù hợp với input.txt (Dấu cách)
// Lưu ý: Nếu bạn muốn dùng CSV thật (dấu phẩy), hãy báo mình để mình sửa lại
void CityDAO::importFromCSV(const char* filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "[Loi] Khong tim thay file: " << filePath << endl;
        return;
    }

    // Đọc số lượng dòng đầu tiên (nếu file input.txt có dòng chứa số lượng)
    // Nếu file CSV chuẩn không có số lượng thì comment dòng dưới này lại
    int n;
    file >> n;

    sqlite3* db;
    if (sqlite3_open(dbName, &db)) return;

    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);

    string name;
    double lat, lon;
    int traffic;
    int count = 0;

    // Đọc theo định dạng: Tên Vĩ_độ Kinh_độ Traffic (cách nhau bởi khoảng trắng)
    while (file >> name >> lat >> lon >> traffic) {
        char* sql = sqlite3_mprintf("INSERT INTO Locations (name, latitude, longitude, traffic_level) VALUES ('%q', %f, %f, %d);",
                                    name.c_str(), lat, lon, traffic);

        if (sqlite3_exec(db, sql, 0, 0, 0) == SQLITE_OK) {
            count++;
        }
        sqlite3_free(sql);
    }

    sqlite3_exec(db, "COMMIT;", 0, 0, 0);
    sqlite3_close(db);
    file.close();

    cout << "-> Da import thanh cong " << count << " dia diem tu file " << filePath << "!" << endl;
}

//Hàm lưu kết quả chuyến đi
bool CityDAO::saveTrip(double distance, double time, string routeDetail) {
    sqlite3* db;
    if (sqlite3_open(dbName, &db)) return false;

    string date = getCurrentDateTime();

    // Insert vào bảng History
    char* sql = sqlite3_mprintf("INSERT INTO History (run_date, total_distance, total_time, route_summary) VALUES ('%q', %f, %f, '%q');",
                                date.c_str(), distance, time, routeDetail.c_str());

    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    sqlite3_free(sql);
    sqlite3_close(db);

    if (rc != SQLITE_OK) {
        cerr << "[Loi SQL History] " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// [MỚI] Hàm lấy toàn bộ lịch sử
vector<TripLog> CityDAO::getTripHistory() {
    vector<TripLog> logs;
    sqlite3* db;
    sqlite3_stmt* stmt;

    if (sqlite3_open(dbName, &db)) return logs;

    const char* sql = "SELECT id, run_date, total_distance, total_time, route_summary FROM History ORDER BY id DESC"; // Lấy mới nhất trước

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            TripLog log;
            log.id = sqlite3_column_int(stmt, 0);
            log.date = (const char*)sqlite3_column_text(stmt, 1);
            log.distance = sqlite3_column_double(stmt, 2);
            log.time = sqlite3_column_double(stmt, 3);
            log.route = (const char*)sqlite3_column_text(stmt, 4);
            logs.push_back(log);
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return logs;
}

// [MỚI] Hàm xóa sạch lịch sử
void CityDAO::clearHistory() {
    sqlite3* db;
    if (sqlite3_open(dbName, &db)) return;
    sqlite3_exec(db, "DELETE FROM History;", 0, 0, 0);
    sqlite3_close(db);
    cout << "-> Da xoa toan bo lich su!" << endl;
}