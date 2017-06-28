#include <iostream>
#include <console_opt.hpp>
#include <boost\filesystem.hpp>
#include <boost\lexical_cast.hpp>
#include "genetic_programming.h"

using namespace GP;

void DrawWalls()
{
	for (int i = 0; i <= HEIGHT + 1; ++i) {
		consoleOptions::setCursorPosition(0, i);
		cout << '.';
		consoleOptions::setCursorPosition(WIDTH * 2 + 2, i);
		cout << '.';
	}

	for (int i = 0; i <= WIDTH + 1; ++i) {
		consoleOptions::setCursorPosition(i * 2, 0);
		cout << '.';
		consoleOptions::setCursorPosition(i * 2, HEIGHT + 1);
		cout << '.';
	}

	consoleOptions::setCursorPosition(15, HEIGHT + 3);
	cout << "Points: ";
}

string solution = 
{
	IF_DANGER_RIGHT,		IF_DANGER_AHEAD,		IF_DANGER_TWO_AHEAD,	LEFT, 
																			FORWARD,
													IF_MOVING_RIGHT,		LEFT,	
																			FORWARD,
							IF_DANGER_AHEAD,		IF_DANGER_LEFT,			IF_FOOD_UP,				RIGHT,
																									RIGHT,
																			IF_DANGER_TWO_AHEAD,	LEFT,
																									FORWARD,
													IF_MOVING_UP,			IF_DANGER_TWO_AHEAD,	IF_FOOD_AHEAD,	IF_MOVING_RIGHT,	RIGHT,
																																		FORWARD,
																													IF_MOVING_DOWN,		LEFT,
																																		RIGHT,
																									PROG2,			FORWARD,
																													FORWARD,
																			IF_MOVING_RIGHT,		RIGHT,
																									FORWARD

};

string GetProgram(string filename)
{
	string snake_program;

	unordered_map<string, char> tokens;
	tokens.emplace("FORWARD", FORWARD);
	tokens.emplace("RIGHT", RIGHT);
	tokens.emplace("LEFT", LEFT);
	tokens.emplace("IF-FOOD-AHEAD", IF_FOOD_AHEAD);
	tokens.emplace("IF_FOOD_UP", IF_FOOD_UP);
	tokens.emplace("IF_FOOD_RIGHT", IF_FOOD_RIGHT);
	tokens.emplace("IF_DANGER_AHEAD", IF_DANGER_AHEAD);
	tokens.emplace("IF_DANGER_TWO_AHEAD", IF_DANGER_TWO_AHEAD);
	tokens.emplace("IF_DANGER_TWO_AHEAD", IF_DANGER_LEFT);
	tokens.emplace("IF_DANGER_RIGHT", IF_DANGER_RIGHT);
	tokens.emplace("IF_MOVING_UP", IF_MOVING_UP);
	tokens.emplace("IF_MOVING_DOWN", IF_MOVING_DOWN);
	tokens.emplace("IF_MOVING_LEFT", IF_MOVING_LEFT);
	tokens.emplace("IF_MOVING_RIGHT", IF_MOVING_RIGHT);
	tokens.emplace("PROG2", PROG2);
	tokens.emplace("PROG3", PROG3);

	fstream program_file(filename, ios::in);

	string line;
	while (true) {
		getline(program_file, line);
		if (program_file.eof())
			break;
		snake_program += tokens[line];
	}

	program_file.close();

	return snake_program;
}

void Test(string &program)
{
	snake = Snake();
	food.spawn();

	DrawWalls();

	while (snake.ingame && snake.steps_left > 0) {
		current_node = program.begin();
		Next();
	}
}

void SaveProgram(string &program, string filename)
{
	fstream results_file(filename, ios::out);

	unordered_map<char, string> tokens;
	tokens.emplace(FORWARD, "FORWARD");
	tokens.emplace(RIGHT, "RIGHT");
	tokens.emplace(LEFT, "LEFT");
	tokens.emplace(IF_FOOD_AHEAD, "IF-FOOD-AHEAD");
	tokens.emplace(IF_FOOD_UP, "IF_FOOD_UP");
	tokens.emplace(IF_FOOD_RIGHT, "IF_FOOD_RIGHT");
	tokens.emplace(IF_DANGER_AHEAD, "IF_DANGER_AHEAD");
	tokens.emplace(IF_DANGER_TWO_AHEAD, "IF_DANGER_TWO_AHEAD");
	tokens.emplace(IF_DANGER_LEFT, "IF_DANGER_TWO_AHEAD");
	tokens.emplace(IF_DANGER_RIGHT, "IF_DANGER_RIGHT");
	tokens.emplace(IF_MOVING_UP, "IF_MOVING_UP");
	tokens.emplace(IF_MOVING_DOWN, "IF_MOVING_DOWN");
	tokens.emplace(IF_MOVING_LEFT, "IF_MOVING_LEFT");
	tokens.emplace(IF_MOVING_RIGHT, "IF_MOVING_RIGHT");
	tokens.emplace(PROG2, "PROG2");
	tokens.emplace(PROG3, "PROG3");

	for (int i = 0; i < program.size(); ++i)
		results_file << tokens[program[i]] << "\n";

	results_file.close();
}

int main()
{
	consoleOptions::SetUpConsole();

	boost::filesystem::path fullpath(boost::filesystem::current_path());
	string temp = boost::lexical_cast<string>(fullpath);
	string curr_dir = string(temp.begin() + 1, temp.end() - 1);

	SetUp(curr_dir + "\\config.ini");

	//Test(GetProgram("snake_program.txt"));

	vector<Individual> parents(popsize);
	Individual current, best;

	best.program = GetProgram("snake_program_6.txt");
	Evaluate(best);

	for (int i = 0; i < popsize; ++i) {
		parents[i].program = BuildTree(max_depth, tree_generation_method);
		Evaluate(parents[i]);

		if (parents[i] < best)
			best = parents[i];
	}

	bool break_flag = false;

	for (int gen = 0; gen < generations; ++ gen) {

		vector<Individual> children;

		for (int i = 0; i < popsize; ++i) {
			Individual offspring = parents[Tournament(parents, tournament_size, IsLess<Individual>)],
					   parent = parents[Tournament(parents, tournament_size, IsLess<Individual>)];

			if (random<float>() < pcrossover && offspring != parent) 
				SubtreeXO(offspring.program, parent.program);
			else if (random<float>() < psubmutation)
				SubtreeMutation(offspring.program);
			else 
				PtMutation(offspring.program);

			Evaluate(offspring);

			if (offspring < current)
				current = offspring;

			children.push_back(offspring);

			if (break_flag = GetAsyncKeyState(VK_ESCAPE)) {
				char c = 0;

				printf("\n");

				while (c != 'Y' && c != 'y' && c != 'N' && c != 'n') {
					printf("\rDo you want quit the program? (Y/N): ");
					cin >> c;
				}

				if (c == 'Y' || c == 'y') {
					break;
				}
				else {
					printf("\n");
					break_flag = false;
				}
			}
		}

		parents = children;

		if (current < best)
			best = current;
		else if (current > best)
			parents[Tournament(parents, tournament_size, IsLess<Individual>)] = best;

		if (!break_flag)
			printf("Gen: %d, Program size: %d, Performance: %.4f\n", gen + 1, best.program.size(), best.fitness);

		if (best.fitness <= 0)
			break;

		if (break_flag)
			break;
	}

	char c = 0;

	if (!break_flag)
		printf("\n");
	
	while (c != 'Y' && c!= 'y' && c != 'N' && c != 'n') {
		printf("\rSave the best program? (Y/N): ");
		cin >> c;
	}

	if (c == 'Y' || c == 'y') {
		string filename;
		printf("Save as: ");
		cin >> filename;
		filename += ".txt";
		SaveProgram(best.program, filename);
	}

	printf("\n");
	system("pause");
}

