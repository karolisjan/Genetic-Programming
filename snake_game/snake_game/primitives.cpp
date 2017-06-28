#pragma once

#include "primitives.h"
#include "snake.h"
#include "world.h"
#include "food.h"
#include "utils/custom_console.h"
#include "utils/utils.hpp"

#include <boost/thread/thread.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <iostream>
#include <ostream>


namespace Snake
{
	Snake snake;
}

namespace World
{
	World world;
}

namespace Food
{
	Food food;
}

std::ostream& operator<<(std::ostream& os, World::World &world)
{
	for (int i = 0; i <= world.height + 1; ++i) {
		Console::SetCursorPosition(0, i);
		os << '.';
		Console::SetCursorPosition(world.width * 2 + 2, i);
		os << '.';
	}

	for (int i = 0; i <= world.width + 1; ++i) {
		Console::SetCursorPosition(i * 2, 0);
		os << '.';
		Console::SetCursorPosition(i * 2, world.height + 1);
		os << '.';
	}

	Console::SetCursorPosition(3 + world.width / 2, world.height + 3);
	os << "Points: ";

	return os;
}

std::ostream& operator<<(std::ostream& os, Snake::Snake &snake)
{
	//Draw head
	Console::SetCursorPosition(snake.X[0] * 2, snake.Y[0]);
	os << snake.head;

	//Draw the rest of the body
	for (int segment = 1; segment < snake.segments; ++segment) {
		Console::SetCursorPosition(snake.X[segment] * 2, snake.Y[segment]);
		os << snake.body;
	}

	//Delete old tail
	Console::SetCursorPosition(snake.X[snake.segments] * 2, snake.Y[snake.segments]);
	os << " ";

	//Update scores tally
	Console::SetCursorPosition(11 + World::world.width / 2, World::world.height + 3);
	os << snake.food_eaten;

	boost::this_thread::sleep(boost::posix_time::milliseconds(50));

	return os;
}

std::ostream& operator<<(std::ostream& os, Food::Food &food)
{
	Console::SetCursorPosition(food.x * 2, food.y);
	os << food.shape;
	return os;
}

namespace Primitives
{
	Function tset[] = { Forward, Right, Left }; // terminal set
	Function fset[] = 
	{	IfFoodAhead,
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
		Prog2,
	}; // function set

	void CheckFood()
	{
		if (Food::food.x == Snake::snake.X[0] 
			&& Food::food.y == Snake::snake.Y[0]) {
			++Snake::snake.segments;
			++Snake::snake.food_eaten;
			Snake::snake.ResetSteps();
			SpawnFood();
		}
	}

	void SpawnFood()
	{
		std::unordered_set<std::pair<int, int>, boost::hash<std::pair<int, int>>> fast_lookup;

		for (int s = 0; s < Snake::snake.segments; ++s)
			fast_lookup.emplace(Snake::snake.X[s], Snake::snake.Y[s]);

		do {
			Food::food.x = random<int>(1, World::world.width);
			Food::food.y = random<int>(1, World::world.height);
		} while (fast_lookup.find(std::make_pair(Food::food.x, Food::food.y)) != fast_lookup.end());

		if (display)
			std::cout << Food::food;
	}

	void MakeMove()
	{
		if (Snake::snake.IsAlive() && Snake::snake.steps_left > 0) {
			Snake::snake.Move();
			--Snake::snake.steps_left;
			CheckFood();

			if (display)
				std::cout << Snake::snake;
		}
	}

	void SkipSubtree()
	{
		int not_end_of_subtree = 1;
		while (not_end_of_subtree > 0) {
			not_end_of_subtree += ArityMin1(*current_node++);
		}
	}

	bool IsFoodAhead()
	{
		if (Food::food.x == Snake::snake.X[0]) {
			if (Snake::snake.direction == Snake::GOING_UP
				&& Food::food.y < Snake::snake.Y[0])
				return true;
			if (Snake::snake.direction == Snake::GOING_DOWN 
				&& Food::food.y > Snake::snake.Y[0])
				return true;
		}
		else if (Food::food.y == Snake::snake.Y[0]) {
			if (Snake::snake.direction == Snake::GOING_LEFT 
				&& Food::food.x < Snake::snake.X[0])
				return true;
			if (Snake::snake.direction == Snake::GOING_RIGHT 
				&& Food::food.x > Snake::snake.X[0])
				return true;
		}

		return false;
	}

	int IfFoodAhead()
	{
		if (IsFoodAhead()) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfFoodUp()
	{
		if (Food::food.y <= Snake::snake.Y[0]) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfFoodRight()
	{
		if (Food::food.x >= Snake::snake.X[0]) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfDangerAhead()
	{
		if (IsDangerNear(Snake::snake.direction_x, Snake::snake.direction_y)) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfDangerTwoAhead()
	{
		if (IsDangerNear(Snake::snake.direction_x * 2, Snake::snake.direction_y * 2)) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfDangerLeft()
	{
		int x, y;

		if (Snake::snake.direction == Snake::GOING_RIGHT) {
			x = 0;
			y = -1;
		}
		else if (Snake::snake.direction == Snake::GOING_UP) {
			x = -1;
			y = 0;
		}
		else if (Snake::snake.direction == Snake::GOING_LEFT) {
			x = 0;
			y = 1;
		}
		else if (Snake::snake.direction == Snake::GOING_DOWN) {
			y = 0;
			x = 1;
		}

		if (IsDangerNear(x, y)) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfDangerRight()
	{
		int x, y;

		if (Snake::snake.direction == Snake::GOING_LEFT) {
			x = 0;
			y = -1;
		}
		else if (Snake::snake.direction == Snake::GOING_UP) {
			x = 1;
			y = 0;
		}
		else if (Snake::snake.direction == Snake::GOING_RIGHT) {
			x = 0;
			y = 1;
		}
		else if (Snake::snake.direction == Snake::GOING_DOWN) {
			y = 0;
			x = -1;
		}

		if (IsDangerNear(x, y)) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfMovingUp()
	{
		if (Snake::snake.direction == Snake::GOING_UP) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfMovingDown()
	{
		if (Snake::snake.direction == Snake::GOING_DOWN) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfMovingLeft()
	{
		if (Snake::snake.direction == Snake::GOING_LEFT) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int IfMovingRight()
	{
		if (Snake::snake.direction == Snake::GOING_RIGHT) {
			Next();
			++current_node;
		}
		else {
			//Skip the first subtree
			SkipSubtree();
			Next();
		}

		return 0;
	}

	int Forward()
	{
		//Just keep moving
		MakeMove();
		return 0;
	}

	int Right()
	{
		++Snake::snake; //Turn right
		MakeMove();
		return 0;
	}

	int Left()
	{
		--Snake::snake; //Turn left
		MakeMove();
		return 0;
	}

	bool IsDangerNear(int x, int y)
	{
		for (int s = Snake::snake.segments; s > 0; --s)
			if (Snake::snake.X[s] == Snake::snake.X[0] + x
				&& Snake::snake.Y[s] == Snake::snake.Y[0] + y)
				return true;

		if (Snake::snake.X[0] + x > World::world.width)
			return true;
		if (Snake::snake.X[0] + x < 1)
			return true;
		if (Snake::snake.Y[0] + y > World::world.height)
			return true;
		if (Snake::snake.Y[0] + y < 1)
			return true;

		return false;
	}

	int Prog2()
	{
		Next();
		Next();

		return 0;
	}

	float Evaluate(std::string program)
	{
		int num_total_food = World::world.height * World::world.width - 3;
		float score = 0.0f;

		for (int run = 0; run < 5; ++run) {
			Snake::snake.Spawn();

			if (display) {
				Console::cls();
				std::cout << World::world;
				std::cout << Snake::snake;
			}

			SpawnFood();

			while (Snake::snake.IsAlive() && Snake::snake.steps_left > 0) {
				current_node = program.begin();
				Next();
			}

			score += (num_total_food - Snake::snake.food_eaten);
		}

		return score / 5;
	}

	void Save(std::string program, std::string fullpath)
	{
		std::fstream program_file(fullpath, std::ios::out);

		std::unordered_map<char, std::string> tokens;
		tokens.emplace(FORWARD, "FORWARD");
		tokens.emplace(RIGHT, "RIGHT");
		tokens.emplace(LEFT, "LEFT");
		tokens.emplace(IF_FOOD_AHEAD, "IF-FOOD-AHEAD");
		tokens.emplace(IF_FOOD_UP, "IF_FOOD_UP");
		tokens.emplace(IF_FOOD_RIGHT, "IF_FOOD_RIGHT");
		tokens.emplace(IF_DANGER_AHEAD, "IF_DANGER_AHEAD");
		tokens.emplace(IF_DANGER_TWO_AHEAD, "IF_DANGER_TWO_AHEAD");
		tokens.emplace(IF_DANGER_LEFT, "IF_DANGER_TWO_AHEAD");
		tokens.emplace(IF_DANGER_RIGHT, "IF_DANGER_RIGHT");
		tokens.emplace(IF_MOVING_UP, "IF_MOVING_UP");
		tokens.emplace(IF_MOVING_DOWN, "IF_MOVING_DOWN");
		tokens.emplace(IF_MOVING_LEFT, "IF_MOVING_LEFT");
		tokens.emplace(IF_MOVING_RIGHT, "IF_MOVING_RIGHT");
		tokens.emplace(PROG2, "PROG2");

		for (int i = 0; i < program.size(); ++i)
			program_file << tokens[program[i]] << "\n";

		program_file.close();
	}

	std::string Load(std::string fullpath)
	{
		std::fstream program_file(fullpath);

		std::unordered_map<std::string, char> tokens;
		tokens.emplace("FORWARD", FORWARD);
		tokens.emplace("RIGHT", RIGHT);
		tokens.emplace("LEFT", LEFT);
		tokens.emplace("IF-FOOD-AHEAD", IF_FOOD_AHEAD);
		tokens.emplace("IF_FOOD_UP", IF_FOOD_UP);
		tokens.emplace("IF_FOOD_RIGHT", IF_FOOD_RIGHT);
		tokens.emplace("IF_DANGER_AHEAD", IF_DANGER_AHEAD);
		tokens.emplace("IF_DANGER_TWO_AHEAD", IF_DANGER_TWO_AHEAD);
		tokens.emplace("IF_DANGER_TWO_AHEAD", IF_DANGER_LEFT);
		tokens.emplace("IF_DANGER_RIGHT", IF_DANGER_RIGHT);
		tokens.emplace("IF_MOVING_UP", IF_MOVING_UP);
		tokens.emplace("IF_MOVING_DOWN", IF_MOVING_DOWN);
		tokens.emplace("IF_MOVING_LEFT", IF_MOVING_LEFT);
		tokens.emplace("IF_MOVING_RIGHT", IF_MOVING_RIGHT);
		tokens.emplace("PROG2", PROG2);

		std::string line, program;
		while (true) {
			getline(program_file, line);
			if (program_file.eof())
				break;
			program += tokens[line];
		}

		program_file.close();

		return program;
	}

	void Test(std::string program_path, bool visualise)
	{
		display = visualise;

		Console::Maximise();
		Console::HideCursor();

		Evaluate(Load(program_path));

		Console::SetCursorPosition(0, World::world.height + 4);

		printf("\nDone!");
		getchar();
	}

	int ArityMin1(char x, char dummy)
	{
		return arity[x] - 1;
	}

	std::unordered_map<char, Function> terminals;
	std::unordered_map<char, Function> jumptable;
	std::unordered_map<char, int> arity;
	std::string::const_iterator current_node;

	int Next() 
	{ 
		return jumptable[*current_node++](); 
	}

	int Terminal() 
	{ 
		return terminals[*(current_node - 1)](); 
	}

	void SetUp()
	{
		char i = Primitives::TSET_START;

		for (; i <= Primitives::TSET_END; ++i) {
			terminals.emplace(i, Primitives::tset[i - Primitives::TSET_START]);
			jumptable.emplace(i, Terminal);
		}

		for (i = Primitives::FSET_START; i <= Primitives::FSET_END; ++i)
			jumptable.emplace(i, Primitives::fset[i - Primitives::FSET_START]);

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
	}

	bool display = false;
}
