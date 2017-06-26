#pragma once

#include "primitives.h"
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

	int Evaluate(std::string program)
	{
		World::world.Refresh();
		Ant::ant.Spawn();

		int i = 0;
		std::string broken = { IF_FOOD_AHEAD, PROG3, PROG3,			LEFT,
																	MOVE, 
																	MOVE, 
													 IF_FOOD_AHEAD, MOVE,
																	MOVE,
													 IF_FOOD_AHEAD, LEFT,
																	MOVE,
											  PROG3, PROG2,			MOVE,
																	LEFT,
													 PROG2,			LEFT,
																	MOVE,
													 PROG2,			LEFT,
																	LEFT};

		while (Ant::ant.steps_left > 0) {
			current_node = program.begin();
			Next();
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
}
