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
		alleles[i].first = parent.getAllele(i);
	}
}

void Individual::setParent(Individual &parent_1, Individual &parent_2){
	// Copiar alelos
	// Model se encarga de la mutacion cuando sea necesario
	alleles.resize(parent_1.getNumMarkers());
	for(unsigned int i = 0; i < alleles.size(); ++i){
		alleles[i].first = parent_1.getAllele(i);
		alleles[i].second = parent_2.getAllele(i);
	}
}
	
unsigned int Individual::getNumMarkers(){
	return alleles.size();
}

unsigned int Individual::getAllele(unsigned int pos){
	return alleles[pos].first;
}
	
unsigned int Individual::getAllele(unsigned int pos, unsigned int ploidy_pos){
	if(ploidy_pos == 0){
		return alleles[pos].first;
	}
	else if(ploidy_pos == 1){
		return alleles[pos].second;
	}
	else{
		cerr << "Individual::getAllele - Ploidy not supported (" << ploidy_pos << ")\n";
		return 0;
	}
}

void Individual::setAllele(unsigned int pos, unsigned int allele){
	alleles[pos].first = allele;
}

void Individual::setAllele(unsigned int pos, unsigned int ploidy_pos, unsigned int allele){
//	cout << "Individual::setAllele - alleles[" << pos << "][" << ploidy_pos << "] = " << allele << "\n";
	if(ploidy_pos == 0){
		alleles[pos].first = allele;
	}
	else if(ploidy_pos == 1){
		alleles[pos].second = allele;
	}
	else{
		cerr << "Individual::setAllele - Ploidy not supported (" << ploidy_pos << ")\n";
	}
}

void Individual::prepare(Profile *profile){
	alleles.clear();
	alleles.resize(profile->getNumMarkers());
	// Aqui podria ser necesario resetear OTROS datos dependiendo del tipo de sequencia quizas
	// Quizas esa informacion pueda estar en el Pool, eso si, quizas si basta con el id del alelo
}












