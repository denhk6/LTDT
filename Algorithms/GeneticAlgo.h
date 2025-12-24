#ifndef GENETICALGO_H
#define GENETICALGO_H

#include <vector>
#include <algorithm>
#include <cstdlib>
#include "../Models/City.h"

using namespace std;

class DistanceManager {
public:
    static vector<vector<double>> matrix;
    static void initMatrix(const vector<City>& cities);
    static double getDistance(int index1, int index2);
};

class Tour {
private:
    vector<int> cityIndices;
    vector<City>* refCities;
    double fitness = 0;
    double totalDistance = 0;
    double totalTime = 0;
    double combinedCost = 0;

public:
    Tour();
    Tour(vector<City>& sourceCities);
    void generateIndividual();
    City getCity(int tourPos) const;
    int getCityIndex(int tourPos) const;
    void setCityIndex(int tourPos, int cityIdx);
    int tourSize() const;
    bool containsCityIndex(int idx);
    void calculateMetrics();
    double getDistance();
    double getTime();
    double getFitness();
    void twoOpt();
};

class Population {
private:
    vector<Tour> tours;
public:
    Population(int populationSize, bool initialise, vector<City>& initialCities);
    void saveTour(int index, Tour tour);
    Tour getTour(int index);
    Tour getFittest();
    int populationSize();
};

class GA {
public:
    static Population evolvePopulation(Population& pop, vector<City>& refCities);
    static Tour crossover(Tour parent1, Tour parent2, vector<City>& refCities);
    static void mutate(Tour tour);
    static Tour tournamentSelection(Population& pop);
};

#endif