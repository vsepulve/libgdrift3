#ifndef _INDIVIDUAL_H_
#define _INDIVIDUAL_H_

using namespace std;

class Individual{

private:

public:

	Individual();
	Individual(const Individual &original);
	Individual& operator=(const Individual& original);
	virtual Individual *clone();
	virtual ~Individual();
	
	void setParent(Individual &parent);
	
	unsigned int getAllele(unsigned int pos);
	
	void setAllele(unsigned int pos, unsigned int allele);
	
};

#endif
