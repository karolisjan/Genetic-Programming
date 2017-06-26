#pragma once

#include <ostream>
#include <vector>
#include <string>
#include <fstream>


namespace World
{
	struct World
	{
		void SetUp(std::string trail_path)
		{
			std::ifstream trail_file(trail_path);

			std::string entry;

			num_food_pieces = 0;

			while (getline(trail_file, entry)) {
				trail.push_back(std::vector<char>(entry.size(), 0));

				for (int i = 0; i < entry.size(); ++i) {
					trail.back()[i] = entry[i];
					if (entry[i] == 'x')
						num_food_pieces += 1;
				}
			}

			trail_file.close();
			width = entry.size();
			height = trail.size();
			trail_copy = trail;
		}

		void SetUp(std::vector<std::vector<char>>& trail)
		{
			trail_copy = trail;
			Refresh();
		}

		bool HasFood(int x, int y)
		{
			return trail[y][x] == 'x';
		}

		void RemovePiece(int x, int y)
		{
			trail[y][x] = '.';
		}

		void Refresh()
		{
			trail = trail_copy;
		}

		std::vector<std::vector<char>> trail;
		int width, height, num_food_pieces;

	private:
		std::vector<std::vector<char>> trail_copy;
	};

	extern World world;
}