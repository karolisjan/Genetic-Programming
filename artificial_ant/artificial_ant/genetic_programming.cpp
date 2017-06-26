#include "genetic_programming.h"
#include "utils/utils.hpp"
#include "primitives.h"

#include <functional>


//template<class R, class... Args>
//constexpr unsigned Arity(std::function<R(Args...)> const&) {
//	return sizeof...(Args);
//}

template<class T>
inline bool IsGreater(T &lhs, T &rhs) { return lhs > rhs; }

template<class T>
inline bool IsLess(T &lhs, T &rhs) { return lhs < rhs; }

void GP::SetUp(int seed, int popsize, int gens, float p_crossover, int tournament_size, 
	float p_pt_mutation, int max_program_length, int max_init_depth, bool verbose)
{
	this->popsize = popsize;
	this->gens = gens;
	this->p_crossover = p_crossover;
	this->tournament_size = tournament_size;
	this->p_pt_mutation = p_pt_mutation;
	this->max_program_length = max_program_length;
	this->max_init_depth = max_init_depth;
	this->verbose = verbose;

	srand(seed);

	p_grow = (float)Primitives::TSET_END /
		(Primitives::TSET_END + Primitives::FSET_END - Primitives::FSET_START + 1);
}

std::string GP::Run()
{
	population = std::vector<Individual>(popsize);

	for (auto &i : population) {
		i.program = BuildTree(max_init_depth, random<int>(0, 1));
		i.fitness = Primitives::Evaluate(i.program);
	}

	Individual best = *std::min_element(population.begin(), population.end());

	int epochs = gens * popsize;

	for (int epoch = 0; epoch < epochs; ++epoch) {
		Individual &offspring = population[Tournament(population, tournament_size, IsGreater<Individual>)]
			= population[Tournament(population, tournament_size, IsLess<Individual>)];

		if (random<float>() < p_crossover) {
			SubtreeXO(offspring.program, population[Tournament(population, tournament_size, IsLess<Individual>)].program);
		}
		else {
			SubtreeMutation(offspring.program);
		}

		offspring.fitness = Primitives::Evaluate(offspring.program);

		if (offspring < best)
			best = offspring;

		if (verbose & ((epoch + 1) % popsize) == 0) {
			printf("Gen: %d, Program size: %d, Performance: %.4f\n", (1 + epoch / popsize), best.program.size(), best.fitness);
		}

		if (best.fitness == 0)
			break;
	}

	return best.program;
}

std::string GP::BuildTree(int max_depth, int method)
{ 
	if (max_depth == 0 || (method == GROW && random<float>() < p_grow))
		return std::string(1, random<char>(Primitives::TSET_START, Primitives::TSET_END));

	char primitive = random<int>(Primitives::FSET_START, Primitives::FSET_END);
	std::string program = std::string(1, primitive);

	for (int i = 0; i < Primitives::ArityMin1(primitive) + 1; ++i)
		program += BuildTree(max_depth - 1, method);

	return program;
}

template<class Cnt, class Cmp>
int GP::Tournament(Cnt &container, int tsize, Cmp comparison)
{
	int p1 = random<int>(container.size() - 1);

	if (tsize == 1)
		return p1;

	int p2 = Tournament(container, tsize - 1, comparison);

	if (comparison(container[p1], container[p2]) >= 1)
		return p1;
	else
		return p2;
}

std::pair<GP::Node, GP::Node> GP::SelectSubtree(std::string &program)
{
	Node end, begin = end = program.begin() + Tournament(program, 3, Primitives::ArityMin1); // beginning of the tree

	int children = 1;
	for (; children > 0; children += Primitives::ArityMin1(*end++)) {} //finds the end of the subtree, i.e. when children = 0;

	return std::make_pair(begin, end);
}

void GP::SubtreeXO(std::string &p1, std::string &p2)
{
	std::pair<Node, Node> range1 = SelectSubtree(p1), range2 = SelectSubtree(p2);

	if ((p1.size() - (range1.second - range1.first) + (range2.second - range2.first)) < max_program_length)
		p1.replace(range1.first, range1.second, range2.first, range2.second);
}

void GP::SubtreeMutation(std::string &program)
{
	std::string new_program = BuildTree(random(1, max_init_depth), random<int>(0, 2));

	SubtreeXO(program, new_program);
}

void GP::PtMutation(std::string &program)
{
	for (Node n = program.begin(); n != program.end(); ++n)
		if (random<float>() < p_pt_mutation)
			*n = Primitives::ArityMin1(*n) == 1 ?
			random<char>(Primitives::FSET_START, Primitives::FSET_END) : random<char>(1, Primitives::TSET_END);
}


