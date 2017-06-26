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

	int Move();

	int IsFoodAhead();

	int IfFoodAhead();

	bool IsDangerNear();

	int IfFoodAhead();

	int IfFoodUp();

	int IfFoodRight();

	int IfDangerAhead();

	int IfDangerTwoAhead();

	int IfDangerLeft();

	int IfDangerRight();

	int IfMovingUp();

	int IfMovingDown();

	int IfMovingLeft();

	int IfMovingRight();

	int Forward();

	int Right();

	int Left();

	int Prog2();

	int Prog3();

	enum terminal_set
	{
		TSET_START = 1,
		FORWARD = TSET_START,
		RIGHT,
		LEFT,
		TSET_END = LEFT
	};

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

	Function tset[];

	Function fset[];

	int ArityMin1(char& x, char dummy = '0');

	float Evaluate(std::string program);

	void Save(std::string program, std::string fullpath);

	std::string Load(std::string fullpath);

	void Test(std::string program_path, bool visualise);

	extern bool display;
}



