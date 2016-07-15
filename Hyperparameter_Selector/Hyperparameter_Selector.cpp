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

#define RAND_NUM			((float)rand()/(RAND_MAX+1))


string paramNames[] = { "Learning Rate", "Dropout Probability", "Regularization Strength" };
/*
##############
# TO DO LIST #
##############

(2) Implement Crossover
(3) Implement Mutation
(4) Determine a novel weighted probability method of choosing elitsts oh roulette wait a second . . .
(5) Profit ?? -> Write code to create offsprings until we reach a certain count, then use elitst

*/
int main()
{
	bitset<CHROM_LEN> bits[2];
	bits[0] = bitset<CHROM_LEN>("0001");
	bits[1] = bitset<CHROM_LEN>("0101");
	cout << bits[1] << "----" << endl;
	cout << bits[0] << endl << bits[1] << endl;
	cout << (bits[0] ^ bits[1]) << endl;
	cout << bits[0] << endl << bits[1] << endl;
	(bits[0] ^= bitset<CHROM_LEN>("0101"));
	cout << bits[0] << endl << bits[1] << endl;
	system("PAUSE");
	
	


	srand((int)time(NULL));

	/* Initialization:
		map of hyperparameters pairing the param name to a struct containing information about:
			It's value (As assumed by a chrosome that owns it), it's max value and min value
	*/
	
	map<string, Hyperparameter> hyperparameters;
	list<Chromosome> population;
	float TotalFitness = 0.0f;



	Hyperparameter learning_rate(paramNames[0], bitsetPair(0,0), bitsetPair(0,0), bitsetPair(0,0));
	Hyperparameter dropout_prob(paramNames[0], bitsetPair(0, 0), bitsetPair(0, 0), bitsetPair(0, 0));
	Hyperparameter reg_strength(paramNames[0], bitsetPair(0, 0), bitsetPair(0, 0), bitsetPair(0, 0));
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


		//Begin the process of creating our new population
	
	}


	//Either call a Roulette function here OR
	//Use some weighted probability ftn to choose two chromosomes!

} //END



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

Chromosome::Chromosome(map<string, Hyperparameter> _hyperparameters, bool isCrossover)
{
	hyperparameters = _hyperparameters;

	if (!(isCrossover)) {
		for (map<string, Hyperparameter>::iterator it = hyperparameters.begin(); it != hyperparameters.end(); ++it) {
			//Use randomization to produce a number between max and min VAL
			//Use randomization to produce a number betwee max and min POW
		}
	}

}

Chromosome::Chromosome() { }

void Chromosome::CalculateFitness()
{
	//Interact with python script, perhaps have the python script save to a csv
	//Then use a module here to load the csv file
}	

void Crossover(Chromosome &offpsring1, Chromosome &offspring2) {


	if (RAND_NUM < CROSSOVER_RATE) {

		for (int i = 0; i < NUM_PARAMS; i++) {

			//Convert our bits to strings for easy cross over
			string p1param1 = offpsring1.hyperparameters[paramNames[i]].value.first.to_string();
			string p1param2 = offspring2.hyperparameters[paramNames[i]].value.second.to_string();
			string p2param1 = offpsring1.hyperparameters[paramNames[i]].value.first.to_string();
			string p2param2 = offspring2.hyperparameters[paramNames[i]].value.second.to_string();

			//Crossover positions
			int crossover_pos1 = (int)round(RAND_NUM * CHROM_LEN);
			int crossover_pos2 = (int)round(RAND_NUM * CHROM_LEN);

			//Param1 crossover
			string n1param1 = p1param1.substr(0, crossover_pos1) + p2param1.substr(crossover_pos1, CHROM_LEN);
			string n1param2 = p1param2.substr(0, crossover_pos2) + p2param2.substr(crossover_pos2, CHROM_LEN);

			//Param2 crossover
			string n2param1 = p2param1.substr(0, crossover_pos1) + p1param1.substr(crossover_pos1, CHROM_LEN);
			string n2param2 = p2param2.substr(0, crossover_pos2) + p1param2.substr(crossover_pos2, CHROM_LEN);

			offpsring1.hyperparameters[paramNames[i]].value = bitsetPair(n1param1, n1param2);
			offspring2.hyperparameters[paramNames[i]].value = bitsetPair(n2param1, n2param2);
		}
	}
}

void Mutation(Chromosome &chromosome)
{

	for (int i = 0; i < NUM_PARAMS; i++) {
		string flip_operator[] = { "0000", "0000" };
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < CHROM_LEN; k++) {

				if (RAND_NUM < MUTATION_RATE) {
					flip_operator[j].at(k) = '1';
				}
			}
		}
		(chromosome.hyperparameters[paramNames[i]].value.first ^= bitset<CHROM_LEN>(flip_operator[0]));
		(chromosome.hyperparameters[paramNames[i]].value.second ^= bitset<CHROM_LEN>(flip_operator[1]));
	}
}

//Make p unsigned, do logic to if negative to produce a signed bit
pair<bitset<4>, bitset<4>> bitsetPair(unsigned int v, unsigned int p) {
	return 	pair<bitset<4>, bitset<4>>(bitset<4>(v), bitset<4>(p));
}

pair<bitset<4>, bitset<4>> bitsetPair(string v, string p) {
	return 	pair<bitset<4>, bitset<4>> (bitset<4>(v), bitset<4>(p));
}
