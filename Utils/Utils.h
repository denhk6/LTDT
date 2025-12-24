#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "../Models/City.h"
#include "../Algorithms/GeneticAlgo.h"

using namespace std;

// HÀM : XUẤT HTML KÈM BIỂU ĐỒ HIỆU NĂNG
// Thêm tham số: history (lịch sử độ dài quãng đường qua từng thế hệ)
void exportToHTML(Tour bestTour, string filename, double distance, double time, const vector<double>& history) {
    ofstream f(filename);

    // 1. CHUẨN BỊ DỮ LIỆU BIỂU ĐỒ (C++) -> (JS Array)
    string chartData = "[";
    for(size_t i = 0; i < history.size(); i++) {
        chartData += to_string(history[i]);
        if(i < history.size() - 1) chartData += ",";
    }
    chartData += "]";

    string labels = "[";
    for(size_t i = 0; i < history.size(); i++) {
        labels += to_string(i); // Thế hệ 0, 1, 2...
        if(i < history.size() - 1) labels += ",";
    }
    labels += "]";

    // 2. GHI HTML
    f << R"(
    <!DOCTYPE html>
    <html lang="vi">
    <head>
        <meta charset="UTF-8">
        <title>Dashboard Giao Hàng</title>
        <link rel="stylesheet" href="style.css">
        <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    </head>
    <body>
        <div class="container">
            <h1>Lộ Trình Giao Hàng Tối Ưu</h1>
            <div class="subtitle">Đồ án Kỹ thuật Phần mềm & Graph Theory</div>

            <div class="stats-row">
                <div class="stat-card">
                    <div class="stat-value">)";
    f << fixed << setprecision(2) << distance;
    f << R"( km</div><div class="stat-label">Tổng Quãng Đường</div></div>
                <div class="stat-card time">
                    <div class="stat-value">)";
    f << fixed << setprecision(2) << time;
    f << R"( giờ</div><div class="stat-label">Thời Gian Dự Kiến</div></div>
                <div class="stat-card" style="background: linear-gradient(135deg, #2ecc71 0%, #27ae60 100%);">
                     <div class="stat-value">GA</div>
                     <div class="stat-label">Thuật toán Di truyền</div>
                </div>
            </div>

            <div style="display: flex; gap: 20px; flex-wrap: wrap;">
                <div style="flex: 6; min-width: 300px;">
                    <div id="status-bar" style="text-align:center; color:#555; margin-bottom:10px; font-style:italic;">
                        <div id="loading" class="loader"></div> <span id="status-text">Đang tối ưu lộ trình...</span>
                    </div>
                    <div id="map"></div>
                </div>

                <div style="flex: 4; min-width: 300px; background: #fff; padding: 10px; border-radius: 12px; border: 1px solid #ddd;">
                    <h3 style="text-align:center; color:#555;">Tiến trình Tối ưu hóa (Generations)</h3>
                    <canvas id="fitnessChart"></canvas>
                </div>
            </div>

            <div class="legend">
                <div class="legend-item"><span class="dot" style="background-color:green;"></span>Thoáng</div>
                <div class="legend-item"><span class="dot" style="background-color:orange;"></span>Đông</div>
                <div class="legend-item"><span class="dot" style="background-color:red;"></span>Kẹt</div>
            </div>
        </div>

        <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>

        <script>
            // --- PHẦN 1: VẼ BIỂU ĐỒ (CHART.JS) ---
            const ctx = document.getElementById('fitnessChart').getContext('2d');
            new Chart(ctx, {
                type: 'line',
                data: {
                    labels: )" << labels << R"(,
                    datasets: [{
                        label: 'Quãng đường (km)',
                        data: )" << chartData << R"(,
                        borderColor: '#667eea',
                        backgroundColor: 'rgba(102, 126, 234, 0.2)',
                        borderWidth: 2,
                        tension: 0.3, // Làm mượt đường cong
                        pointRadius: 1
                    }]
                },
                options: {
                    responsive: true,
                    plugins: {
                        legend: { display: false },
                        tooltip: { mode: 'index', intersect: false }
                    },
                    scales: {
                        x: { title: { display: true, text: 'Thế hệ (Generation)' } },
                        y: { title: { display: true, text: 'Km' } }
                    }
                }
            });

            // --- PHẦN 2: BẢN ĐỒ LEAFLET (GIỮ NGUYÊN) ---
            var startLat = )" << bestTour.getCity(0).getLat() << R"(;
            var startLon = )" << bestTour.getCity(0).getLon() << R"(;
            var map = L.map('map').setView([startLat, startLon], 13);
            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', { maxZoom: 19 }).addTo(map);

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

            var latlngs = [];
            waypoints.forEach((pt, index) => {
                latlngs.push([pt.lat, pt.lon]);
                if (index === waypoints.length - 1) return;

                let bgColor = '#27ae60';
                if(index === 0) bgColor = '#2c3e50';
                else if(pt.traffic == 2) bgColor = '#f39c12';
                else if(pt.traffic == 3) bgColor = '#c0392b';

                var myIcon = L.divIcon({
                    className: 'custom-div-icon',
                    html: `<div style="background-color: ${bgColor};" class="marker-pin"></div><span class="marker-number">${index}</span>`,
                    iconSize: [30, 42],
                    iconAnchor: [15, 42]
                });

                let status = pt.traffic == 3 ? 'Kẹt xe' : (pt.traffic == 2 ? 'Đông' : 'Thoáng');
                L.marker([pt.lat, pt.lon], {icon: myIcon}).addTo(map)
                 .bindPopup(`<b>TRẠM ${index}: ${pt.name}</b><br>Tình trạng: ${status}`);
            });

            function drawStraightLines() {
                var polyline = L.polyline(latlngs, {
                    color: '#3498db', weight: 4, opacity: 0.8, dashArray: '10, 10', lineJoin: 'round'
                }).addTo(map);
                map.fitBounds(polyline.getBounds());
                document.getElementById('loading').style.display = 'none';
                document.getElementById('status-text').innerHTML = "Chế độ đường thẳng (Backup).";
                document.getElementById('status-text').style.color = "orange";
            }

            var coordString = waypoints.map(pt => pt.lon + ',' + pt.lat).join(';');
            var url = `https://router.project-osrm.org/route/v1/driving/${coordString}?overview=full&geometries=geojson`;

            const controller = new AbortController();
            const timeoutId = setTimeout(() => controller.abort(), 3000);

            fetch(url, { signal: controller.signal })
                .then(response => { if (!response.ok) throw new Error("Err"); return response.json(); })
                .then(data => {
                    if(data.code === 'Ok') {
                        L.geoJSON(data.routes[0].geometry, { style: { color: 'blue', weight: 5, opacity: 0.7 } }).addTo(map);
                        document.getElementById('loading').style.display = 'none';
                        document.getElementById('status-text').innerText = "Đã tải xong lộ trình đường bộ!";
                        document.getElementById('status-text').style.color = "green";
                    } else throw new Error("API not Ok");
                })
                .catch(err => { drawStraightLines(); });
        </script>
    </body>
    </html>
    )";

    f.close();
    cout << "\n[INFO] Da xuat bao cao HTML (Ban do + Bieu do) tai: " << filename << endl;
}

// Hàm sinh ra file HTML hỗ trợ tìm kiếm địa điểm (Phiên bản an toàn ASCII)
void createMapPickerTool() {
    ofstream f("chon_dia_diem.html");

    // Sử dụng R"HTML(...)HTML" để tránh lỗi biên dịch chuỗi
    f << R"HTML(
    <!DOCTYPE html>
    <html>
    <head>
        <title>Tool Tim Toa Do (Geocoding)</title>
        <meta charset="utf-8">
        <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
        <style>
            body { font-family: 'Segoe UI', sans-serif; margin: 0; padding: 0; display: flex; height: 100vh; }
            #map { flex: 3; height: 100%; }
            #sidebar { flex: 1; padding: 20px; background: #f8f9fa; border-left: 2px solid #ddd; display: flex; flex-direction: column; gap: 10px; overflow-y: auto; }

            h2 { margin-top: 0; color: #2c3e50; }
            .control-group { background: white; padding: 15px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.05); }

            label { font-weight: bold; display: block; margin-bottom: 5px; font-size: 0.9em; color: #555; }
            input, select, textarea { width: 100%; padding: 10px; margin-bottom: 10px; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }

            button { width: 100%; padding: 12px; background: #3498db; color: white; border: none; cursor: pointer; font-weight: bold; border-radius: 4px; transition: 0.2s; }
            button:hover { background: #2980b9; }
            button.copy-btn { background: #2ecc71; margin-top: 5px; }
            button.copy-btn:hover { background: #27ae60; }

            #spinner { display: none; text-align: center; color: #666; font-style: italic; margin-bottom: 10px; }
        </style>
    </head>
    <body>
        <div id="map"></div>
        <div id="sidebar">
            <h2>Tim kiem & Lay Toa do</h2>

            <div class="control-group" style="border-left: 4px solid #3498db;">
                <label>Nhap dia chi / Ten dia diem:</label>
                <div style="display: flex; gap: 5px;">
                    <input type="text" id="searchQuery" placeholder="VD: Dai hoc Su pham TPHCM" onkeypress="handleEnter(event)">
                </div>
                <button onclick="searchPlace()">Tim kiem Tu dong</button>
                <div id="spinner">Dang tim du lieu ve tinh...</div>
            </div>

            <div class="control-group">
                <label>Ten hien thi (Khong dau):</label>
                <input type="text" id="pName" placeholder="...">

                <div style="display: flex; gap: 10px;">
                    <div style="flex:1">
                        <label>Vi do (Lat):</label>
                        <input type="text" id="pLat" readonly>
                    </div>
                    <div style="flex:1">
                        <label>Kinh do (Lon):</label>
                        <input type="text" id="pLon" readonly>
                    </div>
                </div>

                <label>Tinh trang giao thong:</label>
                <select id="pTraffic">
                    <option value="1">1 - Thoang</option>
                    <option value="2">2 - Trung binh</option>
                    <option value="3">3 - Ket xe</option>
                </select>
            </div>

            <div class="control-group" style="background: #e8f6f3; border: 1px solid #d4efdf;">
                <label><b>Du lieu chuan (Copy dong nay):</b></label>
                <textarea id="finalResult" rows="2" readonly style="font-family: monospace; font-size: 1.1em; color: #c0392b;"></textarea>
                <button class="copy-btn" onclick="copyToClipboard()">Sao chep vao Tool C++</button>
            </div>

            <div style="font-size: 0.8em; color: #888; text-align: center; margin-top: 10px;">
                OpenStreetMap Contributors
            </div>
        </div>

        <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
        <script>
            var map = L.map('map').setView([10.762622, 106.660172], 13);
            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', { maxZoom: 19 }).addTo(map);

            var marker;

            function searchPlace() {
                var query = document.getElementById('searchQuery').value;
                if(!query) { alert("Vui long nhap dia chi!"); return; }

                document.getElementById('spinner').style.display = 'block';

                fetch('https://nominatim.openstreetmap.org/search?format=json&q=' + encodeURIComponent(query) + '&limit=1')
                    .then(response => response.json())
                    .then(data => {
                        document.getElementById('spinner').style.display = 'none';
                        if(data.length > 0) {
                            var lat = data[0].lat;
                            var lon = data[0].lon;
                            var rawName = query;
                            map.setView([lat, lon], 16);
                            updateMarker(lat, lon, rawName);
                        } else {
                            alert("Khong tim thay! Hay thu tu khoa khac.");
                        }
                    })
                    .catch(err => {
                        document.getElementById('spinner').style.display = 'none';
                        alert("Loi ket noi API!");
                    });
            }

            map.on('click', function(e) {
                updateMarker(e.latlng.lat, e.latlng.lng, "Vi_tri_chon_tay");
                fetch('https://nominatim.openstreetmap.org/reverse?format=json&lat=' + e.latlng.lat + '&lon=' + e.latlng.lng)
                    .then(r => r.json())
                    .then(d => {
                        if(d.display_name) {
                            let shortName = d.display_name.split(',')[0];
                            document.getElementById('pName').value = sanitizeName(shortName);
                            updateResultString();
                        }
                    });
            });

            function updateMarker(lat, lon, nameInput) {
                lat = parseFloat(lat).toFixed(6);
                lon = parseFloat(lon).toFixed(6);

                if (marker) map.removeLayer(marker);
                marker = L.marker([lat, lon]).addTo(map).bindPopup("Vi tri da chon").openPopup();

                document.getElementById('pLat').value = lat;
                document.getElementById('pLon').value = lon;
                document.getElementById('pName').value = sanitizeName(nameInput);

                updateResultString();
            }

            function sanitizeName(str) {
                str = str.normalize("NFD").replace(/[\u0300-\u036f]/g, "");
                str = str.replace(/\s+/g, '_');
                str = str.replace(/[^a-zA-Z0-9_]/g, '');
                return str;
            }

            function handleEnter(e) { if(e.key === 'Enter') searchPlace(); }

            document.getElementById('pTraffic').addEventListener('change', updateResultString);
            document.getElementById('pName').addEventListener('input', updateResultString);

            function updateResultString() {
                var name = document.getElementById('pName').value;
                var lat = document.getElementById('pLat').value;
                var lon = document.getElementById('pLon').value;
                var traffic = document.getElementById('pTraffic').value;

                var str = name + " " + lat + " " + lon + " " + traffic;
                document.getElementById('finalResult').value = str;
            }

            function copyToClipboard() {
                var copyText = document.getElementById("finalResult");
                copyText.select();
                navigator.clipboard.writeText(copyText.value);
                var btn = document.querySelector('.copy-btn');
                var originalText = btn.innerText;
                btn.innerText = "Da Copy!";
                setTimeout(() => btn.innerText = originalText, 2000);
            }
        </script>
    </body>
    </html>
    )HTML";

    f.close();
    cout << "\n[INFO] Da cap nhat Tool Tim Kiem: chon_dia_diem.html" << endl;
}
#endif