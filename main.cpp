#include <iostream>
#include <ctime>
#include <iomanip>
#include "Constants.h"
#include "City.h"
#include "GeneticAlgo.h"
#include "Utils.h"

using namespace std;

int main() {
    srand(time(0));
    cout << "--- HE THONG TOI UU LO TRINH (HIGH PERFORMANCE) ---" << endl;

    // 1. Đọc dữ liệu
    vector<City> cities = readCitiesFromFile("input.txt");

    // [TỐI ƯU 1] KHỞI TẠO MA TRẬN KHOẢNG CÁCH (Chạy 1 lần duy nhất)
    cout << "Dang xay dung ma tran khoang cach (O(N^2))..." << endl;
    DistanceManager::initMatrix(cities);

    // 2. Khởi tạo
    Population pop(POPULATION_SIZE, true, cities);

    cout << fixed << setprecision(2);

    // Biến cho Early Stopping
    int stagnantGenerations = 0;
    double bestFitnessSeen = 0.0;

    // 3. Chạy thuật toán
    cout << "Bat dau tien hoa..." << endl;

    for (int i = 0; i < MAX_GENERATIONS; i++) {
        pop = GA::evolvePopulation(pop, cities);

        Tour currentBest = pop.getFittest();
        double currentFit = currentBest.getFitness();

        // [TỐI ƯU 2] KIỂM TRA DỪNG SỚM
        if (currentFit > bestFitnessSeen) {
            bestFitnessSeen = currentFit;
            stagnantGenerations = 0; // Có tiến bộ -> Reset đếm

            // [TỐI ƯU 3] CHỈ CHẠY 2-OPT KHI CÓ KẾT QUẢ TRIỂN VỌNG MỚI
            // Không chạy liên tục gây chậm máy
            if (i > 10) currentBest.twoOpt();
        } else {
            stagnantGenerations++;
        }

        // Nếu 40 thế hệ liên tiếp không khá hơn -> DỪNG LUÔN
        if (stagnantGenerations >= 40) {
            cout << "\n[INFO] DA HOI TU SOM TAI THE HE: " << i << " (Early Stopping)" << endl;
            break;
        }

        if(i % 50 == 0) {
             cout << "Gen " << setw(3) << i
                  << " | Quang duong: " << currentBest.getDistance() << " km" << endl;
        }
    }

    // 4. Kết quả & Chạy 2-Opt lần cuối cùng (Vét cạn lỗi)
    Tour finalBest = pop.getFittest();
    finalBest.twoOpt(); // Chạy kỹ lần cuối

    cout << "\n--- KET QUA CUOI CUNG ---" << endl;
    cout << "Tong Quang Duong: " << finalBest.getDistance() << " km" << endl;
    cout << "Tong Thoi Gian:   " << finalBest.getTime() << " gio" << endl;

    exportToHTML(finalBest, "ket_qua_ban_do.html", finalBest.getDistance(), finalBest.getTime());

    cout << "\n(Nhan Enter de thoat...)";
    cin.ignore(); cin.get();
    return 0;
}