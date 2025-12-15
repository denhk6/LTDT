cmake_minimum_required(VERSION 3.20)
project(LTDT)

set(CMAKE_CXX_STANDARD 14)

# Báo cho CMake biết cần tìm file header ở đâu
include_directories(
        .
        Models
        Database
        Algorithms
        Utils
)

# Liệt kê tất cả các file mã nguồn (.cpp) cần biên dịch
add_executable(LTDT
        main.cpp
        sqlite3.c
        Database/CityDAO.cpp  # Đừng quên file này
        # Các file .h không cần liệt kê ở đây, nhưng liệt kê cũng không sao
)

if(UNIX)
    target_link_libraries(LTDT pthread dl)
endif()