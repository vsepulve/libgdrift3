#ifndef _POOL_H_
#define _POOL_H_

#include <map>
#include <vector>
#include <random>
#include <assert.h>

#include "Profile.h"

using namespace std;

class Pool{

private:
	
	vector< map<unsigned int, unsigned int> > mutations_map;
	vector<unsigned int> next_allele;

public:

	Pool();
	Pool(Profile *profile);
	virtual ~Pool();
	
	unsigned int getNumMarkers();
	
	unsigned int getNewAllele(unsigned int marker_pos, unsigned int allele);
	
	unsigned int getParent(unsigned int marker_pos, unsigned int allele);
	
	unsigned int getNumAlleles(unsigned int marker_pos);
	
	unsigned int getAllele(unsigned int marker, unsigned int pos);
	
	unsigned int getNextAllele(unsigned int marker){
		return next_allele[marker];
	}
	
};

#endif
