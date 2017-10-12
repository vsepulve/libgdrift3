#ifndef _POPULATION_H_
#define _POPULATION_H_

#include <map>

#include "NanoTimer.h"

using namespace std;

class Population{

private:

public:

	Population();
	Population(unsigned int size);
	Population(const Population &original);
	Population& operator=(const Population& original);
	virtual Population *clone();
	virtual ~Population();
	
	
	
};

#endif
