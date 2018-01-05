#include "Pool.h"

Pool::Pool(){
}

Pool::Pool(Profile *profile){
	// Tomar el numero inicial de alelos para llenar el pool, por cada marcador
	
	for(unsigned int marker = 0; marker < profile->getNumMarkers(); ++marker){
		// Preparo un mapa para los alelos de ESTE marcador, y una variable para el id del alelo
		map<unsigned int, unsigned int> marker_map;
		unsigned int new_allele = 0;
		for(unsigned int i = 0; i <= profile->getMarker(marker).getInitialAlleles(); ++i){
			marker_map[++new_allele] = 0;
		}
		mutations_map.push_back(marker_map);
		next_allele.push_back(new_allele);
	}
	
}

Pool::~Pool(){
	for(unsigned int i = 0; i < mutations_map.size(); ++i){
		mutations_map[i].clear();
	}
	mutations_map.clear();
	next_allele.clear();
}

unsigned int Pool::getNewAllele(unsigned int marker_pos, unsigned int allele){
	
	unsigned int new_allele = ++(next_allele[marker_pos]);
	mutations_map[marker_pos][new_allele] = allele;
	
	return new_allele;
}
	
unsigned int Pool::getNumMarkers(){
	return mutations_map.size();
}
	
unsigned int Pool::getNumAlleles(unsigned int marker_pos){
	return mutations_map[marker_pos].size();
}

unsigned int Pool::getAllele(unsigned int marker, unsigned int pos){
	return mutations_map[marker][pos];
}
	
	
	
	
	






