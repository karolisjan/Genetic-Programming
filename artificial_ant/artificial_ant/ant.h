#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <ostream>


namespace Ant 
{
	struct Ant
	{
		Ant(int x = 0, int y = 0, std::pair<int, int> facing = std::make_pair(1, 0), int num_steps = 400) :
			x(x), y(y), facing(facing), steps_left(num_steps), food_eaten(0)
		{
			Draw();
		}

		void SetUp(int x = 0, int y = 0, std::pair<int, int> facing = std::make_pair(1, 0), int num_steps = 400)
		{
			this->x_cpy = x;
			this->y_cpy = y;
			this->facing_cpy = facing;
			this->steps_left_cpy = num_steps;
		}

		void Spawn()
		{
			food_eaten = 0;
			x = x_cpy;
			y = y_cpy;
			facing = facing_cpy;
			steps_left = steps_left_cpy;
		}

		Ant &operator++() //for turning right, clockwise
		{
			--steps_left -= 1;
			facing.second = -facing.second;
			std::swap(facing.second, facing.first);

			return *this;
		}

		Ant &operator--() //for turning left, counter-clockwise
		{
			--steps_left;
			facing.first = -facing.first;
			std::swap(facing.second, facing.first);

			return *this;
		}

		bool operator==(const std::pair<char, char> &other)
		{
			if (facing.first == other.first && facing.second == other.second)
				return true;
			return false;
		}

		char Draw()
		{
			if (facing == std::make_pair(1, 0))
				ant_face = '>';
			else if (facing == std::make_pair(0, -1))
				ant_face = '^';
			else if (facing == std::make_pair(-1, 0))
				ant_face = '<';
			else if (facing == std::make_pair(0, 1))
				ant_face = 'v';

			return ant_face;
		}

		int x;
		int y;
		std::pair<int, int> facing;
		int food_eaten;
		int steps_left;
		char ant_face;

private:		
		int x_cpy;
		int y_cpy;
		std::pair<int, int> facing_cpy;
		int steps_left_cpy;
	};

	extern Ant ant;
}