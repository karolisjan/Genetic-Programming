#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <stdio.h>
#include <algorithm>

#include "utils/utils.hpp"
#include "functions_terminals.hpp"


class GP
{
public:
	void SetUp(int seed, int popsize, int gens, float p_crossover, int tournament_size,
		float p_pt_mutation, int max_program_length, int max_init_depth, bool verbose = false);

	std::string Run();

private:
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

	typedef std::string::iterator Node;

	enum generation_method { GROW, FULL };

	template <typename R, typename ... Types> constexpr size_t GetArgumentCount(R(*f)(Types ...))
	{
		return sizeof...(Types);
	}

	inline int ArityMin1(char &x, char dummy = '0')
	{
		if (x < FunctionsTerminals::FSET_START)
			return -1;

		return GetArgumentCount(Jumptable::jumptable[x]) - 1;
	}

	template<class T>
	inline bool IsGreater(T &lhs, T &rhs) { return lhs > rhs; }

	template<class T>
	inline bool IsLess(T &lhs, T &rhs) { return lhs < rhs; }

	std::string BuildTree(int max_depth, int method);

	template<class Cnt, class Cmp>
	int Tournament(Cnt &container, int tsize, Cmp comparison);

	pair<Node, Node> SelectSubtree(std::string &program);

	void SubtreeXO(std::string &p1, std::string &p2);

	void SubtreeMutation(std::string &program);

	void PtMutation(std::string &program);

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

	std::vector<Individual> population;
};
