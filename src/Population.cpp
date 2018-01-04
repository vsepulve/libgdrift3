#include "Population.h"

Population::Population(){
	n_inds = 0;
	pool = new Pool();
}

Population::Population(unsigned int _n_inds){
	n_inds = _n_inds;
	pool = new Pool();
}

Population::Population(const Population &original){
	pool = new Pool();
}

Population& Population::operator=(const Population& original){
	if (this != &original){
		pool = new Pool();
	
	}
	return *this;
}

Population *Population::clone(){
	return new Population(*this);
}

Population::~Population(){
	if( pool != NULL ){
		delete pool;
		pool = NULL;
	}
}

unsigned int Population::size(){
	return n_inds;
}

void Population::increase(unsigned int num){
	// por ahora lo marco con n_inds
	n_inds += num;
}

void Population::decrease(unsigned int num){
	if( num > n_inds ){
		num = n_inds; 
	}
	// remover efectivamente los elementos
	// por ahora lo marco con n_inds
	n_inds -= num;
}
	
void Population::add(Population *pop, unsigned int num){
	if( num == 0 ){
		cout<<"Population::add - Moving every individual.\n";
		num = pop->size();
	}
	else if( num > pop->size() ){
		cerr<<"Population::add - Warning, num greater than population size (" << num << " > "<< pop->size() <<").\n";
		num = pop->size();
	}
	else{
		cout<<"Population::add - Moving " << num << " from " << pop->size() << ".\n";
	}
	// Copio los primeros num individuos aqui
	// Borro esos individuos (por ejemplo, pasando el resto a un nuevo vector y un swap)
	// por ahora simplemente lo marco con un decrease, pero esto es temporeal
	// TODO: Remover el decrease que sigue
	pop->decrease(num);
	
	
}

vector<Individual> &Population::getIndividuals(){
	return inds;
}

Individual &Population::get(unsigned int pos){
	return inds[pos];
}

Pool *Population::getPool(){
	return pool;
}













