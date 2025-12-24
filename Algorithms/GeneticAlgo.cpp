#include "GeneticAlgo.h"
#include "Constants.h"
#include <iostream>
#include <cmath>

// --- DistanceManager ---
vector<vector<double>> DistanceManager::matrix;

void DistanceManager::initMatrix(const vector<City>& cities) {
    int n = cities.size();
    matrix.resize(n, vector<double>(n, 0.0));
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(i == j) matrix[i][j] = 0;
            else matrix[i][j] = cities[i].distanceTo(cities[j]);
        }
    }
}

double DistanceManager::getDistance(int index1, int index2) {
    return matrix[index1][index2];
}

// --- Tour ---
Tour::Tour() {}

Tour::Tour(vector<City>& sourceCities) {
    refCities = &sourceCities;
    for(int i=0; i<sourceCities.size(); i++) cityIndices.push_back(i);
}

void Tour::generateIndividual() {
    random_shuffle(cityIndices.begin(), cityIndices.end());
}

City Tour::getCity(int tourPos) const { return (*refCities)[cityIndices[tourPos]]; }
int Tour::getCityIndex(int tourPos) const { return cityIndices[tourPos]; }

void Tour::setCityIndex(int tourPos, int cityIdx) {
    cityIndices[tourPos] = cityIdx;
    fitness = 0; combinedCost = 0;
}

int Tour::tourSize() const { return cityIndices.size(); }

bool Tour::containsCityIndex(int idx) {
    for(int val : cityIndices) if(val == idx) return true;
    return false;
}

void Tour::calculateMetrics() {
    if (combinedCost != 0) return;
    double d = 0; double t = 0;
    double currentTime = 8.0;

    for (int i = 0; i < tourSize(); i++) {
        int idxFrom = getCityIndex(i);
        int idxTo = getCityIndex((i + 1) % tourSize());
        double dist = DistanceManager::getDistance(idxFrom, idxTo);
        City targetCity = getCity((i + 1) % tourSize());
        double speed = targetCity.getSpeedInZone(currentTime);
        double travelTime = dist / speed;

        d += dist; t += travelTime; currentTime += travelTime;
    }
    totalDistance = d; totalTime = t;
    double timeConverted = totalTime * AVG_SPEED_NORMAL;
    combinedCost = (totalDistance * WEIGHT_DISTANCE) + (timeConverted * WEIGHT_TIME);
}

double Tour::getDistance() { calculateMetrics(); return totalDistance; }
double Tour::getTime() { calculateMetrics(); return totalTime; }
double Tour::getFitness() { calculateMetrics(); if (fitness == 0) fitness = 1.0 / combinedCost; return fitness; }

void Tour::twoOpt() {
    int size = tourSize();
    bool improvement = true;
    while (improvement) {
        improvement = false;
        for (int i = 0; i < size - 1; i++) {
            for (int k = i + 1; k < size; k++) {
                int a = getCityIndex(i); int b = getCityIndex((i+1)%size);
                int c = getCityIndex(k); int d = getCityIndex((k+1)%size);
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

// --- Population ---
Population::Population(int populationSize, bool initialise, vector<City>& initialCities) {
    tours.resize(populationSize);
    if (initialise) {
        for (int i = 0; i < populationSize; i++) {
            Tour newTour(initialCities);
            newTour.generateIndividual();
            saveTour(i, newTour);
        }
    }
}
void Population::saveTour(int index, Tour tour) { tours[index] = tour; }
Tour Population::getTour(int index) { return tours[index]; }
Tour Population::getFittest() {
    Tour fittest = tours[0];
    for (int i = 1; i < tours.size(); i++) {
        if (fittest.getFitness() <= getTour(i).getFitness()) fittest = getTour(i);
    }
    return fittest;
}
int Population::populationSize() { return tours.size(); }

// --- GA ---
Population GA::evolvePopulation(Population& pop, vector<City>& refCities) {
    Population newPopulation(pop.populationSize(), false, refCities);
    int elitismOffset = ELITISM ? 1 : 0;
    if (ELITISM) newPopulation.saveTour(0, pop.getFittest());
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

Tour GA::crossover(Tour parent1, Tour parent2, vector<City>& refCities) {
    Tour child(refCities);
    for(int k=0; k<child.tourSize(); k++) child.setCityIndex(k, -1);
    int size = parent1.tourSize();
    int startPos = rand() % size; int endPos = rand() % size;
    if (startPos > endPos) swap(startPos, endPos);
    for (int i = 0; i < size; i++) if (i >= startPos && i <= endPos) child.setCityIndex(i, parent1.getCityIndex(i));
    for (int i = 0; i < size; i++) {
        int currentCityIdx = parent2.getCityIndex(i);
        if (!child.containsCityIndex(currentCityIdx)) {
            for (int j = 0; j < size; j++) {
                if (child.getCityIndex(j) == -1) { child.setCityIndex(j, currentCityIdx); break; }
            }
        }
    }
    return child;
}

void GA::mutate(Tour tour) {
    for(int tourPos1 = 0; tourPos1 < tour.tourSize(); tourPos1++){
        if((double)rand() / RAND_MAX < MUTATION_RATE){
            int tourPos2 = rand() % tour.tourSize();
            int idx1 = tour.getCityIndex(tourPos1); int idx2 = tour.getCityIndex(tourPos2);
            tour.setCityIndex(tourPos2, idx1); tour.setCityIndex(tourPos1, idx2);
        }
    }
}

Tour GA::tournamentSelection(Population& pop) {
    int randomId = rand() % pop.populationSize();
    Tour best = pop.getTour(randomId);
    for (int i = 1; i < TOURNAMENT_SIZE; i++) {
        randomId = rand() % pop.populationSize();
        Tour competitor = pop.getTour(randomId);
        if(competitor.getFitness() > best.getFitness()) best = competitor;
    }
    return best;
}