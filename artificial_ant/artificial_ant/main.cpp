#include <iostream>
#include <stdio.h>
#include <string>
#include <exception>
#include <boost\filesystem.hpp>
#include <boost\lexical_cast.hpp>

#include "utils/ini_reader.hpp"
#include "ant.h"
#include "world.h"
#include "functions_terminals.h"
#include "genetic_programming.h"


namespace 
{
	std::string GetCurrentPath()
	{
		boost::filesystem::path fullpath(boost::filesystem::current_path());
		std::string temp = boost::lexical_cast<string>(fullpath);
		return std::string(temp.begin() + 1, temp.end() - 1);
	}

	void SetUp(GP& gp, std::string config_path, bool& save, std::string& save_as_path)
	{
		IniReader ini(config_path);

		std::string section = "GENERAL_OPTIONS";
		bool verbose = ini.Read<bool>(section, "verbose");
		save = ini.Read<bool>(section, "save_best_program");
		save_as_path = ini.Read<std::string>(section, "save_as");

		section = "TESTING";
		bool testing_on = ini.Read<bool>(section, "testing_on");
		std::string program_to_test = ini.Read<std::string>(section, "program_to_test");

		section = "GP_PARAMETERS";
		int seed = ini.Read<int>(section, "seed");
		int popsize = ini.Read<int>(section, "popsize");
		int max_depth = ini.Read<int>(section, "max_init_depth");
		int max_length = ini.Read<int>(section, "max_program_length");
		int generations = ini.Read<int>(section, "generations");
		int tournament_size = ini.Read<int>(section, "tournament_size");
		float pm_per_node = ini.Read<float>(section, "mutation_rate_per_node");
		float pcrossover = ini.Read<float>(section, "crossover_rate");

		section = "TRAIL_OPTIONS";
		std::string trail_path = ini.Read<std::string>(section, "trail_filepath");

		section = "ANT_OPTIONS";
		int ant_x = ini.Read<int>(section, "init_spawn_x");
		int ant_y = ini.Read<int>(section, "init_spawn_y");
		int facing_x = ini.Read<int>(section, "facing_x");
		int facing_y = ini.Read<int>(section, "facing_y");
		int max_steps = ini.Read<int>(section, "max_steps");

		Jumptable::SetUp();

		gp.SetUp(seed, popsize, generations, pcrossover,
			tournament_size, pm_per_node, max_length, max_depth, verbose);

		World::world.Load(trail_path);
		Ant::ant.SetUp(ant_x, ant_y, std::make_pair(facing_x, facing_y), max_steps);

		ini.Close(); 
	}
}

int main()
{
	std::string current_path = GetCurrentPath();
	std::string config_path = current_path + "\\config.ini";

	GP gp;
	bool save; 
	std::string save_as_path;

	try {
		SetUp(gp, config_path, save, save_as_path);
	}
	catch (std::exception &e) {
		printf("%s", e.what());
		getchar();
		return 1;
	}

	std::string best_program = gp.Run();

	if (save)
		FunctionsTerminals::SaveProgram(best_program, save_as_path);

	printf("\nDone!");
	getchar();

	return 0;
}