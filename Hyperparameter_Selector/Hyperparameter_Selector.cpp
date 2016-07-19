#include "stdafx.h"
#include "stdlib.h"
#include "iostream"
#include "fstream"
#include "time.h"
#include "Genetic_Optimization.h"
#include "math.h"

#define MUTATION_RATE		0.001
#define CROSSOVER_RATE		0.7
#define POPULATION_SIZE		24
#define MAX_GENERATION		100
#define NUM_PARAMS			3
#define FITNESS_SOLUTION	0.95
#define CHROM_LEN			4
#define LR_MAX_VAL			1
#define LR_MAX_POW			-2
#define LR_MIN_VAL			1
#define LR_MIN_POW			-5
#define DP_MAX_VAL			8
#define DP_MAX_POW			-1
#define DP_MIN_VAL			1
#define DP_MIN_POW			-1
#define RS_MAX_VAL			1
#define RS_MAX_POW			-2
#define RS_MIN_VAL			1
#define RS_MIN_POW			-5
#define NULL_BITS			"0000"
#define RAND_NUM			((float)rand()/(RAND_MAX+1))


string paramNames[] = { " Rate", "Dropout Probability", "Regularization Strength" };

int main()
{
	srand((int)time(NULL));

	/* 
	INITILIZATION
		map of hyperparameters pairing the param name to a struct containing information about:
			It's value (As assumed by a chrosome that owns it), it's max value and min value
	*/
	
	map<string, Hyperparameter> hyperparameters;
	Chromosome population[POPULATION_SIZE];
	Chromosome next_population[POPULATION_SIZE];
	float TotalFitness = 0.0f;

	Hyperparameter learning_rate(paramNames[0], bitsetPair(0,0), bitsetPair(LR_MAX_VAL,LR_MAX_POW), bitsetPair(LR_MIN_VAL,LR_MIN_POW));
	Hyperparameter dropout_prob(paramNames[1], bitsetPair(0, 0), bitsetPair(DP_MAX_VAL, DP_MAX_POW), bitsetPair(DP_MIN_VAL, DP_MIN_POW));
	Hyperparameter reg_strength(paramNames[2], bitsetPair(0, 0), bitsetPair(RS_MAX_VAL, RS_MAX_POW), bitsetPair(RS_MIN_VAL, RS_MIN_POW));
	Hyperparameter params[] = { learning_rate, dropout_prob, reg_strength };
	
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
	
	Chromosome solutionChromosome(hyperparameters);	
	bool solutionFound = false; 
	int generation_count = 0;
	while (generation_count < MAX_GENERATION) {
		float total_fitness = 0.0f;


		/*
		FITNESS CALCULATION
			Loop over each chromosome, and calculate the fitness value; Summate fitness values to later use
			for roulette selection of chromosomes during crossover;
			If a combination of parameters is found that suprasses our requirements, stop building generations
		*/
		
		for (int i = 0; i < POPULATION_SIZE; i++) {
			population[i].CalculateFitness();
			total_fitness += population[i].fitness;
			if (population[i].fitness >= FITNESS_SOLUTION) {
				solutionFound = true;
				solutionChromosome = population[i];
			}
		}
		
		if (solutionFound) {
			//Add some messages here
			break;
		}

		/*
		PRODUCING NEXT GENERATION
			
		*/

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
		cout << "Best of the generation " << generation_count << " has fitness " << population[0].fitness << endl;


		//Copy next_population into population;
		for (int i = 0; i < POPULATION_SIZE; i++) {
			population[i] = next_population[i];
		}

		++generation_count;
	}




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

Hyperparameter::Hyperparameter(string _name, pair<bitset<4>, bitset<4>> _value, pair<bitset<4>, bitset<4>> _maximum, pair<bitset<4>, bitset<4>> _minimum)
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
*/


Chromosome::Chromosome(map<string, Hyperparameter> _hyperparameters)
{
	hyperparameters = _hyperparameters;

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

void Chromosome::CalculateFitness()
{
	string command = "python Script.py ";
	for (map<string, Hyperparameter>::iterator it = hyperparameters.begin(); it != hyperparameters.end(); ++it) {
		command += to_string(parseChromosomeValue(it->second)) + " ";
	}
	system(command.c_str());

	//Read textfile that has our accuracy
	string line;
	ifstream myfile("validation_accuracy.txt");
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			fitness = stof(line);
		}
		myfile.close();
	}
}	

/*
##############
# Crossover  #
##############
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

pair<bitset<4>, bitset<4>> bitsetPair(unsigned int v, int p) {
	bitset<CHROM_LEN> power(abs(p));
	if (p < 0) {
		(power ^= bitset<CHROM_LEN>("1000"));
	} 
	return 	pair<bitset<4>, bitset<4>>(bitset<4>(v), power);
}

pair<bitset<4>, bitset<4>> bitsetPair(string v, string p) {
	return 	pair<bitset<4>, bitset<4>> (bitset<4>(v), bitset<4>(p));
}

/*
bitset<CHROM_LEN> random_bitset() {
	string result = "";

	for (int i = 0; i < CHROM_LEN; i++) {
		result += to_string((int)round(RAND_NUM));
	}
	return bitset<CHROM_LEN>(result);
}*/

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
		if (param.maximum.second.to_ulong() > 7 && param.minimum.second.to_ulong() > 7) {
			pow += '1';
			looplength = 3;
		}
		else {
			looplength = 4;
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
	
	
	string SIGN_CHECK = "1000";

	bitset<CHROM_LEN> pow_abs_bitset('0' + pow.to_string().substr(1, CHROM_LEN));

	if ((pow&bitset<CHROM_LEN>(SIGN_CHECK)).to_string() == SIGN_CHECK) {
		return (int)(pow_abs_bitset.to_ulong() * -1);
	}
	else {
		return (int)pow_abs_bitset.to_ulong();
	}
	
}

/*
#######################
# Population Handlers #
#######################
*/

Chromosome Roulette(float total_fitness, Chromosome *population) {

	float sliced_pos = (float)(RAND_NUM * total_fitness);
	float fitness_accumulated = 0.0f;

	for (int i = 0; i < POPULATION_SIZE; i++) {
		fitness_accumulated += population[i].fitness;
		if (fitness_accumulated >= sliced_pos) {
			cout << endl << i << endl;
			return population[i];
		}
	}

	//Should technically never reach this point
	return population[0];
}