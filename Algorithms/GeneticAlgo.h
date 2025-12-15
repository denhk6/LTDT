#ifndef GENETICALGO_H
#define GENETICALGO_H

#include <vector>
#include <algorithm>
#include <cstdlib>
#include "../Models/City.h"
#include "../Algorithms/Constants.h"

using namespace std;

// --- CLASS QUẢN LÝ KHOẢNG CÁCH ---
class DistanceManager {
public:
    // Chỉ khai báo static ở đây (Declaration)
    static vector<vector<double>> matrix;

    static void initMatrix(const vector<City>& cities) {
        int n = cities.size();
        matrix.resize(n, vector<double>(n, 0.0));
        for(int i=0; i<n; i++) {
            for(int j=0; j<n; j++) {
                if(i == j) matrix[i][j] = 0;
                else matrix[i][j] = cities[i].distanceTo(cities[j]);
            }
        }
    }

    static double getDistance(int index1, int index2) {
        return matrix[index1][index2];
    }
};

// [QUAN TRỌNG] ĐÃ XÓA DÒNG KHỞI TẠO TẠI ĐÂY ĐỂ TRÁNH LỖI LINKER SAU NÀY
// Dòng bị xóa: vector<vector<double>> DistanceManager::matrix;

// --- CLASS TOUR ---
class Tour {
    // ... (Giữ nguyên toàn bộ code class Tour bên dưới)
    // ...
private:
    vector<int> cityIndices;
    vector<City>* refCities;
    double fitness = 0;
    double totalDistance = 0;
    double totalTime = 0;
    double combinedCost = 0;

public:
    Tour() {}
    Tour(vector<City>& sourceCities) {
        refCities = &sourceCities;
        for(int i=0; i<sourceCities.size(); i++) cityIndices.push_back(i);
    }

    // ... (Giữ nguyên các hàm bên trong Tour)
    void generateIndividual() { random_shuffle(cityIndices.begin(), cityIndices.end()); }

    City getCity(int tourPos) const { return (*refCities)[cityIndices[tourPos]]; }
    int getCityIndex(int tourPos) const { return cityIndices[tourPos]; }

    void setCityIndex(int tourPos, int cityIdx) {
        cityIndices[tourPos] = cityIdx;
        fitness = 0; combinedCost = 0;
    }

    int tourSize() const { return cityIndices.size(); }

    bool containsCityIndex(int idx) {
        for(int val : cityIndices) if(val == idx) return true;
        return false;
    }

    void calculateMetrics() {
        if (combinedCost != 0) return;
        double d = 0;
        double t = 0;
        double currentTime = 8.0;

        for (int i = 0; i < tourSize(); i++) {
            int idxFrom = getCityIndex(i);
            int idxTo = getCityIndex((i + 1) % tourSize());
            double dist = DistanceManager::getDistance(idxFrom, idxTo);
            City targetCity = getCity((i + 1) % tourSize());
            double speed = targetCity.getSpeedInZone(currentTime);
            double travelTime = dist / speed;

            d += dist;
            t += travelTime;
            currentTime += travelTime;
        }
        totalDistance = d;
        totalTime = t;
        double timeConverted = totalTime * AVG_SPEED_NORMAL;
        combinedCost = (totalDistance * WEIGHT_DISTANCE) + (timeConverted * WEIGHT_TIME);
    }

    double getDistance() { calculateMetrics(); return totalDistance; }
    double getTime() { calculateMetrics(); return totalTime; }

    double getFitness() {
        calculateMetrics();
        if (fitness == 0) fitness = 1.0 / combinedCost;
        return fitness;
    }

    void twoOpt() {
        int size = tourSize();
        bool improvement = true;
        while (improvement) {
            improvement = false;
            for (int i = 0; i < size - 1; i++) {
                for (int k = i + 1; k < size; k++) {
                    int a = getCityIndex(i);
                    int b = getCityIndex((i+1)%size);
                    int c = getCityIndex(k);
                    int d = getCityIndex((k+1)%size);

                    double currentDist = DistanceManager::getDistance(a, b) + DistanceManager::getDistance(c, d);
                    double newDist = DistanceManager::getDistance(a, c) + DistanceManager::getDistance(b, d);

                    if (newDist < currentDist - 0.0001) {
                        reverse(cityIndices.begin() + i + 1, cityIndices.begin() + k + 1);
                        improvement = true;
                        fitness = 0; combinedCost = 0;
                    }
                }
            }
        }
    }
};

// --- CLASS POPULATION ---
class Population {
    // ... (Giữ nguyên code Class Population)
private:
    vector<Tour> tours;
public:
    Population(int populationSize, bool initialise, vector<City>& initialCities) {
        tours.resize(populationSize);
        if (initialise) {
            for (int i = 0; i < populationSize; i++) {
                Tour newTour(initialCities);
                newTour.generateIndividual();
                saveTour(i, newTour);
            }
        }
    }
    void saveTour(int index, Tour tour) { tours[index] = tour; }
    Tour getTour(int index) { return tours[index]; }
    Tour getFittest() {
        Tour fittest = tours[0];
        for (int i = 1; i < tours.size(); i++) {
            if (fittest.getFitness() <= getTour(i).getFitness()) fittest = getTour(i);
        }
        return fittest;
    }
    int populationSize() { return tours.size(); }
};

// --- CLASS GA ---
class GA {
    // ... (Giữ nguyên code Class GA)
public:
    static Population evolvePopulation(Population& pop, vector<City>& refCities) {
        Population newPopulation(pop.populationSize(), false, refCities);
        int elitismOffset = 0;
        if (ELITISM) {
            newPopulation.saveTour(0, pop.getFittest());
            elitismOffset = 1;
        }
        for (int i = elitismOffset; i < pop.populationSize(); i++) {
            Tour parent1 = tournamentSelection(pop);
            Tour parent2 = tournamentSelection(pop);
            Tour child = crossover(parent1, parent2, refCities);
            newPopulation.saveTour(i, child);
        }
        for (int i = elitismOffset; i < newPopulation.populationSize(); i++) {
            mutate(newPopulation.getTour(i));
        }
        return newPopulation;
    }

    static Tour crossover(Tour parent1, Tour parent2, vector<City>& refCities) {
        Tour child(refCities);
        for(int k=0; k<child.tourSize(); k++) child.setCityIndex(k, -1);
        int size = parent1.tourSize();
        int startPos = rand() % size;
        int endPos = rand() % size;
        if (startPos > endPos) swap(startPos, endPos);
        for (int i = 0; i < size; i++) {
            if (i >= startPos && i <= endPos)
                child.setCityIndex(i, parent1.getCityIndex(i));
        }
        for (int i = 0; i < size; i++) {
            int currentCityIdx = parent2.getCityIndex(i);
            if (!child.containsCityIndex(currentCityIdx)) {
                for (int j = 0; j < size; j++) {
                    if (child.getCityIndex(j) == -1) {
                        child.setCityIndex(j, currentCityIdx); break;
                    }
                }
            }
        }
        return child;
    }

    static void mutate(Tour tour) {
        for(int tourPos1 = 0; tourPos1 < tour.tourSize(); tourPos1++){
            if((double)rand() / RAND_MAX < MUTATION_RATE){
                int tourPos2 = rand() % tour.tourSize();
                int idx1 = tour.getCityIndex(tourPos1);
                int idx2 = tour.getCityIndex(tourPos2);
                tour.setCityIndex(tourPos2, idx1);
                tour.setCityIndex(tourPos1, idx2);
            }
        }
    }

    static Tour tournamentSelection(Population& pop) {
        int randomId = rand() % pop.populationSize();
        Tour best = pop.getTour(randomId);
        for (int i = 1; i < TOURNAMENT_SIZE; i++) {
            randomId = rand() % pop.populationSize();
            Tour competitor = pop.getTour(randomId);
            if(competitor.getFitness() > best.getFitness()) best = competitor;
        }
        return best;
    }
};

#endif