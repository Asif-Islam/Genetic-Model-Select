#include "stdafx.h"
#include "stdlib.h"
#include "iostream"
#include "time.h"
#include "Genetic_Optimization.h"
#include "math.h"

#define MUTATION_RATE		0.001
#define CROSSOVER_RATE		0.7
#define POPULATION_SIZE		25
#define MAX_GENERATION		100
#define NUM_PARAMS			3
#define FITNESS_SOLUTION	0.95
#define CHROM_LEN			4
#define LR_MAX_VAL			0
#define LR_MAX_POW			0
#define LR_MIN_VAL			0
#define LR_MIN_POW			0
#define DP_MAX_VAL			0
#define DP_MAX_POW			0
#define DP_MIN_VAL			0
#define DP_MIN_POW			0
#define RS_MAX_VAL			0
#define RS_MAX_POW			0
#define RS_MIN_VAL			0
#define RS_MIN_POW			0

#define NULL_BITS			"0000"
#define RAND_NUM			((float)rand()/(RAND_MAX+1))


string paramNames[] = { " Rate", "Dropout Probability", "Regularization Strength" };

int main()
{
	int x = 2;
	x = x * (3 * 5 == 14);
	cout << x;
	system("PAUSE");
	srand((int)time(NULL));

	/* Initialization:
		map of hyperparameters pairing the param name to a struct containing information about:
			It's value (As assumed by a chrosome that owns it), it's max value and min value
	*/
	
	map<string, Hyperparameter> hyperparameters;
	list<Chromosome> population;
	float TotalFitness = 0.0f;



	Hyperparameter learning_rate(paramNames[0], bitsetPair(0,0), bitsetPair(LR_MAX_VAL,LR_MAX_POW), bitsetPair(LR_MIN_VAL,LR_MIN_POW));
	Hyperparameter dropout_prob(paramNames[0], bitsetPair(0, 0), bitsetPair(DP_MAX_VAL, DP_MAX_POW), bitsetPair(DP_MIN_VAL, DP_MIN_POW));
	Hyperparameter reg_strength(paramNames[0], bitsetPair(0, 0), bitsetPair(RS_MAX_VAL, RS_MAX_POW), bitsetPair(RS_MIN_VAL, RS_MIN_POW));
	Hyperparameter params[] = { learning_rate, dropout_prob, reg_strength };
	
	for (int i = 0; i < NUM_PARAMS; i++) {
		hyperparameters.insert(pair<string, Hyperparameter>(paramNames[i], params[i]));
	}

	//Generate our population size, sending false in the constructor to indicate random values must be made

	for (int i = 0; i < POPULATION_SIZE; i++) {
		Chromosome ch(hyperparameters, false);
		population.push_front(ch);
	}

	/*
	#############
	# MAIN LOOP #
	#############
	*/


	while (0) {

		Chromosome solutionChromosome(hyperparameters, false);	//Take these outside the loop lol
		bool solutionFound = false; //!!!!!!!!!!!!!!!!!!!!!!!!

		//Calculate fitness of each chromosome, calculate the population's total fitness for roulette
		
		for (list<Chromosome>::iterator it = population.begin(); it != population.end(); ++it) {
			it->CalculateFitness();
			TotalFitness += it->fitness;
			if (it->fitness >= FITNESS_SOLUTION) {
				solutionFound = true;
				solutionChromosome = *it;
			}
		}
		
		if (solutionFound) {
			break;
		}
	
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


Chromosome::Chromosome(map<string, Hyperparameter> _hyperparameters, bool isCrossover)
{
	hyperparameters = _hyperparameters;

	if (!(isCrossover)) {
		for (map<string, Hyperparameter>::iterator it = hyperparameters.begin(); it != hyperparameters.end(); ++it) {
			do {
				it->second.value = pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>>(random_bitset(), random_bitset());
			} while (!parseChromosomeValue(it->second));
		}
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
	//Interact with python script, perhaps have the python script save to a csv
	//Then use a module here to load the csv file
}	

/*
##############
# Crossover  #
##############
*/

void Crossover(Chromosome &offspring1, Chromosome &offspring2) {


	if (RAND_NUM < CROSSOVER_RATE) {

		for (int i = 0; i < NUM_PARAMS; i++) {

			//Convert our bits to strings for easy cross over
			string p1param1 = offspring1.hyperparameters[paramNames[i]].value.first.to_string();
			string p1param2 = offspring2.hyperparameters[paramNames[i]].value.second.to_string();
			string p2param1 = offspring1.hyperparameters[paramNames[i]].value.first.to_string();
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

			} while (!parseChromosomeValue(offspring1.hyperparameters[paramNames[i]]) || !parseChromosomeValue(offspring2.hyperparameters[paramNames[i]]));

		}
	}
}

/*
##############
# Mutation  #
##############
*/


void Mutation(Chromosome &chromosome)
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

		} while (!parseChromosomeValue(chromosome.hyperparameters[paramNames[i]]));
	}
}

/*
###################################
# Helper: Bitset Pair Generators  #
###################################
*/

pair<bitset<4>, bitset<4>> bitsetPair(unsigned int v, unsigned int p) {
	return 	pair<bitset<4>, bitset<4>>(bitset<4>(v), bitset<4>(p));
}

pair<bitset<4>, bitset<4>> bitsetPair(string v, string p) {
	return 	pair<bitset<4>, bitset<4>> (bitset<4>(v), bitset<4>(p));
}


/*
###################################
# Helper: Parse Chromosome Value  #
###################################
*/

bool parseChromosomeValue(Hyperparameter param) {
	float max = (float)(param.maximum.first.to_ulong() * pow(10, parsePow(param.maximum.second)));
	float min = (float)(param.minimum.first.to_ulong() * pow(10, parsePow(param.minimum.second)));
	float val = (float)(param.value.first.to_ulong() * pow(10, parsePow(param.value.second)));

	return (val <= max && val >= min);
}

bool parseChromosomeValue(Hyperparameter param, float &parsedValue) {
	float max = (float)(param.maximum.first.to_ulong() * pow(10, parsePow(param.maximum.second)));
	float min = (float)(param.minimum.first.to_ulong() * pow(10, parsePow(param.minimum.second)));
	parsedValue = (float)(param.value.first.to_ulong() * pow(10, parsePow(param.value.second)));

	return (parsedValue <= max && parsedValue >= min);
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

bitset<CHROM_LEN> random_bitset() {
	string result = "";

	for (int i = 0; i < CHROM_LEN; i++) {
		result += to_string((int)round(RAND_NUM));
	}
	return bitset<CHROM_LEN>(result);
}

Chromosome Roulette(float total_fitness, list<Chromosome> population) {

	float sliced_pos = (float)(RAND_NUM * total_fitness);
	float fitness_accumulated = 0.0f;

	for (list<Chromosome>::iterator it = population.begin(); it != population.end(); ++it) {
		fitness_accumulated += it->fitness;
		if (fitness_accumulated >= sliced_pos) {
			return *it;
		}
	}

	return population.front();
}