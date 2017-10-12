#include "Population.h"

Population::Population(){
}

Population::Population(unsigned int size){
}

Population::Population(const Population &original){
}

Population& Population::operator=(const Population& original){
	if (this != &original){
	
	}
	return *this;
}

Population *Population::clone(){
	return new Population(*this);
}

Population::~Population(){
	
}


