using namespace std;

#include "string";
#include "map";
#include "utility"
#include "list"
#include "bitset";

#ifndef GENETiC_OPTIMIZATION
#define GENETiC_OPTIMIZATION


/*
	Each tuple <A,B> where A is the nuermical value ignoring the powers in standard notation
					and B is the numerical power, with one extra bit as a sign bit
*/

struct Hyperparameter {
	Hyperparameter();
	Hyperparameter(string name, pair<bitset<4>, bitset<4>> value, pair<bitset<4>, bitset<4>> maximum, pair<bitset<4>, bitset<4>> minimum);
	string name;
	pair<bitset<4>, bitset<4>> value;
	pair<bitset<4>, bitset<4>> maximum;
	pair<bitset<4>, bitset<4>> minimum;
};


class Chromosome
{
public:
	map<string, Hyperparameter> hyperparameters;
	float fitness;

	Chromosome();
	Chromosome(map<string, Hyperparameter> _hyperparameters);
	void CalculateFitness();
};

bool Crossover(Chromosome &offspring1, Chromosome &offspring2);

void Mutate(Chromosome &chromosome);

pair<bitset<4>, bitset<4>> bitsetPair(unsigned int v, int p);

pair<bitset<4>, bitset<4>> bitsetPair(string v, string p);

Chromosome Roulette(float total_fitness, list<Chromosome> population);

bool VerifyParamValue(Hyperparameter param);

float parseChromosomeValue(Hyperparameter param);

int parsePow(bitset<4> pow);

pair<bitset<4>, bitset<4>> random_bitset(Hyperparameter param);

Chromosome Roulette(float total_fitness, Chromosome *population);
#endif