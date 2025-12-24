cmake_minimum_required(VERSION 3.20)
project(LTDT)

set(CMAKE_CXX_STANDARD 14)

include_directories(
        .
        Models
        Database
        Algorithms
        Utils
)

add_executable(LTDT
        main.cpp
        sqlite3.c
        Database/CityDAO.cpp
        Algorithms/GeneticAlgo.cpp
)

if(UNIX)
    target_link_libraries(LTDT pthread dl)
endif()