using namespace std;

#include "string";
#include "map";
#include "utility"
#include "list"
#include "bitset";

#ifndef GENETiC_OPTIMIZATION
#define GENETiC_OPTIMIZATION

#define CHROM_LEN			8
#define MUTATION_RATE		0.001
#define CROSSOVER_RATE		0.7
#define POPULATION_SIZE		24
#define MAX_GENERATION		100
#define RAND_NUM			((float)rand()/(RAND_MAX+1))
#define FITNESS_SOLUTION	0.95

/*
	Each tuple <A,B> where A is the nuermical value ignoring the powers in standard notation
					and B is the numerical power, with one extra bit as a sign bit
*/

struct Hyperparameter {
	Hyperparameter();
	Hyperparameter(string name, pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> value, pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> maximum, pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> minimum);
	string name;
	pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> value;
	pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> maximum;
	pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> minimum;
};


class Chromosome
{
public:
	map<string, Hyperparameter> hyperparameters;
	float fitness;

	Chromosome();
	Chromosome(map<string, Hyperparameter> _hyperparameters);
	void CalculateFitness(string thread_title);
	void print();
};

bool Crossover(Chromosome &offspring1, Chromosome &offspring2);

void Mutate(Chromosome &chromosome);

pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> bitsetPair(unsigned int v, int p);

pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> bitsetPair(string v, string p);

bool VerifyParamValue(Hyperparameter param);

float parseChromosomeValue(Hyperparameter param);

int parsePow(bitset<CHROM_LEN> pow);

void ThreadedCalcFitness(Chromosome *subpopulation, string thread_title);

pair<bitset<CHROM_LEN>, bitset<CHROM_LEN>> random_bitset(Hyperparameter param);

Chromosome Roulette(float total_fitness, Chromosome *population);
#endif