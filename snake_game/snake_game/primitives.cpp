#pragma once

#include "primitives.h"
#include "snake.h"
#include "world.h"
#include "utils/custom_console.h"

#include <boost/thread/thread.hpp>
#include <iostream>
#include <ostream>


namespace Snake
{
	Snake ant;
}

namespace World
{
	World world;
}

std::ostream &operator<<(std::ostream &os, World::World& world)
{
	for (char x = 0; x < world.trail.size(); ++x) {
		for (char y = 0; y < world.trail[x].size(); ++y) {
			os << world.trail[x][y] << " ";
		}

		os << "\n";
	}

	return os;
}

std::ostream &operator<<(std::ostream &os, Ant::Ant& ant)
{
	Console::SetCursorPosition(ant.x * 2, ant.y);
	os << ant.Draw();
	boost::this_thread::sleep(boost::posix_time::milliseconds(150));

	return os;
}

namespace Primitives
{
	Function tset[] = { Move, Right, Left }; // terminal set
	Function fset[] = { IfFoodAhead, Prog2, Prog3 }; // function set

	int Primitives::IsFoodAhead()
	{
		char x = Ant::ant.x + Ant::ant.facing.first,
			 y = Ant::ant.y + Ant::ant.facing.second;

		if (x < 0)
			x = World::world.width - 1;
		else if (x > World::world.width - 1)
			x = 0;

		if (y < 0)
			y = World::world.height - 1;
		else if (y > World::world.height - 1)
			y = 0;

		return World::world.HasFood(x, y);
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
		if (display) {
			Console::SetCursorPosition(Ant::ant.x * 2, Ant::ant.y);
			std::cout << ".";
		}

		if (Ant::ant.steps_left-- > 0) {

			Ant::ant.x += Ant::ant.facing.first;
			Ant::ant.y += Ant::ant.facing.second;

			if (Ant::ant.x < 0)
				Ant::ant.x = World::world.width - 1;
			else if (Ant::ant.x > World::world.width - 1)
				Ant::ant.x = 0;

			if (Ant::ant.y < 0)
				Ant::ant.y = World::world.height - 1;
			else if (Ant::ant.y > World::world.height - 1)
				Ant::ant.y = 0;

			if (World::world.HasFood(Ant::ant.x, Ant::ant.y)) {
				World::world.RemovePiece(Ant::ant.x, Ant::ant.y);
				Ant::ant.food_eaten += 1;
			}

			if (display && Ant::ant.food_eaten < World::world.num_food_pieces)
				std::cout << Ant::ant;
		}

		return 0;
	}

	int Right()
	{
		if (Ant::ant.steps_left > 0)
			++Ant::ant;

		if (display && Ant::ant.food_eaten < World::world.num_food_pieces)
			std::cout << Ant::ant;

		return 0;
	}

	int Left()
	{
		if (Ant::ant.steps_left > 0)
			--Ant::ant;

		if (display && Ant::ant.food_eaten < World::world.num_food_pieces)
			std::cout << Ant::ant;

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

	float Evaluate(std::string program)
	{
		World::world.Refresh();
		Ant::ant.Spawn();

		int best_steps = Ant::ant.steps_left;
		int best_num_eaten = World::world.num_food_pieces;
		
		auto normalise_steps = [&best_steps](int steps_left)->float {return (float)steps_left / best_steps; };
		auto normalise_num_eaten = [&best_num_eaten](int num_pieces_eaten)->float {return (float)num_pieces_eaten / best_num_eaten; };

		while (Ant::ant.steps_left > 0) {
			current_node = program.begin();
			Next();
		}

		float food_weight = 0.7, steps_weight = 0.3;

		return 1 - food_weight * normalise_num_eaten(Ant::ant.food_eaten) - steps_weight * normalise_steps(Ant::ant.steps_left);
	}

	void Save(std::string program, std::string fullpath)
	{
		std::fstream program_file(fullpath, std::ios::out);

		std::unordered_map<char, std::string> tokens;
		tokens.emplace(MOVE, "MOVE");
		tokens.emplace(RIGHT, "RIGHT");
		tokens.emplace(LEFT, "LEFT");
		tokens.emplace(IF_FOOD_AHEAD, "IF-FOOD-AHEAD");
		tokens.emplace(PROG2, "PROG2");
		tokens.emplace(PROG3, "PROG3");

		for (int i = 0; i < program.size(); ++i)
			program_file << tokens[program[i]] << "\n";

		program_file.close();
	}

	std::string Load(std::string fullpath)
	{
		std::fstream program_file(fullpath);

		std::unordered_map<std::string, char> reverse_tokens;
		reverse_tokens.emplace("MOVE", MOVE);
		reverse_tokens.emplace("RIGHT", RIGHT);
		reverse_tokens.emplace("LEFT", LEFT);
		reverse_tokens.emplace("IF-FOOD-AHEAD", IF_FOOD_AHEAD);
		reverse_tokens.emplace("PROG2", PROG2);
		reverse_tokens.emplace("PROG3", PROG3);

		std::string line, program;
		while (true) {
			getline(program_file, line);
			if (program_file.eof())
				break;
			program += reverse_tokens[line];
		}

		program_file.close();

		return program;
	}

	void Test(std::string program_path, bool visualise)
	{
		display = visualise;

		Console::Maximise();
		Console::HideCursor();

		std::cout << World::world;

		Evaluate(Load(program_path));

		Console::SetCursorPosition(0, World::world.height);

		printf("\nNo. pieces available: %d", World::world.num_food_pieces);
		printf("\nNo. pieces eaten: %d", Ant::ant.food_eaten);
		printf("\nDone!");
		getchar();
	}

	int ArityMin1(char& x, char dummy)
	{
		if (x < Primitives::FSET_START)
			return -1;

		if (x == PROG3)
			return 2;

		return 1;
	}

	std::unordered_map<char, Function> terminals;
	std::unordered_map<char, Function> jumptable;
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
	}

	bool display = false;
}
