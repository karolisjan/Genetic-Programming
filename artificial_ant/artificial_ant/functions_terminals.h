#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fstream>


namespace Jumptable
{
	typedef int(*Function)();

	extern std::unordered_map<char, Function> terminals;
	extern std::unordered_map<char, Function> jumptable;
	extern std::string::const_iterator current_node;

	int Next();

	int Terminal();

	void SetUp();
}

namespace FunctionsTerminals
{
	int IsFoodAhead();

	int IfFoodAhead();

	int Move();

	int Right();

	int Left();

	int Prog2();

	int Prog3();

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
		FSET_START = TSET_END + 1,
		IF_FOOD_AHEAD = FSET_START,
		PROG2,
		PROG3,
		FSET_END = PROG3
	};

	extern Jumptable::Function tset[];
	extern Jumptable::Function fset[];

	int ArityMin1(char& x, char dummy = '0');

	int Evaluate(std::string program);

	void SaveProgram(std::string program, std::string fullpath);
}



