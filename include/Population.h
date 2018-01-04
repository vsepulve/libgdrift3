#ifndef _POPULATION_H_
#define _POPULATION_H_

#include <map>
#include <vector>

#include "NanoTimer.h"
#include "Individual.h"
#include "Pool.h"

using namespace std;

class Population{

private:
	unsigned int n_inds;
	vector<Individual> inds;
	Pool *pool;

public:

	Population();
	Population(unsigned int _n_inds);
	Population(const Population &original);
	Population& operator=(const Population& original);
	virtual Population *clone();
	virtual ~Population();
	
	unsigned int size();
	
	void increase(unsigned int num);
	
	void decrease(unsigned int num);
	
	void add(Population *pop, unsigned int num = 0);
	
	vector<Individual> &getIndividuals();
	
	Individual &get(unsigned int pos);
	
	Pool *getPool();
	
};

#endif
