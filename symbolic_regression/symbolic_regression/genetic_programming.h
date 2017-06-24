#pragma once

#include "ini_reader.hpp"
#include "helpers.h"
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <sstream>

using namespace std;

struct Individual
{
				Individual() : fitness(INT_MAX) {}
				Individual(Individual &other) : fitness(other.fitness), program(other.program) {}
	
				Individual	&operator=(Individual &rhs)
	{
		this->fitness = rhs.fitness;
		this->program = rhs.program;
		return *this;
	}
	bool		operator<(const Individual &rhs) { return this->fitness < rhs.fitness; }
	bool		operator>(const Individual &rhs) { return !operator<(rhs); }
	
	float		fitness;
	string		program;
};

enum generation_method
{
	GROW,
	FULL
};

typedef float(*Function)();
unordered_map<char, Function>	jumptable;
string::const_iterator			current_node;
vector<float>					args;

float							Next() { return jumptable[*current_node++](); }
float							Arg() { return args[*(current_node - 1) - 1]; }
float							Add() { return Next() + Next(); }
float							Sub() { return Next() - Next(); }
float							Mul() { return Next() * Next(); }
float							Div() { float num = Next(), den = Next(); return (den == 0) ? 1 : num / den; }
float							Exp() { return exp(Next()); }
float							Log() { double arg = Next(); return (arg == 0) ? 0 : log(abs(arg)); }
float							Sin() { return sin(Next()); }
float							Cos() { return cos(Next()); }
float							Pow() { return pow(Next(), Next()); }


Function func_set[] =
{
	//Exp,
	//Sin,
	//Cos, 
	//Log,
	Add,
	Sub,
	Mul,
	Div,
	//Pow,
};

enum function_set
{
	FSET_START = 110,
	//EXP = FSET_START,
	//SIN,
	//COS,
	//LOG,
	ARITY_OF_2 = FSET_START,
	ADD = ARITY_OF_2,
	SUB,
	MUL,
	DIV,
	//POW,
	FSET_END = DIV
};

int						seed;
int						popsize;
int						max_depth;
int						max_length;
int						generations;
int						tournament_size;
float					pm_per_node;
float					pcrossover;
int						num_vars;
int						num_rand_nums;
float					min_rand_num;
float					max_rand_num;
float					pgrow;
bool					seed_w_program;
string					prog_name;
string					fitness_data_filename;
vector<vector<float>>	input_data;

inline int				arity_minus_1(const char &x, const char dummy = '0') { if (x < FSET_START) return -1; else if (x < ARITY_OF_2) return 0; else return 1; }

template<class T>
bool					IsGreater(T &lhs, T &rhs) { return lhs > rhs; }

template<class T>
bool					IsLess(T &lhs, T &rhs) { return lhs < rhs; }

void					SetUp(string filename)
{
	iniReader ini(filename);

	string section = "GP_PARAMETERS";
	seed = ini.Read<int>(section, "seed");
	popsize = ini.Read<int>(section, "popsize");
	max_depth = ini.Read<int>(section, "max_depth");
	max_length = ini.Read<int>(section, "max_length");
	generations = ini.Read<int>(section, "generations");
	tournament_size = ini.Read<int>(section, "tournament_size");
	pm_per_node = ini.Read<float>(section, "mutation_rate_per_node");
	pcrossover = ini.Read<float>(section, "crossover_rate");
	seed_w_program = ini.Read<bool>(section, "seed_with_program");
	prog_name = ini.Read<string>(section, "program_name");

	section = "FITNESS_PARAMETERS";
	num_vars = ini.Read<int>(section, "num_vars");
	num_rand_nums = ini.Read<int>(section, "num_rand_nums");
	min_rand_num = ini.Read<float>(section, "min_rand_num");
	max_rand_num = ini.Read<float>(section, "max_rand_num");
	fitness_data_filename = ini.Read<string>(section, "data_filename");

	args.resize(num_vars + num_rand_nums);

	char i = 0;
	for (; i < num_vars; ++i)
		jumptable.emplace(i + 1, Arg);

	for (i = num_vars; i < args.size(); ++i) {
		args[i] = random(min_rand_num, max_rand_num);
		jumptable.emplace(i + 1, Arg);
	}

	for (i = FSET_START; i <= FSET_END; ++i)
		jumptable.emplace(i, func_set[i - FSET_START]);

	ifstream fitness_data_file(fitness_data_filename);
	assert(fitness_data_file.is_open());

	string entry;

	while (getline(fitness_data_file, entry)) {
		vector<string> datum;
		int pos = 0;
		string delimiter = ",", token;
		while ((pos = entry.find(delimiter)) != std::string::npos) {
			datum.push_back(entry.substr(0, pos));
			entry.erase(0, pos + delimiter.length());
		}
		if (!entry.empty())
			datum.push_back(entry);

		input_data.push_back(vector<float>());

		for (i = 0; i < num_vars; ++i) {
			input_data[input_data.size() - 1].push_back(stof(datum[i]));
		}

		input_data[input_data.size() - 1].push_back(stod(datum[datum.size() - 1]));
	}

	fitness_data_file.close();

	pgrow = args.size() / (args.size() + FSET_END - FSET_START);

	if (seed < 0)
		srand(time(0));
	else
		srand(seed);
}

string					BuildTree(int max_depth, int method)
{
	if (max_depth == 0 || (method == GROW &&  random(0, 1))) {
		return string(1, random<char>(num_vars + num_rand_nums));
	}
	else {
		char primitive = random<char>(FSET_START, FSET_END);
		string expression = string(1, primitive);
		int arity = arity_minus_1(primitive);

		for (int arg = 0; arg < arity + 1; ++arg) {
			expression += BuildTree(max_depth - 1, method);
		}

		return expression;
	}
}

template<class Cnt, class Cmp>
int						Tournament(Cnt &container, int tsize, Cmp comparison)
{
	int p1 = random<int>(container.size() - 1);

	if (tsize == 1)
		return p1;

	int p2 = Tournament(container, tsize - 1, comparison);

	return (comparison(container[p1], container[p2]) == true) ? p1 : p2;
}

typedef string::iterator node;
pair<node, node>		SelectSubtree(string &program)
{
	node end, begin = end = program.begin() + Tournament(program, 3, arity_minus_1); //beginning of the tree

	for (int depth = 1; depth > 0; depth += arity_minus_1(*end++)) {} //finds the end of the subtree;

	return make_pair(begin, end);
}

void					SubtreeXO(Individual &p1, Individual &p2)
{
	pair<node, node> range1 = SelectSubtree(p1.program), range2 = SelectSubtree(p2.program);

	if ((p1.program.size() - (range1.second - range1.first) + (range2.second - range2.first)) < max_length)
		p1.program.replace(range1.first, range1.second, range2.first, range2.second);
}

void					PtMutation(string &program)
{
	for (node n = program.begin(); n != program.end(); ++n)
		if (random<double>() < pm_per_node) {
			if (arity_minus_1(*n) == 1)
				*n = random<char>(ARITY_OF_2, FSET_END);
			else if (arity_minus_1(*n) == 0)
				*n = random<char>(FSET_START, ARITY_OF_2 - 1);
			else
				*n = random<char>(1, num_vars + num_rand_nums);
		}
}

void					Evaluate(Individual &i)
{
	i.fitness = 0;

	for (int fit_case = 0; fit_case < input_data.size(); ++fit_case) {
		for (int var = 0; var < num_vars; ++var) 
			args[var] = input_data[fit_case][var];

		current_node = i.program.begin();

		i.fitness += (abs(Next() - input_data[fit_case][num_vars]));
	}

	i.fitness /= input_data.size();
}



