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
	
//	vector< map<unsigned int, unsigned int> > mutations_map;
	
	// Version con vector posicionale en lugar de mapa
	// Aqui el id del alelo es directamente la posicion
	vector< vector<unsigned int> > mutations_map;
	
//	vector<unsigned int> next_allele;

public:

	Pool();
	Pool(Profile *profile);
	virtual ~Pool();
	
	unsigned int getNumMarkers();
	
	unsigned int getNewAllele(unsigned int marker_pos, unsigned int allele);
	
	unsigned int getParent(unsigned int marker_pos, unsigned int allele);
	
	unsigned int getNumAlleles(unsigned int marker_pos);
	
//	unsigned int getAllele(unsigned int marker, unsigned int pos);
	
//	unsigned int getNextAllele(unsigned int marker){
//		return next_allele[marker];
//	}
	
	// Metodo de debug
	void print(){
//		cout << "Pool::print - n_markers: " << mutations_map.size() << " (next_allele.size(): " << next_allele.size() << ")\n";
		cout << "Pool::print - n_markers: " << mutations_map.size() << "\n";
		for( unsigned int i = 0; i < mutations_map.size(); ++i ){
			cout << "Pool::print - mutations[" << i << "]: " << mutations_map[i].size() << "\n";
//			cout << "Pool::print - next_allele[" << i << "]: " << next_allele[i] << "\n";
		}
		cout << "Pool::print - End\n";
	}
	
};

#endif
