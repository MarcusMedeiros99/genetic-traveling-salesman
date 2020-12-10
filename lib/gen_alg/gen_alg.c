#include "gen_alg.h"
#include "../simulation/simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef double (*evalute_function_t)(int*, int**, int);

void print_individual(individual_t* individual) {
    for (int i = 0; i < individual->length; i++) {
        printf("%d ", (int) individual->genes[i]);
    }
    printf("\n");
}

individual_t* create_individual(int individual_length) {
    individual_t* individual = (individual_t*) malloc(sizeof(individual_t));
    if (individual == NULL) {
        printf("malloc error\n");
        return NULL;
    }
    
    individual->genes = malloc(individual_length*sizeof(gene_t));
    if (individual->genes == NULL) {
        printf("malloc error\n");
        return NULL;
    }
    individual->length = individual_length;

    return individual;
}

void copy_individual(individual_t* individual1, individual_t* individual2) {
    individual2->length = individual1->length;
    for (int i = 0; i < individual1->length; i++) {
        individual2->genes[i] = individual1->genes[i];
    }
}

void destroy_individual(individual_t* individual) {
    free(individual->genes);
    free(individual);
}

population_t* create_population(int population_length, int individual_length) {
    population_t* population = malloc(sizeof(population_t));
    population->individuals = malloc(population_length*sizeof(individual_t));
    for (int i = 0; i < population_length; i++) {
        population->individuals[i].length = individual_length;
        population->individuals[i].genes = malloc(individual_length*sizeof(gene_t));
    }
    population->length = population_length;
    population->individual_length = individual_length;
    return population;
}

void destroy_population(population_t* population) {
    for (int i = 0; i < population->length; i++) {
        free(population->individuals[i].genes);
    }
    free(population->individuals);
    free(population);
}

void generate_individual(individual_t* individual, int begin) {
    int possible_genes[individual->length];
    memset(possible_genes, 0, sizeof(int)*individual->length);

    individual->genes[0] = begin;
    possible_genes[begin] = 1;

    for (int i = 1; i < individual->length; i++) {
        gene_t gene;
        do {
            gene = (gene_t) rand() % individual->length;
        } while(possible_genes[(int) gene] == 1);
        possible_genes[(int) gene] = 1;
        individual->genes[i] = gene;
    }
}

void generate_population(population_t* population, int begin) {
    srand(time(NULL));
    for (int i = 0; i < population->length; i++) {
        generate_individual(population->individuals + i, begin);
    }
    
}

double evaluate_individual(individual_t* individual, int** costs) {
    return (double) 1 / (double) simulate(individual, costs);
}

void evaluate_population(population_t* population, double* population_fitness, int** costs,int* best_index,int* worst_index) {
    *best_index = 0;
    *worst_index = 0;
    for (int i = 0; i < population->length; i++) {
        population_fitness[i] = evaluate_individual(population->individuals + i, costs);
        // printf("population_fitness: %lf\n", population_fitness[i]);
        if (population_fitness[i] > population_fitness[*best_index]) *best_index = i;
        if (population_fitness[i] < population_fitness[*worst_index]) *worst_index = i;
    }
}

void breed_parents(individual_t* parent1, individual_t* parent2, individual_t* child, int length) {
    int start_crossover;
    int end_crossover;
    // print_individual(parent1);
    // print_individual(parent2);
    start_crossover = rand() % length;
    end_crossover = rand() % length;

    if (start_crossover > end_crossover) {
        int swap_aux = start_crossover;
        start_crossover = end_crossover;
        end_crossover = swap_aux;
    }

    int is_in_child[length];
    memset(is_in_child, 0, sizeof(int)*length);
    int j = 0;
    for (int i = start_crossover; i <= end_crossover; i++) {
        child->genes[j] = parent1->genes[i];
        is_in_child[child->genes[j]] = 1;
        // printf("i %d\n", i);
        j++;
    }
    
    
    for (int i = 0; i < length; i++) {
        // if (i >= start_crossover && i <= end_crossover) {
        //     continue;
        // }
        if (is_in_child[parent2->genes[i]]) {
            continue;
        }
        // printf("i %d j %d\n", i, j);
        child->genes[j] = parent2->genes[i];
        is_in_child[parent2->genes[i]] = 1;
        j++;
    }
    // print_individual(child);
}

individual_t* tournament(population_t* population, double* population_fitness) {
    int individual1 = rand() % population->length;
    int individual2 = rand() % population->length;
    // printf("%d %d\n", individual1, individual2);

    if (population_fitness[individual1] > population_fitness[individual2]) {
        return population->individuals + individual1;
    }
    return population->individuals + individual2;
}

void breed_population(population_t* population, double* population_fitness) {
    gene_t new_population[population->length][population->individual_length];
    for (int i = 0; i < population->length; i++) {
        individual_t* parent1 = tournament(population, population_fitness);
        individual_t* parent2 = tournament(population, population_fitness);
        individual_t* child = create_individual(parent1->length);
        if (child == NULL || child->genes == NULL) {
            printf("malloc error\n");
        }
        breed_parents(parent1, parent2, child, population->individual_length);
        for (int j = 0; j < population->individual_length; j++) {
            new_population[i][j] = child->genes[j];
        }

        free(child->genes);
        free(child);
    }
    
    for (int i = 0; i < population->length; i++) {
        for (int j = 0; j < population->individual_length; j++) {
            population->individuals[i].genes[j] = new_population[i][j];
        }
    }

}

void mutate_individual(individual_t* individual, double mutation_rate) {
    for (int i = 2; i < individual->length; i++) {
        double p = rand()/RAND_MAX;
        if (p < mutation_rate) {
            gene_t swap_aux = individual->genes[i - 1];
            individual->genes[i - 1] = individual->genes[i];
            individual->genes[i] = swap_aux;
        }
    }
}

void mutate_population(population_t* population, double mutation_rate) {
    for (int i = 0; i < population->length; i++) {
        mutate_individual(population->individuals + i, mutation_rate);
    }
}