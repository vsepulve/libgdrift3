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

unsigned int Population::size(){
	return 0;
}

void Population::increase(unsigned int num){

}

void Population::decrease(unsigned int num){

}
	
void Population::add(Population *pop){
}





