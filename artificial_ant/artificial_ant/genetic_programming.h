#pragma once

#include "ini_reader.hpp"
#include "custom_console.hpp"
#include "helpers.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <stdio.h>
#include <algorithm>
#include <time.h>

ostream &operator<<(ostream &os, std::vector<std::vector<char>> &santa_fe_ant_trail)
{
	for (char x = 0; x < santa_fe_ant_trail.size(); ++x) {
		for (char y = 0; y < santa_fe_ant_trail[x].size(); ++y) {
			os << santa_fe_ant_trail[x][y] << " ";
		}

		os << "\n";
	}

	return os;
}

namespace GP 
{
	typedef int(*Function)();
	typedef Function TerminalType;

	unordered_map<char, TerminalType> terminals;
	unordered_map<char, Function> jumptable;
	typedef std::string::iterator Node;
	typedef std::string::const_iterator ConstNode;
	ConstNode current_node;

	int seed;
	int generations, popsize;
	int	max_depth, max_length, num_terminals;
	int	tournament_size;
	float pm_per_node, pcrossover, pgrow;
	int tree_generation_method;
	int num_food_pieces, trail_width, trail_height;
	bool save_best_program;
	std::string trail_filepath, save_as, program_to_test;
	std::vector<std::vector<char>> trail, trail_copy;

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

	struct Ant 
	{
		Ant() : x(0), y(0), facing(make_pair(1, 0)), food_eaten(0), steps_left(400), ant_face('>') {}

		Ant &operator++() //for turning right
		{
			--steps_left -= 1;
			facing.second = -facing.second;
			std::swap(facing.second, facing.first);

			return *this;
		}

		Ant &operator--() //for turning left
		{
			--steps_left;
			facing.first = -facing.first;
			std::swap(facing.second, facing.first);

			return *this;
		}

		bool operator==(const pair<char, char> &other)
		{
			if (facing.first == other.first && facing.second == other.second)
				return true;
			else
				return false;
		}

		void paint_ant()
		{
			pair<char, char> direction;
			if (facing == (direction = make_pair(1, 0)))
				ant_face = '>';
			else if (facing == (direction = make_pair(0, -1)))
				ant_face = '^';
			else if (facing == (direction = make_pair(-1, 0)))
				ant_face = '<';
			else if (facing == (direction = make_pair(0, 1)))
				ant_face = 'v';

			trail_copy[y][x] = ant_face;
					
			Console::SetCursorPosition(ant.x * 2, ant.y);
			cout << ant_face;
			Sleep(100);
		}
		
		char x, y;
		pair<char, char> facing;
		char food_eaten;
		int steps_left;
		char ant_face;

	} ant;

	int Next() { return jumptable[*current_node++](); }

	int Terminal() { return terminals[*(current_node - 1)](); }
	
	int IsFoodAhead()
	{ 
		char x = ant.x + ant.facing.first, y = ant.y + ant.facing.second;
	
		if (x < 0)
			x = 31;
		else if (x > 31)
			x = 0;
	
		if (y < 0)
			y = 31;
		else if (y > 31)
			y = 0;
	
		return (trail_copy[y][x] == 'x') ? 1 : 0;
	}
	
	int IfFoodAhead() 
	{ 
		if (IsFoodAhead()) {
			Next();
			++current_node;
		}
		else {
			++current_node;
			Next();
		}

		return 0;
	}

	int Move() 
	{ 
		if (ant.steps_left-- > 0) {
	
			ant.x += ant.facing.first;
			ant.y += ant.facing.second;
	
			if (ant.x < 0)
				ant.x = trail_width - 1;
			else if (ant.x > trail_width - 1)
				ant.x = 0;
	
			if (ant.y < 0)
				ant.y = trail_height - 1;
			else if (ant.y > trail_height - 1)
				ant.y = 0;
	
			if (trail_copy[ant.y][ant.x] == 'x') {
				trail_copy[ant.y][ant.x] = '.';
				ant.food_eaten += 1;
			}
	
			#if TESTING	
			if (ant.food_eaten < num_food_pieces)
				ant.paint_ant();
			#endif
		}

		return 0; 
	}

	int Right()
	{ 
		if (ant.steps_left > 0)  
			++ant;  

		#if TESTING	
		if (ant.food_eaten < num_food_pieces)
			ant.paint_ant();
		#endif

		return 0; 
	}

	int Left() 
	{ 
		if (ant.steps_left > 0) 
			--ant;  

		#if TESTING	
		if (ant.food_eaten < num_food_pieces)
			ant.paint_ant();
		#endif

		return 0;
	}

	int Prog2()
	{ 
		Next(); 
		Next(); 

		return 0;
	}

	int Prog3()
	{ 
		Next(); 
		Next(); 
		Next(); 

		return 0;
	}
	
	enum terminal_set
	{
		TSET_START = 1,
		MOVE = TSET_START,
		RIGHT,
		LEFT,
		TSET_END = LEFT
	};

	enum function_set
	{
		FSET_START = 110,
		IF_FOOD_AHEAD = FSET_START,
		PROG2,
		PROG3,
		FSET_END = PROG3
	};
	
	Function tset[3] = { Move, Right, Left }; // terminal set
	Function fset[3] = { IfFoodAhead, Prog2, Prog3 }; // function set

	inline int ArityMin1(char &x, char dummy = '0') 
	{ 
		if (x < FSET_START)
			return -1;
		else if (x >= FSET_START && x < FSET_END)
			return 1;
		else if (x == PROG3)
			return 2;
	}

	template<class T> 
	bool IsGreater(T &lhs, T &rhs) { return lhs > rhs; }

	template<class T> 
	bool IsLess(T &lhs, T &rhs) { return lhs < rhs; }

	void LoadTrail()
	{
		ifstream fitness_data_file(trail_filepath);

		std::string entry;

		while (getline(fitness_data_file, entry)) {
			trail.push_back(std::vector<char>(entry.size(), 0));

			for (int i = 0; i < entry.size(); ++i)
				trail.back()[i] = entry[i];
		}

		fitness_data_file.close();

		trail_width = entry.size();
		trail_height = trail.size();
	}

	void ResetTrail()
	{
		trail_copy = trail;
	}

	void SetUp(std::string filename)
	{
		IniReader ini(filename);

		std::string section = "GENERAL_OPTIONS";
		save_best_program = ini.Read<bool>(section, "save_best_program");
		save_as = ini.Read<std::string>(section, "save_as");

		section = "GP_PARAMETERS";
		seed = ini.Read<int>(section, "seed");
		popsize = ini.Read<int>(section, "popsize");
		max_depth = ini.Read<int>(section, "max_depth");
		max_length = ini.Read<int>(section, "max_length");
		generations = ini.Read<int>(section, "generations");
		tournament_size = ini.Read<int>(section, "tournament_size");
		pm_per_node = ini.Read<float>(section, "mutation_rate_per_node");
		pcrossover = ini.Read<float>(section, "crossover_rate");
		tree_generation_method = ini.Read<int>(section, "tree_generation_method");
		
		section = "TRAIL_OPTIONS";
		trail_filepath = ini.Read<std::string>(section, "trail_filepath ");
		num_food_pieces = ini.Read<int>(section, "num_food_pieces");

		section = "TESTING";
		program_to_test = ini.Read<std::string>(section, "program_to_test");

		num_terminals = 3;

		char i = 0;

		for (; i < num_terminals; ++i) {
			terminals.emplace(i + 1, tset[i]);
			jumptable.emplace(i + 1, Terminal);
		}

		for (i = FSET_START; i <= FSET_END; ++i)
			jumptable.emplace(i, fset[i - FSET_START]);

		pgrow = (float)num_terminals / (num_terminals + FSET_END - FSET_START);

		if (seed < 0)
			srand(time(0));
		else
			srand(seed);

		LoadTrail();
	}

	void SaveProgram(std::string& program)
	{
		fstream results_file(save_as, ios::out);

		unordered_map<char, std::string> tokens;
		tokens.emplace(MOVE, "MOVE");
		tokens.emplace(RIGHT, "RIGHT");
		tokens.emplace(LEFT, "LEFT");
		tokens.emplace(IF_FOOD_AHEAD, "IF-FOOD-AHEAD");
		tokens.emplace(PROG2, "PROG2");
		tokens.emplace(PROG3, "PROG3");

		for (int i = 0; i < program.size(); ++i)
			results_file << tokens[program[i]] << "\n";

		results_file.close();
	}

	std::string BuildTree(int max_depth, int method)
	{
		if (max_depth == 0 || (method == GROW && random<float>() < pgrow)) {
			return std::string(1, random<char>(1, num_terminals));
		}
		else {
			char primitive = random<char>(FSET_START, FSET_END);
			
			switch (ArityMin1(primitive)) {
				case 1:
					return std::string(1, primitive) + BuildTree(max_depth - 1, method) + BuildTree(max_depth - 1, method);
				case 2:
					return std::string(1, primitive) + BuildTree(max_depth - 1, method) + BuildTree(max_depth - 1, method) + BuildTree(max_depth - 1, method);
			}
		}
	}

	template<class Cnt, class Cmp> 
	int Tournament(Cnt &container, int tsize, Cmp comparison)
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

	pair<Node, Node> SelectSubtree(std::string &program)
	{
		Node end, begin = end = program.begin() + Tournament(program, 3, ArityMin1); // beginning of the tree

		int children = 1;
		for (; children > 0; children += ArityMin1(*end++)) {} //finds the end of the subtree, i.e. when children = 0;

		//while (children > 0) children += ArityMin1(*end++);

		return make_pair(begin, end);
	}

	void SubtreeXO(std::string &p1, std::string &p2)
	{
		pair<Node, Node> range1 = SelectSubtree(p1), range2 = SelectSubtree(p2);

		if ((p1.size() - (range1.second - range1.first) + (range2.second - range2.first)) < max_length)
			p1.replace(range1.first, range1.second, range2.first, range2.second);
	}

	void SubtreeMutation(std::string &program)
	{
		std::string new_program = BuildTree(random(1, max_depth), tree_generation_method);

		SubtreeXO(program, new_program);
	}

	void PtMutation(std::string &program)
	{
		for (Node n = program.begin(); n != program.end(); ++n)
			if (random<float>() < pm_per_node)
				*n = ArityMin1(*n) == 1 ? random<char>(FSET_START, FSET_END) : random<char>(1, num_terminals);
	}

	int Evaluate(Individual &i)
	{
		ant = Ant();
		ResetTrail();

		#if TESTING
		std::cout << trail;
		#endif
	
		while (ant.steps_left > 0) {
			current_node = i.program.begin();
			Next();
		}
	
		i.fitness = num_food_pieces - ant.food_eaten;

		#if TESTING
		Console::SetCursorPosition(0, trail_height + 2);
		#endif

		return i.fitness;
	}
}