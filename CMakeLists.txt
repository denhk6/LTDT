cmake_minimum_required(VERSION 4.0)
project(LTDT)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(LTDT
        main.cpp
        Constants.h
        City.h
        GeneticAlgo.h
        Utils.h
)
