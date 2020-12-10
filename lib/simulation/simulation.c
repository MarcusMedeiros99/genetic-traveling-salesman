#include "simulation.h"

int simulate(individual_t* individual,int** costs) {
    int cost = 0;
    for (int i = 1; i < individual->length; i++) {
        cost += costs[individual->genes[i-1]][individual->genes[i]];
    }
    cost += costs[individual->genes[individual->length - 1]][individual->genes[0]];
    return cost;
}