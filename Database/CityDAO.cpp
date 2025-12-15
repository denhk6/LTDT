#include "CityDAO.h"

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

    // Tạo bảng nếu chưa có
    const char* sqlTable = 
        "CREATE TABLE IF NOT EXISTS Locations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "latitude REAL NOT NULL,"
        "longitude REAL NOT NULL,"
        "traffic_level INTEGER DEFAULT 1);";

    sqlite3_exec(db, sqlTable, 0, 0, &errMsg);

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

    const char* sql = "SELECT name, latitude, longitude, traffic_level FROM Locations";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string name = (const char*)sqlite3_column_text(stmt, 0);
            double lat = sqlite3_column_double(stmt, 1);
            double lon = sqlite3_column_double(stmt, 2);
            int traffic = sqlite3_column_int(stmt, 3);
            list.push_back(City(name, lat, lon, traffic));
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

    // Sử dụng sqlite3_mprintf để an toàn dữ liệu
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