#include "Individual.h"

Individual::Individual(){
}

Individual::Individual(const Individual &original){
}

Individual& Individual::operator=(const Individual& original){
	if (this != &original){
	
	}
	return *this;
}

Individual *Individual::clone(){
	return new Individual(*this);
}

Individual::~Individual(){
	
}

void Individual::setParent(Individual &parent){
	// Copiar alelos
	// Model se encarga de la mutacion cuando sea necesario
}
	
unsigned int Individual::getAllele(unsigned int pos){
	return 0;
}

void Individual::setAllele(unsigned int pos, unsigned int allele){
	
}
