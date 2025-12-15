#ifndef CITY_H
#define CITY_H

#include <iostream>
#include <cmath>
#include <string>
#include "../Algorithms/Constants.h"

using namespace std;

class City {
private:
    double lat, lon; // Tọa độ thực (Kinh độ, Vĩ độ)
    string name;
    int trafficLevel;

public:
    City(string name, double lat, double lon, int traffic)
        : name(name), lat(lat), lon(lon), trafficLevel(traffic) {}

    // Các hàm lấy dữ liệu (Getter)
    double getLat() const { return lat; }
    double getLon() const { return lon; }
    string getName() const { return name; }
    int getTrafficLevel() const { return trafficLevel; }

    // Hàm tính khoảng cách giữa 2 điểm GPS (đơn vị: Km)
    double distanceTo(const City& city) const {
        // 1. Tính khoảng cách đường chim bay (Haversine simplified)
        // 1 độ vĩ tuyến ~ 111.32 km
        double latDist = (lat - city.getLat()) * 111.32;
        // 1 độ kinh tuyến thay đổi theo vĩ độ (nhân với cos)
        double lonDist = (lon - city.getLon()) * 111.32 * cos(lat * 3.14159 / 180);

        // Pytago: căn bậc 2 của tổng bình phương
        double straightDist = sqrt(latDist * latDist + lonDist * lonDist);

        // 2. Áp dụng HỆ SỐ ĐƯỜNG BỘ (Tortuosity Factor)
        // Vì thực tế xe phải chạy vòng vèo, không bay thẳng được
        // Hệ số 1.4 là mức trung bình tại các đô thị Việt Nam
        double ROAD_FACTOR = 1.4;

        return straightDist * ROAD_FACTOR;
    }

    // Hàm trả về tốc độ di chuyển tùy theo tình trạng giao thông
    double getSpeedInZone(double arrivalHour) const {
        double speed = 0;

        // Logic cơ bản theo Traffic Level
        switch (trafficLevel) {
            case 1: speed = 40.0; break;
            case 2: speed = 20.0; break;
            case 3: speed = 5.0; break;
            default: speed = 30.0;
        }

        // Logic nâng cao: GIỜ CAO ĐIỂM (Rush Hour)
        // Chuyển giờ về khung 24h (ví dụ 25.5h tức là 1h30 sáng hôm sau)
        double hourOfDay = fmod(arrivalHour, 24.0);

        bool isRushHour = (hourOfDay >= 7.0 && hourOfDay <= 9.0) ||
                          (hourOfDay >= 17.0 && hourOfDay <= 19.0);

        if (isRushHour) {
            return speed * 0.6; // Giảm còn 60% tốc độ nếu dính giờ cao điểm
        }
        return speed;
    }
};
#endif