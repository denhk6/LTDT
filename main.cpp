#include <iostream>
#include <ctime>
#include <iomanip>
#include "Constants.h"
#include "City.h"
#include "GeneticAlgo.h"
#include "Utils.h"

using namespace std;

// [SỬA LỖI TẠI ĐÂY]
// Đặt dòng khởi tạo biến static ở file .cpp (Global Scope)
vector<vector<double>> DistanceManager::matrix;

int main() {
    srand(time(0));
    cout << "--- HE THONG TOI UU LO TRINH (HIGH PERFORMANCE) ---" << endl;

    // ... (Giữ nguyên toàn bộ phần còn lại của hàm main)

    // 1. Đọc dữ liệu
    vector<City> cities = readCitiesFromFile("input.txt");

    // [TỐI ƯU 1] KHỞI TẠO MA TRẬN KHOẢNG CÁCH (Chạy 1 lần duy nhất)
    cout << "Dang xay dung ma tran khoang cach (O(N^2))..." << endl;
    DistanceManager::initMatrix(cities);

    // 2. Khởi tạo
    Population pop(POPULATION_SIZE, true, cities);

    cout << fixed << setprecision(2);

    int stagnantGenerations = 0;
    double bestFitnessSeen = 0.0;

    cout << "Bat dau tien hoa..." << endl;

    for (int i = 0; i < MAX_GENERATIONS; i++) {
        pop = GA::evolvePopulation(pop, cities);
        Tour currentBest = pop.getFittest();
        double currentFit = currentBest.getFitness();

        if (currentFit > bestFitnessSeen) {
            bestFitnessSeen = currentFit;
            stagnantGenerations = 0;
            if (i > 10) currentBest.twoOpt();
        } else {
            stagnantGenerations++;
        }

        if (stagnantGenerations >= 40) {
            cout << "\n[INFO] DA HOI TU SOM TAI THE HE: " << i << " (Early Stopping)" << endl;
            break;
        }

        if(i % 50 == 0) {
             cout << "Gen " << setw(3) << i
                  << " | Quang duong: " << currentBest.getDistance() << " km" << endl;
        }
    }

    Tour finalBest = pop.getFittest();
    finalBest.twoOpt();

    cout << "\n--- KET QUA CUOI CUNG ---" << endl;
    cout << "Tong Quang Duong: " << finalBest.getDistance() << " km" << endl;
    cout << "Tong Thoi Gian:   " << finalBest.getTime() << " gio" << endl;

    exportToHTML(finalBest, "ket_qua_ban_do.html", finalBest.getDistance(), finalBest.getTime());

    cout << "\n(Nhan Enter de thoat...)";
    cin.ignore(); cin.get();
    return 0;
}