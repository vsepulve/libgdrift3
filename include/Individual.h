#ifndef _INDIVIDUAL_H_
#define _INDIVIDUAL_H_

#include "Profile.h"

using namespace std;

class Individual{

private:
	vector<unsigned int> alleles;

public:

	Individual();
//	Individual(const Individual &original);
//	Individual& operator=(const Individual& original);
//	virtual Individual *clone();
	virtual ~Individual();
	
	void setParent(Individual &parent);
	
	unsigned int getNumMarkers();
	
	unsigned int getAllele(unsigned int pos);
	
	void setAllele(unsigned int pos, unsigned int allele);
	
	void prepare(Profile *profile);
	
};

#endif
