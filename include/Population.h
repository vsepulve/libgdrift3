#ifndef _POPULATION_H_
#define _POPULATION_H_

#include <map>
#include <vector>
#include <random>
#include <algorithm>

#include "NanoTimer.h"
#include "Individual.h"
#include "Pool.h"

using namespace std;

class Population{

private:
//	unsigned int n_inds;
	vector<Individual> inds;
	Pool *pool;
	// Notar que el profile es de la simulation por ahora
	Profile *profile;

public:

	Population();
	Population(unsigned int _n_inds, Profile *_profile, mt19937 &generator);
//	Population(const Population &original);
//	Population& operator=(const Population& original);
//	virtual Population *clone();
	virtual ~Population();
	
	unsigned int size();
	
	void increase(unsigned int num, mt19937 &generator);
	
	void decrease(unsigned int num, mt19937 &generator);
	
	void add(Population *pop, unsigned int num, mt19937 &generator);
	
	vector<Individual> &getIndividuals();
	
	Individual &get(unsigned int pos);
	
	Pool *getPool();
	
};

#endif
