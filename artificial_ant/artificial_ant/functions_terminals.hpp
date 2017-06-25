#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fstream>

#include "ant.hpp"
#include "world.hpp"

namespace Jumptable
{
	typedef int(*Function)();
	typedef Function TerminalType;

	std::unordered_map<char, TerminalType> terminals;
	std::unordered_map<char, Function> jumptable;
	std::string::const_iterator current_node;

	int Next();
}

namespace FunctionsTerminals
{
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
		FSET_START = TSET_END,
		IF_FOOD_AHEAD,
		PROG2,
		PROG3,
		FSET_END = PROG3
	};

	int IsFoodAhead()
	{
		char x = Ant::ant.x + Ant::ant.facing.first,
			y = Ant::ant.y + Ant::ant.facing.second;

		if (x < 0)
			x = World::world.width;
		else if (x > World::world.width)
			x = 0;

		if (y < 0)
			y = World::world.height;
		else if (y > World::world.height)
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
		fstream program_file(fullpath, ios::out);

		unordered_map<char, std::string> tokens;
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

	Jumptable::Function tset[3] = { Move, Right, Left }; // terminal set
	Jumptable::Function fset[3] = { IfFoodAhead, Prog2, Prog3 }; // function set
}


namespace Jumptable
{
	int Next() { return jumptable[*current_node++](); }

	int Terminal() { return terminals[*(current_node - 1)](); }

	void SetUpJumptable()
	{
		char i = 0;

		for (; i < FunctionsTerminals::TSET_END; ++i) {
			terminals.emplace(i + 1, FunctionsTerminals::tset[i]);
			jumptable.emplace(i + 1, Terminal);
		}

		for (i = FunctionsTerminals::FSET_START; i <= FunctionsTerminals::FSET_END; ++i)
			jumptable.emplace(i, FunctionsTerminals::fset[i - FunctionsTerminals::FSET_START]);
	}
}


