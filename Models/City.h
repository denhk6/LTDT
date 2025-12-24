#ifndef CITY_H
#define CITY_H

#include <iostream>
#include <cmath>
#include <string>
#include "../Algorithms/Constants.h"

using namespace std;

class City {
private:
    int id;          // ID từ Database (0 nếu chưa lưu)
    double lat, lon;
    string name;
    int trafficLevel;

public:
    // Constructor 1: Dùng khi lấy từ Database (có ID)
    City(int id, string name, double lat, double lon, int traffic)
        : id(id), name(name), lat(lat), lon(lon), trafficLevel(traffic) {}

    // Constructor 2: Dùng khi nhập mới (ID mặc định = 0)
    City(string name, double lat, double lon, int traffic)
        : id(0), name(name), lat(lat), lon(lon), trafficLevel(traffic) {}

    // Getter
    int getId() const { return id; }
    double getLat() const { return lat; }
    double getLon() const { return lon; }
    string getName() const { return name; }
    int getTrafficLevel() const { return trafficLevel; }

    // Hàm tính khoảng cách (Giữ nguyên)
    double distanceTo(const City& city) const {
        double latDist = (lat - city.getLat()) * 111.32;
        double lonDist = (lon - city.getLon()) * 111.32 * cos(lat * 3.14159 / 180);
        double straightDist = sqrt(latDist * latDist + lonDist * lonDist);
        double ROAD_FACTOR = 1.4;
        return straightDist * ROAD_FACTOR;
    }

    // Hàm tính tốc độ (Giữ nguyên)
    double getSpeedInZone(double arrivalHour) const {
        double speed = 0;
        switch (trafficLevel) {
            case 1: speed = 40.0; break;
            case 2: speed = 20.0; break;
            case 3: speed = 5.0; break;
            default: speed = 30.0;
        }
        double hourOfDay = fmod(arrivalHour, 24.0);
        bool isRushHour = (hourOfDay >= 7.0 && hourOfDay <= 9.0) ||
                          (hourOfDay >= 17.0 && hourOfDay <= 19.0);
        if (isRushHour) return speed * 0.6;
        return speed;
    }
};
#endif