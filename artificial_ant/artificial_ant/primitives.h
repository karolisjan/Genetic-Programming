#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fstream>


namespace Primitives
{
	typedef int(*Function)();

	extern std::unordered_map<char, Function> terminals;
	extern std::unordered_map<char, Function> jumptable;
	extern std::string::const_iterator current_node;

	int Next();

	int Terminal();

	void SetUp();

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

	Function tset[];

	Function fset[];

<<<<<<< HEAD
	int ArityMin1(char& x, char dummy = '0');
=======
	int ArityMin1(char x, char dummy = '0');
>>>>>>> snake_game_development

	float Evaluate(std::string program);

	void Save(std::string program, std::string fullpath);

	std::string Load(std::string fullpath);

	void Test(std::string program_path, bool visualise);

	extern bool display;
}



