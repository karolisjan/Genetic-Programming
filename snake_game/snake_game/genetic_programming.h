#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <stdio.h>
#include <algorithm>

class GP
{
public:
	void SetUp(int seed, int popsize, int gens, float p_crossover, int tournament_size,
		float p_pt_mutation, int max_program_length, int max_init_depth, bool verbose = false);

	std::string Run(std::string seed_prog = "");

private:
	int seed;
	int gens;
	int popsize;
	int	max_init_depth;
	int max_program_length;
	int	tournament_size;
	float p_pt_mutation;
	float p_crossover;
	float p_grow;
	bool verbose;

	typedef std::string::iterator Node;
	enum generation_method { GROW, FULL };

	struct Individual
	{
		Individual() : fitness(INT_MAX) {}
		Individual(const Individual &other) : fitness(other.fitness), program(other.program) {}

		Individual &operator=(const Individual &rhs)
		{
			this->fitness = rhs.fitness;
			this->program = rhs.program;

			return *this;
		}

		bool operator<(const Individual &rhs)
		{
			return this->fitness < rhs.fitness;
		}

		bool operator>(const Individual &rhs)
		{
			return !operator<(rhs);
		}

		float fitness;
		std::string	program;
	};

	std::vector<Individual> population;

	std::string BuildTree(int max_depth, int method);

	template<class Cnt, class Cmp>
	int Tournament(Cnt &container, int tsize, Cmp comparison);

	std::pair<Node, Node> SelectSubtree(std::string &program);

	void SubtreeXO(std::string &p1, std::string &p2);

	void SubtreeMutation(std::string &program);

	void PtMutation(std::string &program);
};
