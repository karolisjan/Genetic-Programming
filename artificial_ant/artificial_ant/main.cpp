#include "options.h"

#if TESTING != 1

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include "genetic_programming.h"

using namespace GP;

int main()
{
	SetUp(CONFIG_FILEPATH);

	std::vector<Individual> population(popsize);
	Individual best;

	for (int i = 0; i < popsize; ++i) {
		population[i].program = BuildTree(max_depth, tree_generation_method);

		Evaluate(population[i]);

		if (population[i] < best) {
			best = population[i];
		}
	}

	int epochs = generations * popsize;

	for (int epoch = 0; epoch < epochs; ++epoch) {
		Individual &offspring = population[Tournament(population, tournament_size, IsGreater<Individual>)]
			= population[Tournament(population, tournament_size, IsLess<Individual>)];

		if (random<float>() < pcrossover) {
			SubtreeXO(offspring.program, population[Tournament(population, tournament_size, IsLess<Individual>)].program);
		}
		else {
			SubtreeMutation(offspring.program);
		}

		Evaluate(offspring);

		if (offspring < best)
			best = offspring;


		if (((epoch + 1) % popsize) == 0) {
			printf("Gen: %d, Program size: %d, Performance: %.4f\n", (1 + epoch / popsize), best.program.size(), best.fitness);
			
			if (best.fitness == 0)
				break;
		}
	}

	printf("\n");
	system("pause");

	return 0;
}

#endif // !TESTING
