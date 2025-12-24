#include <iostream>
#include <iomanip>
#include <fstream>
#include "Database/CityDAO.h"
#include "Algorithms/GeneticAlgo.h"
#include "Utils/Utils.h"

using namespace std;

// Hàm nhập thành phố thủ công
void inputNewCity(CityDAO& dao) {
    string name;
    double lat, lon;
    int traffic;
    cout << "Nhap ten (khong dau): "; cin >> name;
    cout << "Vi do: "; cin >> lat;
    cout << "Kinh do: "; cin >> lon;
    cout << "Muc do ket xe (1-3): "; cin >> traffic;
    // Dùng constructor nhập liệu (không cần ID)
    if(dao.addCity(City(name, lat, lon, traffic))) cout << "-> Them thanh cong!" << endl;
    else cout << "-> Them that bai!" << endl;
}

// Hàm nhập từ file
void importFromFile(CityDAO& dao) {
    string filename = "input.txt";
    ifstream file(filename);
    if (!file.is_open()) { cout << "[Loi] Khong tim thay " << filename << endl; return; }

    int n; file >> n; // Đọc số lượng
    string name; double lat, lon; int traffic;
    int count = 0;
    while(file >> name >> lat >> lon >> traffic) {
        if(dao.addCity(City(name, lat, lon, traffic))) count++;
    }
    file.close();
    cout << "-> Da nhap thanh cong " << count << " dia diem tu file." << endl;
}

// Hàm xóa địa điểm theo ID
void deleteCityUI(CityDAO& dao) {
    vector<City> cities = dao.getAllCities();
    cout << "\n--- CHON ID DE XOA ---" << endl;
    cout << left << setw(5) << "ID" << setw(20) << "Ten" << endl;
    for(const auto& c : cities) cout << left << setw(5) << c.getId() << setw(20) << c.getName() << endl;

    int id;
    cout << "Nhap ID can xoa: "; cin >> id;
    if (dao.deleteCity(id)) cout << "-> Xoa thanh cong!" << endl;
    else cout << "-> Xoa that bai (ID khong ton tai)!" << endl;
}

//Hàm hiển thị lịch sử
void showHistoryUI(CityDAO& dao) {
    vector<TripLog> history = dao.getTripHistory();
    if(history.empty()) {
        cout << "Chua co lich su nao!" << endl;
        return;
    }

    cout << "\n=== LICH SU CAC LAN CHAY (Moi nhat truoc) ===" << endl;
    cout << left << setw(5) << "ID"
         << setw(20) << "Ngay gio"
         << setw(10) << "Quang duong"
         << setw(10) << "Thoi gian" << endl;
    cout << "--------------------------------------------------------" << endl;

    for(const auto& log : history) {
        cout << left << setw(5) << log.id
             << setw(20) << log.date
             << fixed << setprecision(2) << log.distance << " km   "
             << fixed << setprecision(2) << log.time << " h" << endl;
        // In lộ trình tóm tắt ở dòng dưới cho gọn
        cout << "   Lo trinh: " << log.route << "\n" << endl;
    }
}

void showMenu() {
    cout << "\n=== HE THONG GIAO HANG TAI TP.HCM ===" << endl;
    cout << "1. Tim lo trinh toi uu (GA)" << endl;
    cout << "2. Them dia diem (Thu cong)" << endl;
    cout << "3. Nhap du lieu tu file (input.txt)" << endl;
    cout << "4. Xoa dia diem" << endl;
    cout << "5. Xem danh sach dia diem" << endl;
    cout << "6. Xem lich su ket qua" << endl;
    cout << "7. Xoa lich su" << endl;
    cout << "8. Mo cong cu lay toa do (Map Picker)" << endl; // <--- MỚI
    cout << "0. Thoat" << endl;
    cout << "Chon chuc nang : ";
}

int main() {
    srand(time(0));
    const char* DB_FILE = "MyCityData.db";
    CityDAO dao(DB_FILE);
    dao.initialize();

    int choice;
    do {
        showMenu();
        cin >> choice;

        if (choice == 1) {
            // ... (Code chạy GA cũ giữ nguyên) ...
            vector<City> cities = dao.getAllCities();
            if(cities.empty()) { cout << "Du lieu trong!" << endl; continue; }

            cout << "--- DANG TINH TOAN VOI " << cities.size() << " DIEM ---" << endl;
            DistanceManager::initMatrix(cities);
            Population pop(POPULATION_SIZE, true, cities);
            vector<double> fitnessHistory;

            cout << "Dang chay GA: ";
            for (int i = 0; i < MAX_GENERATIONS; i++) {
                pop = GA::evolvePopulation(pop, cities);
                fitnessHistory.push_back(pop.getFittest().getDistance());
                if(i % 20 == 0) cout << ".";
            }
            cout << " Xong!" << endl;

            Tour best = pop.getFittest();
            best.twoOpt();
            fitnessHistory.push_back(best.getDistance());

            cout << "Ket qua toi uu: " << best.getDistance() << " km" << endl;
            exportToHTML(best, "ket_qua_ban_do.html", best.getDistance(), best.getTime(), fitnessHistory);

            // --- [CODE MỚI] LƯU VÀO DATABASE ---
            string routeStr = "";
            for(int k=0; k<best.tourSize(); k++) {
                routeStr += best.getCity(k).getName();
                if(k < best.tourSize()-1) routeStr += " -> ";
            }
            // Chỉ lấy 50 ký tự đầu để lưu tóm tắt
            if(routeStr.length() > 50) routeStr = routeStr.substr(0, 47) + "...";

            if(dao.saveTrip(best.getDistance(), best.getTime(), routeStr)) {
                cout << "[DB] Da luu ket qua vao lich su!" << endl;
            }
            // -----------------------------------

        } else if (choice == 2) {
            inputNewCity(dao);
        } else if (choice == 3) {
            dao.importFromCSV("input.txt");
        } else if (choice == 4) {
            deleteCityUI(dao);
        } else if (choice == 5) {
            // Xem danh sách điểm
             vector<City> cities = dao.getAllCities();
            cout << left << setw(5) << "ID" << setw(20) << "Ten" << setw(10) << "Ket xe" << endl;
            for(const auto& c : cities)
                cout << left << setw(5) << c.getId() << setw(20) << c.getName() << setw(10) << c.getTrafficLevel() << endl;
        } else if (choice == 6) {
            showHistoryUI(dao); // Xem lịch sử
        } else if (choice == 7) {
            dao.clearHistory(); // Xóa lịch sử
        }
        else if (choice == 8) {
            // Gọi hàm vừa viết trong Utils.h
            createMapPickerTool();
            // Lệnh mở file HTML tự động (Windows)
            system("start chon_dia_diem.html");
        }
    } while (choice != 0);
    return 0;
}