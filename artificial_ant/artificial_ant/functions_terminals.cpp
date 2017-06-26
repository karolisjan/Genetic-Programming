#pragma once

#include "functions_terminals.h"
#include "ant.h"
#include "world.h"

#include <exception>

namespace Ant
{
	Ant ant;
}

namespace World
{
	World world;
}

namespace FunctionsTerminals
{
	Jumptable::Function tset[] = { Move, Right, Left }; // terminal set
	Jumptable::Function fset[] = { IfFoodAhead, Prog2, Prog3 }; // function set

	int FunctionsTerminals::IsFoodAhead()
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
			Jumptable::Next();
			++Jumptable::current_node;
		}
		else {
			++Jumptable::current_node;
			Jumptable::Next();
		}

		return 0;
	}

	int Move()
	{
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
				Ant::ant.food_eaten += 1;
			}

			//if (ant.food_eaten < num_food_pieces)
			//	ant.paint_ant();
		}

		return 0;
	}

	int Right()
	{
		if (Ant::ant.steps_left > 0)
			++Ant::ant;

		//if (Ant::ant.food_eaten < World::world.num_food_pieces)
		//	Ant::ant.Draw();

		return 0;
	}

	int Left()
	{
		if (Ant::ant.steps_left > 0)
			--Ant::ant;

		//if (ant.food_eaten < num_food_pieces)
		//	ant.paint_ant();

		return 0;
	}

	int Prog2()
	{
		Jumptable::Next();
		Jumptable::Next();

		return 0;
	}

	int Prog3()
	{
		Jumptable::Next();
		Jumptable::Next();
		Jumptable::Next();

		return 0;
	}

	int Evaluate(std::string program)
	{
		World::world.Refresh();
		Ant::ant.Spawn();

		while (Ant::ant.steps_left > 0) {
			Jumptable::current_node = program.begin();
			Jumptable::Next();
		}

		return World::world.num_food_pieces - Ant::ant.food_eaten;
	}

	void SaveProgram(std::string program, std::string fullpath)
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

	int ArityMin1(char& x, char dummy)
	{
		if (x < FunctionsTerminals::FSET_START || x == IF_FOOD_AHEAD)
			return -1;

		if (x == PROG3)
			return 2;

		return 1;
	}
}

namespace Jumptable
{
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
		char i = 1;

		for (; i <= FunctionsTerminals::TSET_END; ++i) {
			terminals.emplace(i, FunctionsTerminals::tset[i]);
			jumptable.emplace(i, Terminal);
		}

		for (i = FunctionsTerminals::FSET_START; i <= FunctionsTerminals::FSET_END; ++i)
			jumptable.emplace(i, FunctionsTerminals::fset[i - FunctionsTerminals::FSET_START]);
	}
}
