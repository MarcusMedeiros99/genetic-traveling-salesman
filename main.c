#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/gen_alg/gen_alg.h"
#include "lib/simulation/simulation.h"

#define GNUPLOT "gnuplot -persist"

#define POPULATION_SIZE 25
#define INDIVIDUAL_SIZE 30
#define INF 1000000
#define N_GENERATIONS 1000000

int main(int argc, char const *argv[])
{
    FILE* output_f = fopen("output.txt", "w");
    FILE* ponteirognuplot = NULL;
    ponteirognuplot=popen("gnuplot -persist","w");

    population_t* population = create_population(POPULATION_SIZE, INDIVIDUAL_SIZE);
    individual_t* best_individual = create_individual(INDIVIDUAL_SIZE);
    double global_best_fitness = 0.0;
    double population_fitness[POPULATION_SIZE];
    memset(population_fitness, 0, sizeof(double)*POPULATION_SIZE);
    int** costs = (int**) malloc(INDIVIDUAL_SIZE*sizeof(int*));
    double mutation_rate = 0.001;
    int best_index, worst_index;
    int count_constant_optimal = 0;
    int begin = 0;

    for (int i = 0; i < INDIVIDUAL_SIZE; i++) {
        costs[i] = (int*) malloc(INDIVIDUAL_SIZE*sizeof(int));
        for (int j = 0; j < INDIVIDUAL_SIZE; j++) {
            scanf("%d", costs[i] + j);
            if (i == j) costs[i][j] = INF;
        }
    }

    generate_population(population, begin);
    evaluate_population(population, population_fitness, costs, &best_index, &worst_index);

    for (int i = 0; i < N_GENERATIONS; i++) {
        copy_individual(best_individual, population->individuals + worst_index);
        evaluate_population(population, population_fitness, costs, &best_index, &worst_index);
        if (population_fitness[best_index] == global_best_fitness) count_constant_optimal++;
        if (count_constant_optimal > 100) {
            generate_population(population, begin);
            evaluate_population(population, population_fitness, costs, &best_index, &worst_index);
            copy_individual(best_individual, population->individuals + worst_index);
            count_constant_optimal = 0;
        }
        if (population_fitness[best_index] > global_best_fitness) {
            copy_individual(population->individuals + best_index, best_individual);
            global_best_fitness = population_fitness[best_index];
            // printf("global_best_fitness: %lf\n", global_best_fitness);
        }
        fprintf(output_f, "%d %d\n", i, (int) (1 / population_fitness[best_index]));
        // printf("generation: %d, best_fitness: %lf best_index: %d\n", i, population_fitness[best_index], best_index);
        if (i == N_GENERATIONS - 1) break;
        breed_population(population, population_fitness);
        mutate_population(population, mutation_rate);
    }
    printf("global_best_fitness: %lf\n", global_best_fitness);
    fclose(output_f);
    fprintf(ponteirognuplot, "plot 'output.txt'\n");
    fclose(ponteirognuplot);

    for (int i = 0; i < INDIVIDUAL_SIZE; i++) {
        free(costs[i]);
    }
    free(costs);
    destroy_population(population);
    destroy_individual(best_individual);
    return 0;
}
