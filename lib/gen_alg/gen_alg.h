#ifndef GEN_ALG_H
#define GEN_ALG_H

typedef int gene_t;

typedef struct Individual {
    int* genes;
    int length;
} individual_t;

typedef struct Population {
    individual_t* individuals;
    int length;
    int individual_length;
} population_t;

population_t* create_population(int population_length, int individual_length);

void generate_population(population_t* population, int begin);
void evaluate_population(population_t* population, double* population_fitness, int** costs,int* best_index,int* worst_index);
void breed_population(population_t* population, double* population_fitness);
void mutate_population(population_t* population, double mutation_rate);

void breed_parents(individual_t* parent1, individual_t* parent2, individual_t* child, int length);
void print_individual(individual_t* individual);
void destroy_population(population_t* population);
individual_t* create_individual(int individual_length);
void copy_individual(individual_t* individual1, individual_t* individual2);
void destroy_individual(individual_t* individual);

#endif