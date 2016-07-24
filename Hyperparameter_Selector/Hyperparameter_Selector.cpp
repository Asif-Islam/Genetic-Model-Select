#include "stdafx.h"
#include "stdlib.h"
#include "iostream"
#include "fstream"
#include "time.h"
#include "Genetic_Optimization.h"
#include "math.h"
#include "thread"
#include "mutex"

#define NUM_PARAMS			4
#define LR_MAX_VAL			1			//Learning Rate Max/Min in Standard Notation (V * 10^P)
#define LR_MAX_POW			-2		
#define LR_MIN_VAL			1
#define LR_MIN_POW			-5
#define DP_MAX_VAL			8			//Dropout Probability Max/Min in Standard Notation (V * 10^P)
#define DP_MAX_POW			-1
#define DP_MIN_VAL			1
#define DP_MIN_POW			-1
#define RS_MAX_VAL			1			//Regularization Strength Max/Min in Standard Notation (V * 10^P)
#define RS_MAX_POW			-2
#define RS_MIN_VAL			1
#define RS_MIN_POW			-5
#define ND_MAX_VAL			5			//Neuron Dimension  Max/Min in Standard Notation (V * 10^P)
#define ND_MAX_POW			2
#define ND_MIN_VAL			5
#define ND_MIN_POW			1
#define NULL_BITS			"00000000"					
#define NUM_THREADS			2


mutex fitness_mutex;
mutex print_mutex;
float total_fitness = 0.0f;
string paramNames[] = { "Learning Rate", "Dropout Probability", "Regularization Strength", "Neuron Dimension" };

int main()
{
	cout << "Starting Genetic Optimization of Two Layer Neural Net..." << endl;
	srand((int)time(NULL));

	/* 
	INITILIZATION
		map of hyperparameters pairing the param name to a struct containing information about:
			It's value (As assumed by a chrosome that owns it), it's max value and min value
	*/
	
	map<string, Hyperparameter> hyperparameters;
	Chromosome population[POPULATION_SIZE];
	Chromosome next_population[POPULATION_SIZE];

	Hyperparameter learning_rate(paramNames[0], bitsetPair(0,0), bitsetPair(LR_MAX_VAL,LR_MAX_POW), bitsetPair(LR_MIN_VAL,LR_MIN_POW));
	Hyperparameter dropout_prob(paramNames[1], bitsetPair(0, 0), bitsetPair(DP_MAX_VAL, DP_MAX_POW), bitsetPair(DP_MIN_VAL, DP_MIN_POW));
	Hyperparameter reg_strength(paramNames[2], bitsetPair(0, 0), bitsetPair(RS_MAX_VAL, RS_MAX_POW), bitsetPair(RS_MIN_VAL, RS_MIN_POW));
	Hyperparameter neuron_dim(paramNames[3], bitsetPair(0, 0), bitsetPair(ND_MAX_VAL, ND_MAX_POW), bitsetPair(ND_MIN_VAL, ND_MIN_POW));
	Hyperparameter params[] = { learning_rate, dropout_prob, reg_strength, neuron_dim };
	
	for (int i = 0; i < NUM_PARAMS; i++) {
		hyperparameters.insert(pair<string, Hyperparameter>(paramNames[i], params[i]));
	}

	/*
	POPULATION GENERATION
		Loop over through our array and instantiate N chromosomes ; the consutrctor will produce random values 
		for each parameter between the set max and mins
	*/

	for (int i = 0; i < POPULATION_SIZE; i++) {
		population[i] = Chromosome(hyperparameters);
	}

	/*
	#############
	# MAIN LOOP #
	#############
	*/
	Chromosome globalBest(hyperparameters);
	int globalBestGen = 0;
	int generation_count = 0;
	while (generation_count < MAX_GENERATION) {


		/*
		FITNESS CALCULATION
			Create a subpopulation for each thread, and initialize each thread to Calculate the fitness of each chromosome
			in their subpoulation. Afterwards, wait for each thread to complete execution, and refill the subpopulation fitnesses
			into the original population
		*/
		
		Chromosome population_groups[NUM_THREADS][POPULATION_SIZE / NUM_THREADS];
		thread threads[NUM_THREADS];
		int count = 0;
		for (int i = 0; i < NUM_THREADS; i++) {
			for (int j = 0; j < POPULATION_SIZE / NUM_THREADS; j++) {
				population_groups[i][j] = population[count];
				++count;
			}
		}
		
		for (int i = 0; i < NUM_THREADS; i++) {
			threads[i] = thread(ThreadedCalcFitness, population_groups[i], "validation_" + to_string(i));
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			threads[i].join();
		}

		count = 0;
		for (int i = 0; i < NUM_THREADS; i++) {
			for (int j = 0; j < POPULATION_SIZE / NUM_THREADS; j++) {
				population[count].fitness = population_groups[i][j].fitness;
				count++;
			}
		}

		/*
		PRODUCING NEXT GENERATION
			Copy construct two offspring chromosomes based on two current chromosomes chosen by roulette
			//If the crossover passes, attempt to mutate both chromosomes and append to the next generation
		*/

		cout << "Producing next generation..." << endl;
		int j = 0;
		do {
			Chromosome offspring1(Roulette(total_fitness, population));
			Chromosome offspring2(Roulette(total_fitness, population));

			//Crossover and Mutate on both offspring
			bool success_crossover = Crossover(offspring1, offspring2);
			if (success_crossover) {
				Mutate(offspring1);
				Mutate(offspring2);

				//Add to new population
				next_population[j] = offspring1;
				next_population[++j] = offspring2;

				++j;
			}

		} while (j < (POPULATION_SIZE / 4) * 3);

		/*
		ELITISM
		1/4th of the next generation's population will be taken from the current generation; 
		A limited reverse-bubble-sort for the 1/4 of the population's size is conducted to take the most fit
		*/

		int next_pop_pos = j;
		for (int i = 0; i < POPULATION_SIZE - j; i++) {
			for (int r = POPULATION_SIZE; r > 0; r--) {
				if (population[r].fitness > population[r - 1].fitness) {
					float temp = population[r].fitness;
					population[r].fitness = population[r - 1].fitness;
					population[r - 1].fitness = temp;
				}
			}
			next_population[next_pop_pos] = population[i];
			++next_pop_pos;
		}

		/* HOUSEKEEPING
			Keep track of the best of the current generation as well as a global best from all generations thus far.
			Then, push all chromosomes stored in the next_population into our running population array
		*/
		cout << "BEST OF THE GENERATION " << generation_count << endl;
		population[0].print();

		if (population[0].fitness > globalBest.fitness) {
			globalBest = population[0];
			globalBestGen = generation_count;
		}

		//Copy next_population into population;
		for (int i = 0; i < POPULATION_SIZE; i++) {
			population[i] = next_population[i];
		}


		cout << "BEST THUS FAR: GENERATION " << globalBestGen << endl;
		globalBest.print();
		total_fitness = 0.0f;
		++generation_count;
	}
	system("PAUSE");



} //END

/*
 ================================
|	   F U N C T I O N S		 |
 ================================
*/



/*
##############################
# Hyperparameter Constructor #
##############################


*/

Hyperparameter::Hyperparameter() {}

Hyperparameter::Hyperparameter(string _name, pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> _value, pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> _maximum, pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> _minimum)
{
	name = _name;
	value = _value;
	maximum = _maximum;
	minimum = _minimum;
}

/*
##########################
# Chromosome Constructor #
##########################

Accepts a map of hyperparameters, and loops to create valid values for each hyperparmeter by calling
the function VerifyParamValue
*/


Chromosome::Chromosome(map<string, Hyperparameter> _hyperparameters)
{
	hyperparameters = _hyperparameters;
	fitness = 0.0f;
	for (map<string, Hyperparameter>::iterator it = hyperparameters.begin(); it != hyperparameters.end(); ++it) {
		do {
			it->second.value = random_bitset(it->second);
		} while (!VerifyParamValue(it->second));
	}


}

Chromosome::Chromosome() { }

/*
#################################
# Chromosome: Calculate Fitness #
#################################
*/


/*
	Loop over the provided subpopulation for the thread;
	Send in a string file for which the python script will write it's validation accuracy to,
	After the scrpit is complete, use a mutex to add onto the total fitness, and verify if we
	have found an optimal solution
*/
void ThreadedCalcFitness(Chromosome *subpopulation, string thread_title) {

	for (int i = 0; i < POPULATION_SIZE / NUM_THREADS; i++) {
		subpopulation[i].CalculateFitness(thread_title);
		
		fitness_mutex.lock();
		total_fitness += subpopulation[i].fitness;
		fitness_mutex.unlock();

		if (subpopulation[i].fitness >= FITNESS_SOLUTION) {
			print_mutex.lock();
			subpopulation[i].print();
			cout << "Pausing Program..." << endl;
			system("PAUSE");
			print_mutex.unlock();
		}
	}

}

/*
	Build a command that runs the python script, with command line arguments for each hyperparameter
	plus a textfile name to write to. When complete, open the file, read the first line and parse to
	a float, storing it into the Chroms
*/

void Chromosome::CalculateFitness(string thread_title)
{
	string command = "python __main__.py ";
	for (map<string, Hyperparameter>::iterator it = hyperparameters.begin(); it != hyperparameters.end(); ++it) {
		if (it->first == paramNames[3]) {
			command += to_string((int)parseChromosomeValue(it->second)) + " ";
		}
		else {
			command += to_string(parseChromosomeValue(it->second)) + " ";
		}

	}
	command += thread_title;
	system(command.c_str());

	//Read textfile that has our accuracy
	string line;
	ifstream myfile(thread_title + ".txt");
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			fitness = stof(line);
		}
		myfile.close();
	}

	print_mutex.lock();
	cout << "Validation Accuracy of: " << fitness << endl;
	print_mutex.unlock();
}	

/*
##############
# Crossover  #
##############
*/

/*
First generate a random number less than the crossover_rate to determine if we should cross over;
Receives two offsprings which are copy constructed from their parents; Convert their bitset value/powers
into strings, and perform crossover using substring concatenation. Verify it is valid before exiting
*/

bool Crossover(Chromosome &offspring1, Chromosome &offspring2) {


	if (RAND_NUM < CROSSOVER_RATE) {

		for (int i = 0; i < NUM_PARAMS; i++) {

			//Convert our bits to strings for easy cross over
			string p1param1 = offspring1.hyperparameters[paramNames[i]].value.first.to_string();
			string p1param2 = offspring1.hyperparameters[paramNames[i]].value.second.to_string();
			string p2param1 = offspring2.hyperparameters[paramNames[i]].value.first.to_string();
			string p2param2 = offspring2.hyperparameters[paramNames[i]].value.second.to_string();
			pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> value_offspring1 = offspring1.hyperparameters[paramNames[i]].value;
			pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> value_offspring2 = offspring2.hyperparameters[paramNames[i]].value;

			do {
				offspring1.hyperparameters[paramNames[i]].value = value_offspring1;
				offspring2.hyperparameters[paramNames[i]].value = value_offspring2;

				//Crossover positions
				int crossover_pos1 = (int)round(RAND_NUM * CHROM_LEN);
				int crossover_pos2 = (int)round(RAND_NUM * CHROM_LEN);

				//Param1 crossover
				string n1param1 = p1param1.substr(0, crossover_pos1) + p2param1.substr(crossover_pos1, CHROM_LEN);
				string n1param2 = p1param2.substr(0, crossover_pos2) + p2param2.substr(crossover_pos2, CHROM_LEN);

				//Param2 crossover
				string n2param1 = p2param1.substr(0, crossover_pos1) + p1param1.substr(crossover_pos1, CHROM_LEN);
				string n2param2 = p2param2.substr(0, crossover_pos2) + p1param2.substr(crossover_pos2, CHROM_LEN);

				//Before assigning, parse the new expected value and ensure it's in the hyperparameter's range
				offspring1.hyperparameters[paramNames[i]].value = bitsetPair(n1param1, n1param2);
				offspring2.hyperparameters[paramNames[i]].value = bitsetPair(n2param1, n2param2);

			} while (!VerifyParamValue(offspring1.hyperparameters[paramNames[i]]) || !VerifyParamValue(offspring2.hyperparameters[paramNames[i]]));

		}
		return true;
	}
	else {
		return false;
	}
}

/*
##############
# Mutate  #
##############

Starting with a string of null bits (all 0s), iterate through each bit; if we are less than the
very small mutation rate, flip that bit to 1. Then use this string and XOR with the pair of bitsets
for the given hyperparameter, flipping the bits where the operator bitstring is 1
*/



void Mutate(Chromosome &chromosome)
{

	for (int i = 0; i < NUM_PARAMS; i++) {
		pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> value = chromosome.hyperparameters[paramNames[i]].value;
		do {
			chromosome.hyperparameters[paramNames[i]].value = value;

			string flip_operator[] = { NULL_BITS, NULL_BITS };
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < CHROM_LEN; k++) {

					if (RAND_NUM < MUTATION_RATE) {
						flip_operator[j].at(k) = '1';
					}
				}
			}

			//XOR-assign with flip-operator of same length; XOR-ing with 1 will always flip the bit, while 0 maintaints the bit
			(chromosome.hyperparameters[paramNames[i]].value.first ^= bitset<CHROM_LEN>(flip_operator[0]));
			(chromosome.hyperparameters[paramNames[i]].value.second ^= bitset<CHROM_LEN>(flip_operator[1]));

		} while (!VerifyParamValue(chromosome.hyperparameters[paramNames[i]]));
	}
}

/*
##############################
# Helper: Bitset Generators  #
##############################
*/

pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> bitsetPair(unsigned int v, int p) {
	bitset<CHROM_LEN> power(abs(p));
	if (p < 0) {
		(power ^= bitset<CHROM_LEN>("10000000"));		//HARDCODED
	} 
	return 	pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>>(bitset<CHROM_LEN>(v), power);
}

pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> bitsetPair(string v, string p) {
	return 	pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> (bitset<CHROM_LEN>(v), bitset<CHROM_LEN>(p));
}

pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> random_bitset(Hyperparameter param) {
	//Generate val-bitset first
	string val = "";
	for (int i = 0; i < CHROM_LEN; i++) {
		val += to_string((int)round(RAND_NUM));
	}
	bitset<CHROM_LEN> random_value(val);

	string pow = "";
	if (param.maximum.second.to_string() == param.minimum.second.to_string()) {
		return pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>>(bitset<CHROM_LEN>(val), param.maximum.second);
	}
	else {
		int looplength;
		if (param.maximum.second.to_ulong() > 127 && param.minimum.second.to_ulong() > 127) {	//HARD CODED
			pow += '1';
			looplength = CHROM_LEN - 1;		
		}
		else {
			looplength = CHROM_LEN;
		}

		for (int i = 0; i < looplength; i++) {
			pow += to_string((int)round(RAND_NUM));
		}
	}
	return pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>>(bitset<CHROM_LEN>(val), bitset<CHROM_LEN>(pow));
}


/*
###############################
# Helper: Chromosome Parsers  #
###############################
*/

bool VerifyParamValue(Hyperparameter param) {
	float max = (float)(param.maximum.first.to_ulong() * pow(10, parsePow(param.maximum.second)));
	float min = (float)(param.minimum.first.to_ulong() * pow(10, parsePow(param.minimum.second)));
	float val = (float)(param.value.first.to_ulong() * pow(10, parsePow(param.value.second)));

	return (val <= max && val >= min);
}

float parseChromosomeValue(Hyperparameter param) {
	return (float)(param.value.first.to_ulong() * pow(10, parsePow(param.value.second)));
}

int parsePow(bitset<CHROM_LEN> pow) {
	
	
	string SIGN_CHECK = "10000000";		//HARDCODED

	bitset<CHROM_LEN> pow_abs_bitset('0' + pow.to_string().substr(1, CHROM_LEN));

	if ((pow&bitset<CHROM_LEN>(SIGN_CHECK)).to_string() == SIGN_CHECK) {
		return (int)(pow_abs_bitset.to_ulong() * -1);
	}
	else {
		return (int)pow_abs_bitset.to_ulong();
	}
	
}

void Chromosome::print() {
	for (int i = 0; i < NUM_PARAMS; i++) {
		cout << paramNames[i] << " ";
		cout << (float)(hyperparameters[paramNames[i]].value.first.to_ulong() * pow(10, parsePow(hyperparameters[paramNames[i]].value.second))) << " ";
	}
	cout << "Fitness " << fitness << endl;

}

/*
############
# Roulette #
############

Effectively weighted probablity: Generate a number between 0 and the total fitness, then loop over
each Chromosome and add their fitness to a sum. If that sum is larger than our random number, then
select said Chromosome. Chromosomes with higher fitness are favoured
*/

Chromosome Roulette(float total_fitness, Chromosome *population) {

	float sliced_pos = (float)(RAND_NUM * total_fitness);
	float fitness_accumulated = 0.0f;

	for (int i = 0; i < POPULATION_SIZE; i++) {
		fitness_accumulated += population[i].fitness;
		if (fitness_accumulated >= sliced_pos) {
			return population[i];
		}
	}

	//Should technically never reach this point
	return population[0];
}

