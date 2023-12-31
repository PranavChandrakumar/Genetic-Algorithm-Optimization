// Include everything necessary here
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "functions.h"

void rand_seed() // Randomizes seed for generate_random function each iteration
{
    time_t t;
    srand((unsigned)(char)time(&t));
}

bool input_validation(int argc, char *argv[]) 
{
    if (argc != 6)
    {
        printf("Invalid number of inputs\n");
        return false;
    }

    for (int i = 1; i <= 5; i++)
    {
        if (i == 1 || i == 2)
        {
            for (int j = 0; j < strlen(argv[i]); j++)
            {
                if (!isdigit(argv[i][j]))
                {
                    printf("Invalid input, detected a non-integer in a position where an integer is expected (first and second argument)\n"); //Always this line that's executed??
                    return false;
                }
            }
        }
        if (i == 3 || i == 4)
        {
            for (int j = 0; j < strlen(argv[i]); j++)
            {
                if (!isdigit(argv[i][j]) && argv[i][j] != '.')
                {
                    printf("Invalid input, expected an float in the third and fourth arguments\n");
                    return false;
                }
            }
        }
        if (i == 5)
        {
            for (int j = 0; j < strlen(argv[i]); j++)
            {
                if ((!isdigit(argv[i][j]) && argv[i][j] != 'e' && argv[i][j] != '-') || (argv[i][j] == 'e' && (argv[i][j + 1] != '-')))
                {
                    printf("Invalid input in stop_criteria (fifth argument)\n");
                    return false;
                }
            }
        }
    }

    return true;
}  

int main(int argc, char *argv[])
{
    // Handle the possible errors in input data given by the user and say how to execute the code
    if (input_validation(argc, argv) == false)
    {
        exit(0); // Got this from chatGPT
    }

    // Assign all inputs given by the user argv[i] : POPULATION_SIZE, MAX_GENERATIONS, crossover_rate, mutate_rate, stop_criteria
    const int POPULATION_SIZE = atoi(argv[1]); // String argument to integer
    const int MAX_GENERATIONS = atoi(argv[2]);
    const double crossover_rate = strtod(argv[3], NULL); // String argument to double, got this from chatGPT
    const double mutate_rate = strtod(argv[4], NULL);
    const double stop_criteria = strtod(argv[5], NULL);

    // the number of variables
    int NUM_VARIABLES = 3;
    // the lower bounds of variables
    double Lbound[] = {-5.0, -5.0};
    // the upper bounds of variable
    double Ubound[] = {5.0, 5.0};

    // All the initial print outs
    printf("Genetic Algorithm is initiated\n-----------------------------------------------\n");
    printf("Number of variables: %d\n", NUM_VARIABLES);
    printf("Lower Bounds: [%f, %f]\n", Lbound[0], Lbound[1]);
    printf("Upper Bounds: [%f, %f]\n\n", Ubound[0], Ubound[1]);
    printf("Population Size:    %d\n", POPULATION_SIZE);
    printf("Max Generations:    %d\n", MAX_GENERATIONS);
    printf("Crossover Rate:     %.16f\n", crossover_rate);
    printf("Mutation Rate:      %.16f\n", mutate_rate);
    printf("Stopping Criteria:  %.16f\n", stop_criteria);

    printf("\nResults\n-----------------------------------------------\n");

    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();

    // Declare all the arrays here
    double population[POPULATION_SIZE][NUM_VARIABLES];
    double fitness[POPULATION_SIZE];
    double new_population[NUM_VARIABLES][NUM_VARIABLES];

    //Call generate_population function to initialize the "population"
    rand_seed(); // Seeding the random number generator

    generate_population(POPULATION_SIZE, NUM_VARIABLES, population, Lbound, Ubound);

    // Iteration starts here. The loop continues until MAX_GENERATIONS is reached or stopping criteria is met

    double best_fitness_original = 0.0; //Keeps track of the best fitness computed across all generations
    double current_best_fitness; //Keeps track of the best fitness computed in the current generation
    int parent; //Keeps track of which member of the population has the best fitness
    double genes[NUM_VARIABLES];
    int counter = 0; //This actively keeps track of how many guesses in a row are within the stop criteria, so that the algorithm has time to "mutate out" of the best solution it currently has

    for (int generation = 0; generation < MAX_GENERATIONS; generation++)
    {
        // Compute the fitness values using objective function for each row in "population" (each set of variables)
        compute_objective_function(POPULATION_SIZE, NUM_VARIABLES, population, fitness);

        //Here implement the logic of finding best solution with minimum fitness value and the stopping criteria
        current_best_fitness = 0.0; //Reinitalize the current best fitness for every generation
        for (int i = 0; i < POPULATION_SIZE; i++)
        { 
            if (fitness[i] > current_best_fitness)
            {
                parent = i;
                current_best_fitness = fitness[i];
            }
        }
        
        if ((fabs(current_best_fitness - best_fitness_original) < stop_criteria)){
            if (counter == MAX_GENERATIONS/5) //Arbitrary upperbound for counter threshold. I set it as one fifth of the total number of generations (one fifth of the generations have to sequentially generate a best fitness that is within the stop criteria).
            {
                if (current_best_fitness > best_fitness_original)
                {
                    for (int i = 0; i < NUM_VARIABLES; i++)
                    {
                    genes[i] = population[parent][i];
                    }
                    printf("Stop criteria reached at generation %d\n",generation);
                    best_fitness_original = current_best_fitness;
                    break;
                }
                else{
                    printf("Stop criteria reached at generation %d\n", generation);
                    break;
                }
            }
            else{
                counter++;
                if (current_best_fitness > best_fitness_original)
                {
                    for (int i = 0; i < NUM_VARIABLES; i++)
                    {
                        genes[i] = population[parent][i];
                    }
                    best_fitness_original = current_best_fitness;
                }
            }
        }
        else if(current_best_fitness > best_fitness_original) // We only care about the parent which has the best fitness possible
        {
            counter = 0;
            for (int i = 0; i < NUM_VARIABLES; i++)
            {
                genes[i] = population[parent][i];
            }
            best_fitness_original = current_best_fitness;
        }

        crossover(POPULATION_SIZE, NUM_VARIABLES, fitness, new_population, population, crossover_rate);

        mutate(POPULATION_SIZE, NUM_VARIABLES, new_population, population, Lbound, Ubound, mutate_rate);
        // Now a new population has been generated, and it goes to the beginning of loop to re-compute all again
    }

    // Jump to this part of code if the stopping criteria is met before MAX_GENERATIONS is met
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("CPU time: %f seconds\n", cpu_time_used);

    printf("Best Fitness: %.16f\n", best_fitness_original);
    printf("Best Solution: [");
    for(int i = 0; i < NUM_VARIABLES; i++)
    {
        if(i != (NUM_VARIABLES - 1))
        {
            printf("%.16f,", genes[i]);
        }
        else
        {
            printf("%.16f", genes[i]);
        }
    }
    printf("]\n");

    return 0;
}