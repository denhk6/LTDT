#include <iostream>
#include "Database/CityDAO.h"  // Gọi DAO
#include "Algorithms/GeneticAlgo.h"
#include "Utils/Utils.h"

using namespace std;

// Biến static của lớp DistanceManager (Đặt ở đây vì file .cpp của GeneticAlgo chưa tách riêng)
vector<vector<double>> DistanceManager::matrix;

void showMenu() {
    cout << "\n=== MENU ===" << endl;
    cout << "1. Chay thuat toan tim duong (GA)" << endl;
    cout << "2. Them dia diem moi" << endl;
    cout << "0. Thoat" << endl;
    cout << "Chon: ";
}

void inputNewCity(CityDAO& dao) {
    string name;
    double lat, lon;
    int traffic;

    cout << "Nhap ten (khong dau): "; cin >> name;
    cout << "Vi do: "; cin >> lat;
    cout << "Kinh do: "; cin >> lon;
    cout << "Muc do ket xe (1-3): "; cin >> traffic;

    City newCity(name, lat, lon, traffic);
    if(dao.addCity(newCity)) {
        cout << "-> Them thanh cong!" << endl;
    } else {
        cout << "-> Them that bai!" << endl;
    }
}

int main() {
    srand(time(0));
    const char* DB_FILE = "MyCityData.db";

    // 1. Khởi tạo lớp truy cập dữ liệu
    CityDAO dao(DB_FILE);
    dao.initialize(); // Tự động tạo bảng/data nếu chưa có

    int choice;
    do {
        showMenu();
        cin >> choice;

        if (choice == 1) {
            // Lấy dữ liệu mới nhất từ DB
            vector<City> cities = dao.getAllCities();
            if(cities.empty()) {
                cout << "Chua co du lieu!" << endl;
                continue;
            }

            cout << "--- BAT DAU TIM DUONG VOI " << cities.size() << " DIEM ---" << endl;

            // Khởi tạo ma trận & Quần thể
            DistanceManager::initMatrix(cities);
            Population pop(POPULATION_SIZE, true, cities);

            // Chạy thuật toán (Code cũ của bạn)
            for (int i = 0; i < MAX_GENERATIONS; i++) {
                pop = GA::evolvePopulation(pop, cities);
            }

            Tour best = pop.getFittest();
            best.twoOpt(); // Tối ưu lần cuối

            cout << "Ket qua: " << best.getDistance() << " km" << endl;
            exportToHTML(best, "ket_qua.html", best.getDistance(), best.getTime());

        } else if (choice == 2) {
            inputNewCity(dao);
        }

    } while (choice != 0);

    return 0;
}