#ifndef _INDIVIDUAL_H_
#define _INDIVIDUAL_H_

#include "Profile.h"

using namespace std;

class Individual{

private:
//	vector<unsigned int> alleles;
	vector<pair<unsigned int, unsigned int>> alleles;

public:

	Individual();
//	Individual(const Individual &original);
//	Individual& operator=(const Individual& original);
//	virtual Individual *clone();
	virtual ~Individual();
	
	void setParent(Individual &parent);
	
	void setParent(Individual &parent_1, Individual &parent_2);
	
	unsigned int getNumMarkers();
	
	unsigned int getAllele(unsigned int pos);
	
	unsigned int getAllele(unsigned int pos, unsigned int ploidy_pos);
	
	void setAllele(unsigned int pos, unsigned int allele);
	
	void setAllele(unsigned int pos, unsigned int ploidy_pos, unsigned int allele);
	
	void prepare(Profile *profile);
	
};

#endif
