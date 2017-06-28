#pragma once


namespace Food
{
	struct Food
	{
		Food() : shape('+') {}

		int	x;
		int y;
		char shape;
	};

	extern Food food;
}