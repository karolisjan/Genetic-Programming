#pragma once

#include "ini_reader.hpp"
#include "helpers.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <stdio.h>
#include <algorithm>
#include <time.h>

using namespace std;

namespace GP {

	static const int					WIDTH = 20;
	static const int					HEIGHT = 11;

	enum directions { GOING_UP, GOING_DOWN, GOING_LEFT, GOING_RIGHT };

	struct Snake
	{
							Snake() : head('x'), body('o'), segments(3), direction_y(0), direction_x(1),
								direction(GOING_RIGHT), steps_left(300), food_eaten(0), ingame(true)
							{
								for (int segment = 0; segment < segments; ++segment) {
									X[segment] = WIDTH / 2 - segment;
									Y[segment] = HEIGHT / 2;
								}
							}

		Snake				&operator++() //for turning right
		{
			direction_y = -direction_y;
			swap(direction_x, direction_y);

			if (direction == GOING_RIGHT)
				direction = GOING_DOWN;
			else if (direction == GOING_DOWN)
				direction = GOING_LEFT;
			else if (direction == GOING_LEFT)
				direction = GOING_UP;
			else if (direction == GOING_UP)
				direction = GOING_RIGHT;

			return *this;
		}
		Snake				&operator--() //for turning left
		{
			direction_x = -direction_x;
			swap(direction_x, direction_y);

			if (direction == GOING_RIGHT)
				direction = GOING_UP;
			else if (direction == GOING_UP)
				direction = GOING_LEFT;
			else if (direction == GOING_LEFT)
				direction = GOING_DOWN;
			else if (direction == GOING_DOWN)
				direction = GOING_RIGHT;

			return *this;
		}

		char				head, body;
		int					segments;
		int					X[WIDTH * HEIGHT], Y[WIDTH * HEIGHT];
		int					direction_x, direction_y, direction;
		int					steps_left, food_eaten;
		bool				ingame;
	} snake;

	inline bool							point_in_snake(int &x, int &y)
	{
		for (int segment = 0; segment < snake.segments; ++segment) {
			if (x == snake.X[segment] && y == snake.Y[segment])
				return true;
		}

		return false;
	}
	inline bool							check_collision()
	{
		for (int segment = snake.segments; segment > 0; --segment)
			if (snake.X[segment] == snake.X[0] && snake.Y[segment] == snake.Y[0])
				return false;

		if (snake.X[0] > WIDTH)
			return false;
		if (snake.X[0] < 1)
			return false;
		if (snake.Y[0] > HEIGHT)
			return false;
		if (snake.Y[0] < 1)
			return false;

		return true;
	}

	struct Food
	{
		inline void						spawn()
		{
			do {
				x = random<int>(1, WIDTH);
				y = random<int>(1, HEIGHT);
			} while (point_in_snake(x, y));

			//consoleOptions::setCursorPosition(x * 2, y);
			//printf("%c", shape);
		}

		int								x, y;
		char							shape = '+';
	} food;

	inline bool							check_food()
	{
		if (food.x == snake.X[0] && food.y == snake.Y[0]) {
			++snake.segments;
			food.spawn();
			return true;
		}

		return false;
	}
	void								MoveSnake()
	{
		if (snake.ingame && snake.steps_left > 0) {
			for (int segment = snake.segments; segment > 0; --segment) {
				int a = snake.direction_x;
				snake.Y[segment] = snake.Y[segment - 1];
				snake.X[segment] = snake.X[segment - 1];
			}

			snake.X[0] += snake.direction_x;
			snake.Y[0] += snake.direction_y;

			snake.ingame = check_collision();

			if (check_food()) {
				++snake.food_eaten;
				snake.steps_left = 300;
				//consoleOptions::setCursorPosition(15 + 9, HEIGHT + 3);
				//cout << snake.food_eaten;
			}

			--snake.steps_left;

			//consoleOptions::setCursorPosition(snake.X[0] * 2, snake.Y[0]);
			//printf("%c", snake.head);

			//for (int segment = 1; segment < snake.segments; ++segment) {
			//	consoleOptions::setCursorPosition(snake.X[segment] * 2, snake.Y[segment]);
			//	printf("%c", snake.body);
			//}

			//consoleOptions::setCursorPosition(snake.X[snake.segments] * 2, snake.Y[snake.segments]);
			//printf(" ");

			//Sleep(50);
		}
	}	
	bool								IsFoodAhead()
	{ 
		if (food.x == snake.X[0]) {
			if (snake.direction == GOING_UP && food.y < snake.Y[0])
				return true;
			if (snake.direction == GOING_DOWN && food.y > snake.Y[0])
				return true;
		}
		else if (food.y == snake.Y[0]) {
			if (snake.direction == GOING_LEFT && food.x < snake.X[0])
				return true;
			if (snake.direction == GOING_RIGHT && food.x > snake.X[0])
				return true;
		}

		return false;
	}
	bool								IsThereDanger(int x, int y)
	{
		for (int segment = snake.segments; segment > 0; --segment)
			if (snake.X[segment] == snake.X[0] + x && snake.Y[segment] == snake.Y[0] + y)
				return true;

		if (snake.X[0] + x > WIDTH)
			return true;
		if (snake.X[0] + x < 1)
			return true;
		if (snake.Y[0] + y > HEIGHT)
			return true;
		if (snake.Y[0] + y < 1)
			return true;

		return false;
	}

	struct Individual
	{
					Individual() : fitness(INT_MAX) {}
		
		void		operator=(Individual &rhs)
		{
			this->fitness = rhs.fitness;
			this->program = rhs.program;
		}
		bool		operator==(Individual &rhs)
		{
			if (this->fitness == rhs.fitness && this->program == rhs.program)
				return true;
			else
				return false;
		}
		bool		operator!=(Individual &rhs)
		{
			return !operator==(rhs);
		}
		bool		operator<(const Individual &rhs) { return this->fitness < rhs.fitness; }
		bool		operator>(const Individual &rhs) { return !operator<(rhs); }

		float		fitness;
		string		program;
	};

	typedef int ReturnType;
	typedef ReturnType(*Function)();
	typedef Function TerminalType;

	unordered_map<char, TerminalType>	terminals;
	unordered_map<char, Function>		jumptable;
	unordered_map<char, char>			arity;

	string::const_iterator				current_node;

	inline int							arity_minus_1(const char &x, const char dummy = '0') { return arity[x] - 1; }

	ReturnType							Next() { return jumptable[*current_node++](); }
	ReturnType							Terminal() { return terminals[*(current_node - 1)](); }
	
	ReturnType							IfFoodAhead() 
	{ 
		if (IsFoodAhead()) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}

		return 0;
	}
	ReturnType							IfFoodUp()
	{
		if (food.y <= snake.Y[0]) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}

		return 0;
	}
	ReturnType							IfFoodRight()
	{
		if (food.x >= snake.X[0]) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}

		return 0;
	}
	
	ReturnType							IfDangerAhead()
	{
		if (IsThereDanger(snake.direction_x, snake.direction_y)) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}

		return 0;
	}
	ReturnType							IfDangerTwoAhead()
	{
		if (IsThereDanger(snake.direction_x * 2, snake.direction_y * 2)) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			//++current_node;
			Next();
		}

		return 0;
	}
	ReturnType							IfDangerLeft()
	{
		int x, y;

		if (snake.direction == GOING_RIGHT) {
			x = 0;
			y = -1;
		}
		else if (snake.direction == GOING_UP) {
			x = -1;
			y = 0;
		}
		else if (snake.direction == GOING_LEFT) {
			x = 0;
			y = 1;
		}
		else if (snake.direction == GOING_DOWN) {
			y = 0;
			x = 1;
		}

		if (IsThereDanger(x, y)) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}

		return 0;
	}
	ReturnType							IfDangerRight()
	{
		int x, y;

		if (snake.direction == GOING_LEFT) {
			x = 0;
			y = -1;
		}
		else if (snake.direction == GOING_UP) {
			x = 1;
			y = 0;
		}
		else if (snake.direction == GOING_RIGHT) {
			x = 0;
			y = 1;
		}
		else if (snake.direction == GOING_DOWN) {
			y = 0;
			x = -1;
		}

		if (IsThereDanger(x, y)) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}

		return 0;
	}
	
	ReturnType							IfMovingUp()
	{
		if (snake.direction == GOING_UP) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}

		return 0;
	}
	ReturnType							IfMovingDown()
	{
		if (snake.direction == GOING_DOWN) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}

		return 0;
	}
	ReturnType							IfMovingLeft()
	{
		if (snake.direction == GOING_LEFT) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}

		return 0;
	}
	ReturnType							IfMovingRight()
	{
		if (snake.direction == GOING_RIGHT) {
			Next();
			++current_node;
		}
		else {
			for (int children = 1; children > 0; children += arity_minus_1(*current_node++)) {}
			Next();
		}
			
		return 0;
	}

	ReturnType							Forward()
	{
		MoveSnake();
		return 0;
	}
	ReturnType							Right()
	{
		++snake;
		MoveSnake();
		return 0; 
	}
	ReturnType							Left() 
	{ 
		--snake;
		MoveSnake();
		return 0;
	}
	
	ReturnType							Prog2()
	{ 
		Next(); 
		Next(); 
		return 0;
	}
	ReturnType							Prog3()
	{ 
		Next(); 
		Next(); 
		Next(); 
		return 0;
	}
	
	enum terminal_set
	{
		TSET_START = 1,
		FORWARD = TSET_START,
		RIGHT,
		LEFT,
		TSET_END = LEFT
	};

	Function							tset[TSET_END - TSET_START + 1] = { Forward, Right, Left };

	enum function_set
	{
		FSET_START = 110,
		IF_FOOD_AHEAD = FSET_START,
		IF_FOOD_UP,
		IF_FOOD_RIGHT,
		IF_DANGER_AHEAD,
		IF_DANGER_TWO_AHEAD,
		IF_DANGER_LEFT,
		IF_DANGER_RIGHT,
		IF_MOVING_UP,
		IF_MOVING_DOWN,
		IF_MOVING_LEFT,
		IF_MOVING_RIGHT,
		PROG2,
		PROG3,
		FSET_END = PROG2
	};
	
	Function							fset[FSET_END - FSET_START + 1] = 
	{ 
		IfFoodAhead, 
		IfFoodUp,
		IfFoodRight,
		IfDangerAhead, 
		IfDangerTwoAhead, 
		IfDangerLeft, 
		IfDangerRight,
		IfMovingUp,
		IfMovingDown,
		IfMovingLeft,
		IfMovingRight,
		Prog2
		//Prog3 
	};

	int									seed;
	int									popsize;
	int									max_depth;
	int									max_length;
	int									generations;
	int									tournament_size;
	float								pm_per_node = 0.05;
	float								pcrossover;
	float								psubmutation = 0.10;
	float								pgrow;
	int									tree_generation_method;

	enum generation_method { GROW, FULL };

	template<class T> bool				IsGreater(T &lhs, T &rhs) { return lhs > rhs; }

	template<class T> bool				IsLess(T &lhs, T &rhs) { return lhs < rhs; }

	void								SetUp(string filename)
	{
		iniReader ini(filename);

		string section = "GP_PARAMETERS";
		seed = ini.Read<int>(section, "seed");
		popsize = ini.Read<int>(section, "popsize");
		max_depth = ini.Read<int>(section, "max_depth");
		max_length = ini.Read<int>(section, "max_length");
		generations = ini.Read<int>(section, "generations");
		tournament_size = ini.Read<int>(section, "tournament_size");
		pcrossover = ini.Read<float>(section, "crossover_rate");
		tree_generation_method = ini.Read<int>(section, "generation_method");

		for (int i = TSET_START; i <= TSET_END; ++i) {
			terminals.emplace(i, tset[i - TSET_START]);
			jumptable.emplace(i, Terminal);
		}

		for (int i = FSET_START; i <= FSET_END; ++i) 
			jumptable.emplace(i, fset[i - FSET_START]);

		arity.emplace(FORWARD, 0);
		arity.emplace(LEFT, 0);
		arity.emplace(RIGHT, 0);
		arity.emplace(IF_FOOD_AHEAD, 2);
		arity.emplace(IF_FOOD_UP, 2);
		arity.emplace(IF_FOOD_RIGHT, 2);
		arity.emplace(IF_DANGER_AHEAD, 2);
		arity.emplace(IF_DANGER_TWO_AHEAD, 2);
		arity.emplace(IF_DANGER_LEFT, 2);
		arity.emplace(IF_DANGER_RIGHT, 2);
		arity.emplace(IF_MOVING_UP, 2);
		arity.emplace(IF_MOVING_DOWN, 2);
		arity.emplace(IF_MOVING_LEFT, 2);
		arity.emplace(IF_MOVING_RIGHT, 2);
		arity.emplace(PROG2, 2);
		arity.emplace(PROG3, 3);

		pgrow = (float)(TSET_END - TSET_START) / (TSET_END - TSET_START + FSET_END - FSET_START);

		if (seed < 0)
			srand(time(0));
		else
			srand(seed);
	}

	string								BuildTree(int max_depth, int method)
	{
		if (max_depth == 0 || (method == GROW && random<float>() < pgrow)) {
			return string(1, random<char>(TSET_START, TSET_END));
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

	template<class Cnt, class Cmp> int	Tournament(Cnt &container, int tsize, Cmp comparison)
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

	typedef string::iterator node;
	pair<node, node>					SelectSubtree(string &program)
	{
		node end, begin = end = program.begin() + Tournament(program, 3, arity_minus_1); //beginning of the tree

		for (int children = 1; children > 0; children += arity_minus_1(*end++)) {} //finds the end of the subtree, i.e. when children = 0;

		return make_pair(begin, end);
	}

	void								SubtreeXO(string &p1, string &p2)
	{
		pair<node, node> range1 = SelectSubtree(p1), range2 = SelectSubtree(p2);

		if ((p1.size() - (range1.second - range1.first) + (range2.second - range2.first)) < max_length)
			p1.replace(range1.first, range1.second, range2.first, range2.second);
	}

	void								SubtreeMutation(string &program)
	{
		string new_program = BuildTree(random(1, max_depth), tree_generation_method);

		SubtreeXO(program, new_program);
	}

	void								PtMutation(string &program)
	{
		for (node n = program.begin(); n != program.end(); ++n)
			if (random<float>() < pm_per_node)
				*n = arity_minus_1(*n) == 1 ? random<char>(FSET_START, FSET_END) : random<char>(TSET_START, TSET_END);
	}

	void								Evaluate(Individual &i)
	{
		i.fitness = 0;

		for (int run = 0; run < 5; ++run) {
			snake = Snake();
			food.spawn();

			while (snake.ingame && snake.steps_left > 0) {
				current_node = i.program.begin();
				Next();
			}

			i.fitness += (211 - snake.food_eaten);
		}

		i.fitness /= 5;
	}
}