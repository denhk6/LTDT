#ifndef CONSTANTS_H
#define CONSTANTS_H

// Cân bằng Đa mục tiêu
const double WEIGHT_DISTANCE = 0.4;
const double WEIGHT_TIME = 0.6;
const double AVG_SPEED_NORMAL = 40.0; // Tốc độ trung bình trong phố (40km/h)

// Cấu hình GA
const double MUTATION_RATE = 0.02;
const int TOURNAMENT_SIZE = 5;
const bool ELITISM = true;
const int POPULATION_SIZE = 50;
const int MAX_GENERATIONS = 200;

#endif