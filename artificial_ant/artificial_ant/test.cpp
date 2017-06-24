#include "options.h"

#if TESTING 

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "genetic_programming.h"
#include "custom_console.hpp"
#include <string>

using namespace GP;
using namespace Console;

Individual LoadProgram(std::string program_filepath)
{
	fstream program_file(program_filepath);

	Individual i;

	unordered_map<std::string, char> reverse_tokens;
	reverse_tokens.emplace("MOVE", MOVE);
	reverse_tokens.emplace("RIGHT", RIGHT);
	reverse_tokens.emplace("LEFT", LEFT);
	reverse_tokens.emplace("IF-FOOD-AHEAD", IF_FOOD_AHEAD);
	reverse_tokens.emplace("PROG2", PROG2);
	reverse_tokens.emplace("PROG3", PROG3);

	std::string line;
	while (true) {
		getline(program_file, line);
		if (program_file.eof())
			break;
		i.program += reverse_tokens[line];
	}

	program_file.close();

	return i;
}

TEST_CASE()
{
	SetUp(CONFIG_FILEPATH);
	
	Console::Maximise();
	Console::HideCursor();

	REQUIRE(Evaluate(LoadProgram(program_to_test)) == 0);
}
#endif // TESTING



