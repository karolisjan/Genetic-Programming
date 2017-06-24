#include <boost\filesystem.hpp>
#include <boost\lexical_cast.hpp>
#include "genetic_programming.h"

int Print(fstream &output_file, string &program, int node)
{
	int node1 = 0, node2;

	if (program[node] < FSET_START) {
		if (program[node] <= num_vars)
			output_file << "x" << (int)program[node];
		else
			output_file << args[program[node] - 1];
		return node + 1;
	}
	else {
		switch (program[node]) {
			//case EXP:
			//	output_file << "exp(";
			//	node2 = Print(output_file, program, node + 1);
			//	output_file << ")";
			//	return node2;
			//case SIN:
			//	output_file << "sin(";
			//	node2 = Print(output_file, program, node + 1);
			//	output_file << ")";
			//	return node2;
			//case COS:
			//	output_file << "cos(";
			//	node2 = Print(output_file, program, node + 1);
			//	output_file << ")";
			//	return node2;
			//case LOG:
			//	output_file << "log(abs(";
			//	node2 = Print(output_file, program, node + 1);
			//	output_file << "))";
			//	return node2;
			case ADD:
				output_file << "(";
				node1 = Print(output_file, program, node + 1);
				output_file << " + ";
				break;
			case SUB:
				output_file << "(";
				node1 = Print(output_file, program, node + 1);
				output_file << " - ";
				break;
			case MUL:
				output_file << "(";
				node1 = Print(output_file, program, node + 1);
				output_file << " * ";
				break;
			case DIV:
				output_file << "(";
				node1 = Print(output_file, program, node + 1);
				output_file << " / ";
				break;
			//case POW:
			//	output_file << "(";
			//	node1 = Print(output_file, program, node + 1);
			//	output_file << " ^ ";
			//	break;
		}
	}

	node2 = Print(output_file, program, node1);
	output_file << ")";

	return node2;
}

void PrintInfixProgram(string &program, string &output_filename)
{
	fstream output_file(output_filename, ios::out);
	int node = 0;
	Print(output_file, program, node);
	output_file.close();
}

void SaveProgram(string &program, string filename)
{
	fstream args_file(filename + "_args.txt", ios::out), prog_file(filename + ".txt", ios::out);

	int i = 0;
	for (; i < args.size(); ++i)
		args_file << args[i] << "\n";

	for (i = 0; i < program.size(); ++i)
		prog_file << (int)program[i] << "\n";

	args_file.close();
	prog_file.close();

	PrintInfixProgram(program, filename + "_infix.txt");
}

void TestProgram(string &program, string output_filename)
{
	fstream output_file(output_filename + ".csv", ios::out);

	output_file << "x,y\n";

	for (int fit_case = 0; fit_case < input_data.size(); ++fit_case) {
		for (int var = 0; var < num_vars; ++var) {
			args[var] = input_data[fit_case][var];
			output_file << args[var] << ",";
		}

		current_node = program.begin();
		output_file << Next() << "\n";
	}

	output_file.close();
}

string LoadProgram(string &program_name)
{
	fstream program_file(program_name, ios::in);

	string program, line;

	while (true) {
		getline(program_file, line);
		if (program_file.eof())
			break;
		program += stoi(line);
	}

	program_file.close();

	return program;
}

int main() {
	boost::filesystem::path fullpath(boost::filesystem::current_path());
	string temp = boost::lexical_cast<string>(fullpath);
	string curr_dir = string(temp.begin() + 1, temp.end() - 1);

	SetUp(curr_dir + "\\config.ini");

	cout << "SYMBOLIC REGRESSION w/ GENETIC PROGRAMMING\n" << endl;
	
	cout << "Pseudo random number generator seed\t= " << seed << endl;
	cout << "Population size\t\t\t\t= " << popsize << endl;
	cout << "Number of generations\t\t\t= " << generations << endl;
	cout << "Max depth at the initialisation\t\t= " << max_depth << endl;
	cout << "Max program length\t\t\t= " << max_length << endl;
	cout << "Tournament size\t\t\t\t= " << tournament_size << endl;
	cout << "Mutation probability per node\t\t= " << pm_per_node << endl;
	cout << "Crossover probability\t\t\t= " << pcrossover << endl;
	cout << "Data filename\t\t\t\t= " << fitness_data_filename << endl;
	cout << "Number of independent variables\t\t= " << num_vars << endl;
	cout << "Number of random constants\t\t= " << num_rand_nums << endl;
	if (num_rand_nums) {
		cout << "Min constant\t\t\t\t= " << min_rand_num << endl;
		cout << "Max constant\t\t\t\t= " << max_rand_num << endl;
	}
	if (seed_w_program)
		cout << "Seed for the initial population\t\t= " << prog_name << endl;

	printf("\nPress any key to continue...\n");
	cin.get();

	vector<Individual> population(popsize);
	Individual best;

	if (seed_w_program) {
		best.program = LoadProgram(prog_name);
		Evaluate(best);
	}

	for (int i = 0; i < popsize; ++i) {
		population[i].program = BuildTree(random(2, max_depth), random(0, 1)); //Ramped half-and-half.
		Evaluate(population[i]);

		if (population[i] < best)
			best = population[i];
	}

	int epochs = generations * popsize, break_flag = 0;

	for (int epoch = 0; epoch < epochs; ++epoch) {
		Individual &offspring = population[Tournament(population, tournament_size, IsGreater<Individual>)]
			= population[Tournament(population, tournament_size, IsLess<Individual>)];

		if (random<float>() < pcrossover) 
			SubtreeXO(offspring, population[Tournament(population, tournament_size, IsLess<Individual>)]);
		else 
			PtMutation(offspring.program);

		Evaluate(offspring);

		if (offspring < best)
			best = offspring;

		if (((epoch + 1) % popsize) == 0) 
			printf("Gen: %d, Program size: %d, Performance: %.4f\n", (1 + epoch / popsize), best.program.size(), best.fitness);

		if (best.fitness < 0.0001)
			break;

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
				break_flag = 0;
				printf("\n");
			}
		}
	}

	if (!break_flag)
		printf("\n");

	char c = 0;
	while (c != 'Y' && c != 'y' && c != 'N' && c != 'n') {
		printf("\rSave the best program? (Y/N): ");
		cin >> c;
	}

	if (c == 'Y' || c == 'y') {
		string filename;
		printf("Save as: ");
		cin >> filename;
		SaveProgram(best.program, filename);
	}

	c = 0;
	while (c != 'Y' && c != 'y' && c != 'N' && c != 'n') {
		printf("\rTest the best program? (Y/N): ");
		cin >> c;
	}

	if (c == 'Y' || c == 'y') {
		string filename;
		printf("Output file: ");
		cin >> filename;
		TestProgram(best.program, filename);
	}

	printf("\nPress any key to quit...\n");
	cin.get();
	cin.get();

	return 0;
}