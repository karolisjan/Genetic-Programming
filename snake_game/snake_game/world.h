#pragma once


namespace World
{
	struct World
	{
		void SetUp(int height, int width)
		{
			this->height = height;
			this->width = width;
		}

		int height;
		int width;
	};

	extern World world;
}