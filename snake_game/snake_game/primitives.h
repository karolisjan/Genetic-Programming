#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fstream>


namespace Primitives
{
	/*List of helpers*/

	void CheckFood();

	void SpawnFood();

	void MakeMove();

	void SkipSubtree();

	bool IsFoodAhead();

	bool IsDangerNear(int x, int y);

	/*End of helpers list*/

	/*Start of the primitives list (all return 0 and invoke Next())*/

	int IfFoodAhead();

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
		FSET_END = PROG2
	}; 

	/*End of the primitives list*/

	typedef int(*Function)();

	extern std::unordered_map<char, Function> terminals;
	extern std::unordered_map<char, Function> jumptable;
	extern std::unordered_map<char, int> arity;
	extern std::string::const_iterator current_node;

	int Next();

	int Terminal();

	void SetUp();

	Function tset[];

	Function fset[];

	int ArityMin1(char x, char dummy = '0');

	float Evaluate(std::string program);

	void Save(std::string program, std::string fullpath);

	std::string Load(std::string fullpath);

	void Test(std::string program_path, bool visualise);

	extern bool display;
}



