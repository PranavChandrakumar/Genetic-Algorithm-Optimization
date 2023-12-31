// Include everything necessary here
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "functions.h"

double generate_random(double min, double max) // srand() is seeded in GA.c
{ 
    // implement a function to return a value between min and max
    double random_value = (double)rand() / RAND_MAX;
    return (min + random_value * (max - min));
}

unsigned int generate_int()
{
    // implement the function to return a random integer value
    return rand();
}

// Function to initialize a random population
void generate_population(int POPULATION_SIZE, int NUM_VARIABLES, double population[POPULATION_SIZE][NUM_VARIABLES], double Lbound[NUM_VARIABLES], double Ubound[NUM_VARIABLES])
{
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        for (int j = 0; j < NUM_VARIABLES; j++)
        {
            population[i][j] = generate_random(Lbound[j], Ubound[j]); //Generate population array
        }
    }
}

// Function to compute the objective function for each member of the population
void compute_objective_function(int POPULATION_SIZE, int NUM_VARIABLES, double population[POPULATION_SIZE][NUM_VARIABLES], double fitness[POPULATION_SIZE])
{
    /* compute "fitness[i]"" for each set of decision variables (individual) or each row in "population"
    by calling "Objective_function" */
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        fitness[i] = 1 / (1e-16 + Objective_function(NUM_VARIABLES, population[i]));
    }
}

void crossover(int POPULATION_SIZE, int NUM_VARIABLES, double fitness[POPULATION_SIZE], double new_population[POPULATION_SIZE][NUM_VARIABLES], double population[POPULATION_SIZE][NUM_VARIABLES], double crossover_rate)
{
    /* Implement the logic of crossover function here based on "fitness_probs" or each set
    of decision variables (individual) or each row in "population".
    And save the new population in "new_population"*/
    double fitness_sum = 0.0;
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        fitness_sum += fitness[i]; // Getting the sum of all fitness values
    }

    double prob_chromosomes[POPULATION_SIZE];
    double cumulative_prob[POPULATION_SIZE];

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        prob_chromosomes[i] = 0.0;
        cumulative_prob[i] = 0.0; //Reinitialize all the probability arrays to zero for every new crossover
    }

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        prob_chromosomes[i] = fitness[i] / fitness_sum; // Probability of each chromosome
        int j = i;
        while (j >= 0){
            cumulative_prob[i]+= prob_chromosomes[j]; //Creating cumulative probability array
            j--;
        }
    }

    double random_values[POPULATION_SIZE];
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        random_values[i] = generate_random(0,1); //Random values for "Roulette Wheel" probability picking
    }

    int switch_list[POPULATION_SIZE]; // Which chromosome set each member of population will be switched with
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        for (int j = 0; j < POPULATION_SIZE; j++)
        {
            if (random_values[i] <= cumulative_prob[j])
            {
                switch_list[i] = j;
                break;
            }
        }
    }

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        for (int j = 0; j < NUM_VARIABLES; j++)
        {
            new_population[i][j] = population[switch_list[i]][j]; //Switching out individuals with more fit individuals
        }
    }

    int indexes_used = 0; // Keep a counter of how many parents go to list of cross_parents
    int cross_parents[POPULATION_SIZE]; // List of which parents will cross with one another

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        double cross_test = generate_random(0,1);
        if (cross_test < crossover_rate)
        {
            cross_parents[indexes_used] = i;
            indexes_used++;
        }
    }

    if (indexes_used > 1) //Check to macke sure there's more than one parent selected to cross
    {
        double stored_switched_value;
        int chromosome_number_switched[indexes_used]; // Which chromosome is switched between each selected parent
        for (int i = 0; i < indexes_used; i++)
        {
            chromosome_number_switched[i] = (int)generate_random(0, NUM_VARIABLES);
        }
        for (int i = 0; i < indexes_used; i++)
        {
            stored_switched_value = 0.0;
            if (i != (indexes_used - 1)) // Ensures that the rightmost element in the list does nor reference an index to its right
            {                                                                                             
                stored_switched_value += new_population[cross_parents[i]][chromosome_number_switched[i]]; // Parent A chromosome 1 swaps with Parent B chromosome 1, this value needs to be stored for the second switch
                new_population[cross_parents[i]][chromosome_number_switched[i]] = new_population[cross_parents[i + 1]][chromosome_number_switched[i]];
                new_population[cross_parents[i + 1]][chromosome_number_switched[i]] = stored_switched_value;
            }
            else // Condition to crossover genes from the rightmost parent in the list to the leftmost parent
            { 
                stored_switched_value += new_population[cross_parents[i]][chromosome_number_switched[i]];
                new_population[cross_parents[i]][chromosome_number_switched[i]] = new_population[cross_parents[0]][chromosome_number_switched[i]];
                new_population[cross_parents[0]][chromosome_number_switched[i]] = stored_switched_value;
            }
        }
    }
}

void mutate(int POPULATION_SIZE, int NUM_VARIABLES, double new_population[POPULATION_SIZE][NUM_VARIABLES], double population[POPULATION_SIZE][NUM_VARIABLES], double Lbound[NUM_VARIABLES], double Ubound[NUM_VARIABLES], double mutate_rate)
{
    /*Implement the logic of mutation on "new_population" and then copy everything into "population"*/
    const int total_chromosomes = POPULATION_SIZE * NUM_VARIABLES;
    const int num_mutations = total_chromosomes * mutate_rate;

    for (int i = 0; i < num_mutations; i++)
    {
        int mutated_gene = (int)generate_random(0, total_chromosomes);
        int row_counter = 0; //Keep track of of which row of the population array (parent) is getting a mutation
        while (mutated_gene > NUM_VARIABLES) //Check to see which parent the mutate gene will be in
        {
            mutated_gene -= NUM_VARIABLES;
            row_counter++; 
        }
        if (mutated_gene != 0) //Checks if the mutated gene is in the same row as row_counter, or in the row prior to it (if mutated_gene = 0)
        {
            new_population[row_counter][mutated_gene - 1] = generate_random(Lbound[NUM_VARIABLES], Ubound[NUM_VARIABLES]);
        }
        else
        {
            new_population[row_counter - 1][NUM_VARIABLES] = generate_random(Lbound[NUM_VARIABLES], Ubound[NUM_VARIABLES]);
        }
    }

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        for (int j = 0; j < NUM_VARIABLES; j++)
        {
            population[i][j] = new_population[i][j];
        }
    }
}