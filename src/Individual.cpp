#include "Individual.h"

Individual::Individual(){
}

//Individual::Individual(const Individual &original){
//}

//Individual& Individual::operator=(const Individual& original){
//	if (this != &original){
//	
//	}
//	return *this;
//}

//Individual *Individual::clone(){
//	return new Individual(*this);
//}

Individual::~Individual(){
	
}

void Individual::setParent(Individual &parent){
	// Copiar alelos
	// Model se encarga de la mutacion cuando sea necesario
	alleles.resize(parent.getNumMarkers());
	for(unsigned int i = 0; i < alleles.size(); ++i){
		alleles[i] = parent.getAllele(i);
	}
}
	
unsigned int Individual::getNumMarkers(){
	return alleles.size();
}
	
unsigned int Individual::getAllele(unsigned int pos){
	return alleles[pos];
}

void Individual::setAllele(unsigned int pos, unsigned int allele){
	alleles[pos] = allele;
}

void Individual::prepare(Profile *profile){
	alleles.clear();
	alleles.resize(profile->getNumMarkers());
	// Aqui podria ser necesario resetear OTROS datos dependiendo del tipo de sequencia quizas
	// Quizas esa informacion pueda estar en el Pool, eso si, quizas si basta con el id del alelo
}












