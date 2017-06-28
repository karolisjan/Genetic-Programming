#pragma once

#include <vector>


namespace Snake
{
	enum directions { GOING_UP, GOING_DOWN, GOING_LEFT, GOING_RIGHT };

	struct Snake
	{
		Snake() : head('x'), body('o'), segments(3), direction_y(0), direction_x(1), direction(GOING_RIGHT) {}

		void SetUp(int max_steps, int world_width, int world_height)
		{
			steps_left_cpy = max_steps;
			segments_cpy = segments;
			direction_x_cpy = direction_x;
			direction_y_cpy = direction_y;
			direction_cpy = direction;

			height = world_height;
			width = world_width;

			X = std::vector<int>(world_width * world_height, 0);
			Y = std::vector<int>(world_width * world_height, 0);

			for (int segment = 0; segment < segments; ++segment) {
				X[segment] = width / 2 - segment;
				Y[segment] = height / 2;
			}
		}

		void Spawn()
		{
			food_eaten = 0;
			steps_left = steps_left_cpy;
			segments = segments_cpy;
			direction_x = direction_x_cpy;
			direction_y = direction_y_cpy;
			direction = direction_cpy;

			X = std::vector<int>(X.size(), 0);
			Y = std::vector<int>(Y.size(), 0);

			for (int s = 0; s < segments; ++s) {
				X[s] = width / 2 - s;
				Y[s] = height / 2;
			}
		}

		void Move()
		{
			for (int s = segments; s > 0; --s) {
				Y[s] = Y[s - 1];
				X[s] = X[s - 1];
			}

			X[0] += direction_x;
			Y[0] += direction_y;
		}

		bool IsAlive()
		{
			if (steps_left <= 0)
				return false;

			for (int s = segments; s > 0; --s)
				if (X[s] == X[0] && Y[s] == Y[0])
					return false;

			if (X[0] > X.size())
				return false;
			if (X[0] < 1)
				return false;
			if (Y[0] > Y.size())
				return false;
			if (Y[0] < 1)
				return false;

			return true;
		}

		void ResetSteps()
		{
			steps_left = steps_left_cpy;
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
		int width;
		int height;
	};

	extern Snake snake;
}

