#ifndef _POOL_H_
#define _POOL_H_

#include <map>
#include <vector>
#include <random>
#include <assert.h>

using namespace std;

class Pool{

private:

public:

	Pool();
	virtual ~Pool();
	
	unsigned int getNewAllele(unsigned int allele);
	
};

#endif
