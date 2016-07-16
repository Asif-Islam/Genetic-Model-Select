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
	Chromosome(map<string, Hyperparameter> _hyperparameters, bool isCrossover);
	void CalculateFitness();
};

void Crossover(Chromosome c1, Chromosome c2, list<Chromosome> *newPopulation);

void Mutation(Chromosome *chromosome);

bool parseChromosomeValue(Hyperparameter param);

pair<bitset<4>, bitset<4>> bitsetPair(unsigned int v, unsigned int p);

pair<bitset<4>, bitset<4>> bitsetPair(string v, string p);

Chromosome Roulette(float total_fitness, list<Chromosome> population);
#endif