#ifndef _MODEL_H_
#define _MODEL_H_

#include <map>
#include <vector>
#include <random>
#include <assert.h>

#include "NanoTimer.h"
#include "Population.h"
#include "Profile.h"
#include "Individual.h"

using namespace std;

class Model{

private:

public:

	Model();
	Model(const Model &original);
	Model& operator=(const Model& original);
	virtual Model *clone();
	virtual ~Model();
	
	void run(Population *population, Profile *profile, mt19937 &generator);
	
};

#endif
