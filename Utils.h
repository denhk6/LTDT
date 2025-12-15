#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <direct.h>
#include <iomanip>
#include "City.h"
#include "GeneticAlgo.h"

using namespace std;

// HÀM 1: ĐỌC FILE (Giữ nguyên)
vector<City> readCitiesFromFile(string filename) {
    vector<City> data;
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "--- LOI: KHONG TIM THAY FILE " << filename << " ---" << endl;
        exit(1);
    }
    int n; inputFile >> n;
    string name; double lat, lon; int traffic;
    for (int i = 0; i < n; i++) {
        inputFile >> name >> lat >> lon >> traffic;
        data.push_back(City(name, lat, lon, traffic));
    }
    inputFile.close();
    cout << "Da doc du lieu GPS thanh cong!" << endl;
    return data;
}

// HÀM 2: XUẤT HTML VỚI CƠ CHẾ AUTO-FIX LỖI MẠNG
void exportToHTML(Tour bestTour, string filename, double distance, double time) {
    ofstream f(filename);

    // HEADER HTML & CSS
    f << R"(
    <!DOCTYPE html>
    <html lang="vi">
    <head>
        <meta charset="UTF-8">
        <title>Lộ Trình Giao Hàng</title>
        <link rel="stylesheet" href="style.css">
        <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
        <style>
            /* CSS cho icon số thứ tự */
            .custom-div-icon { background: transparent; border: none; }
            .marker-pin {
                width: 30px; height: 30px; border-radius: 50% 50% 50% 0;
                background: #c0392b; position: absolute; transform: rotate(-45deg);
                left: 50%; top: 50%; margin: -15px 0 0 -15px;
                box-shadow: 2px 2px 5px rgba(0,0,0,0.3);
            }
            .marker-pin::after {
                content: ''; width: 20px; height: 20px; margin: 5px 0 0 5px;
                background: #fff; position: absolute; border-radius: 50%;
            }
            .marker-number {
                position: absolute; top: -2px; left: 0; width: 30px;
                text-align: center; font-weight: bold; font-size: 14px; color: #333; z-index: 100;
            }
            /* Loading Spinner */
            .loader { border: 4px solid #f3f3f3; border-top: 4px solid #3498db; border-radius: 50%; width: 20px; height: 20px; animation: spin 1s linear infinite; display: inline-block; vertical-align: middle; margin-right: 10px;}
            @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>Lộ Trình Giao Hàng Tối Ưu</h1>
            <div class="subtitle">Đồ án môn Lý thuyết Đồ thị - Nhóm 4</div>

            <div class="stats-row">
                <div class="stat-card">
                    <div class="stat-value">)";
    f << fixed << setprecision(2) << distance;
    f << R"( km</div><div class="stat-label">Tổng Quãng Đường</div></div>
                <div class="stat-card time">
                    <div class="stat-value">)";
    f << fixed << setprecision(2) << time;
    f << R"( giờ</div><div class="stat-label">Thời Gian Dự Kiến</div></div>
            </div>

            <div id="status-bar" style="text-align:center; color:#555; margin-bottom:10px; font-style:italic;">
                <div id="loading" class="loader"></div> <span id="status-text">Đang kết nối vệ tinh tìm đường...</span>
            </div>

            <div id="map"></div>

            <div class="legend">
                <div class="legend-item"><span class="dot" style="background-color:green;"></span>Thoáng</div>
                <div class="legend-item"><span class="dot" style="background-color:orange;"></span>Đông</div>
                <div class="legend-item"><span class="dot" style="background-color:red;"></span>Kẹt</div>
            </div>
        </div>

        <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>

        <script>
            // TỌA ĐỘ TRUNG TÂM
            var startLat = )" << bestTour.getCity(0).getLat() << R"(;
            var startLon = )" << bestTour.getCity(0).getLon() << R"(;

            var map = L.map('map').setView([startLat, startLon], 13);

            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                maxZoom: 19, attribution: '© OpenStreetMap'
            }).addTo(map);

            // DỮ LIỆU ĐIỂM ĐẾN
            var waypoints = [
    )";

    // Vòng lặp in tọa độ
    for (int i = 0; i < bestTour.tourSize(); i++) {
        City city = bestTour.getCity(i);
        f << "{lat: " << city.getLat() << ", lon: " << city.getLon() << ", name: '" << city.getName() << "', traffic: " << city.getTrafficLevel() << "}," << endl;
    }
    // Quay về điểm đầu
    f << "{lat: " << bestTour.getCity(0).getLat() << ", lon: " << bestTour.getCity(0).getLon() << ", name: 'Về đích', traffic: 1}" << endl;

    f << R"(
            ];

            // 1. VẼ CÁC ĐIỂM MARKER (SỐ THỨ TỰ)
            var latlngs = [];
            waypoints.forEach((pt, index) => {
                latlngs.push([pt.lat, pt.lon]); // Lưu tọa độ để vẽ đường

                if (index === waypoints.length - 1) return; // Bỏ qua điểm cuối trùng lặp

                let bgColor = '#27ae60';
                if(index === 0) bgColor = '#2c3e50';
                else if(pt.traffic == 2) bgColor = '#f39c12';
                else if(pt.traffic == 3) bgColor = '#c0392b';

                var myIcon = L.divIcon({
                    className: 'custom-div-icon', // Class này phải khớp với CSS ở trên
                    // HTML chỉ chứa 2 thẻ: div (giọt nước) và span (số)
                    html: `<div style="background-color: ${bgColor};" class="marker-pin"></div><span class="marker-number">${index}</span>`,
                    iconSize: [30, 42],
                    iconAnchor: [15, 42]
                });

                let status = pt.traffic == 3 ? 'Kẹt xe' : (pt.traffic == 2 ? 'Đông' : 'Thoáng');
                L.marker([pt.lat, pt.lon], {icon: myIcon}).addTo(map)
                 .bindPopup(`<b>TRẠM ${index}: ${pt.name}</b><br>Tình trạng: ${status}`);
            });

            // 2. HÀM VẼ ĐƯỜNG THẲNG (DỰ PHÒNG KHI MẠNG LỖI)
            function drawStraightLines() {
                var polyline = L.polyline(latlngs, {
                    color: '#3498db', weight: 4, opacity: 0.8, dashArray: '10, 10', lineJoin: 'round'
                }).addTo(map);
                map.fitBounds(polyline.getBounds());

                // Cập nhật thông báo
                document.getElementById('loading').style.display = 'none';
                document.getElementById('status-text').innerHTML = "Đã hiển thị chế độ đường bay (Do máy chủ OSRM quá tải).";
                document.getElementById('status-text').style.color = "orange";
            }

            // 3. GỌI API TÌM ĐƯỜNG (CÓ TIMEOUT 3 GIÂY)
            var coordString = waypoints.map(pt => pt.lon + ',' + pt.lat).join(';');
            var url = `https://router.project-osrm.org/route/v1/driving/${coordString}?overview=full&geometries=geojson`;

            // Tạo bộ đếm thời gian: Quá 3 giây là hủy
            const controller = new AbortController();
            const timeoutId = setTimeout(() => controller.abort(), 3000);

            fetch(url, { signal: controller.signal })
                .then(response => {
                    if (!response.ok) throw new Error("Server Error");
                    return response.json();
                })
                .then(data => {
                    if(data.code === 'Ok') {
                        // Vẽ đường uốn lượn thành công
                        L.geoJSON(data.routes[0].geometry, {
                            style: { color: 'blue', weight: 5, opacity: 0.7 }
                        }).addTo(map);

                        document.getElementById('loading').style.display = 'none';
                        document.getElementById('status-text').innerText = "Đã tải xong lộ trình đường bộ thực tế!";
                        document.getElementById('status-text').style.color = "green";
                    } else {
                        throw new Error("API Code not OK");
                    }
                })
                .catch(err => {
                    console.log("Lỗi tải đường bộ hoặc quá thời gian: ", err);
                    drawStraightLines(); // GỌI HÀM VẼ THẲNG NGAY LẬP TỨC
                });

        </script>
    </body>
    </html>
    )";

    f.close();
    cout << "\n[INFO] Da xuat bao cao HTML (Che do an toan) tai: " << filename << endl;
}
#endif