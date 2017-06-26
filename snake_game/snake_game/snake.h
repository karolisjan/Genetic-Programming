#pragma once

#include <algorithm>
#include <vector>


namespace Snake
{
	enum directions { GOING_UP, GOING_DOWN, GOING_LEFT, GOING_RIGHT };

	struct Snake
	{
		Snake() : head('x'), body('o'), segments(3), direction_y(0), direction_x(1), direction(GOING_RIGHT)
		{

		}

		void SetUp(int max_steps, int world_width, int world_height, 
			int segments = 3, int direction_x = 1, int direction_y = 0, int direction = GOING_RIGHT)
		{
			steps_left_cpy = max_steps;
			segments_cpy = segments;
			direction_x_cpy = direction_x;
			direction_y_cpy = direction_y;
			direction_cpy = direction;

			for (int segment = 0; segment < segments; ++segment) {
				X[segment] = X.size() / 2 - segment;
				Y[segment] = Y.size() / 2;
			}
		}

		void Spawn()
		{
			steps_left = steps_left_cpy;
			segments = segments_cpy;
			direction_x = direction_x_cpy;
			direction_y = direction_y_cpy;
			direction = direction_cpy;

			X = std::vector<int>(X.size());
			Y = std::vector<int>(Y.size());

			for (int segment = 0; segment < segments; ++segment) {
				X[segment] = X.size() / 2 - segment;
				Y[segment] = Y.size() / 2;
			}
		}

		Snake &operator++() //for turning right
		{
			direction_y = -direction_y;
			std::swap(direction_x, direction_y);

			if (direction == GOING_RIGHT)
				direction = GOING_DOWN;
			else if (direction == GOING_DOWN)
				direction = GOING_LEFT;
			else if (direction == GOING_LEFT)
				direction = GOING_UP;
			else if (direction == GOING_UP)
				direction = GOING_RIGHT;

			return *this;
		}

		Snake &operator--() //for turning left
		{
			direction_x = -direction_x;
			std::swap(direction_x, direction_y);

			if (direction == GOING_RIGHT)
				direction = GOING_UP;
			else if (direction == GOING_UP)
				direction = GOING_LEFT;
			else if (direction == GOING_LEFT)
				direction = GOING_DOWN;
			else if (direction == GOING_DOWN)
				direction = GOING_RIGHT;

			return *this;
		}

		char head;
		char body;
		int	segments;
		int	direction_x;
		int direction_y;
		int direction;
		int	steps_left;
		int food_eaten;
		bool ingame;

		std::vector<int> X, Y;

	private:
		int steps_left_cpy;
		int direction_x_cpy;
		int direction_y_cpy;
		int direction_cpy;
		int segments_cpy;
	};

	extern Snake snake;
}

