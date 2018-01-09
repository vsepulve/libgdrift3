#include "Population.h"

Population::Population(){
	
	profile = NULL;
	pool = NULL;
}

Population::Population(unsigned int _n_inds, Profile *_profile, Pool *_pool, mt19937 &generator){
	assert(_profile != NULL);
	assert(_pool != NULL);

	cout << "Population - Start\n";
	
	profile = _profile;
//	pool = new Pool(profile);
	pool = _pool;
	inds.resize(_n_inds);
	
	cout << "Population - Preparing Distributions\n";
	vector< uniform_int_distribution<> > alleles_dist;
	
//	for(unsigned int marker = 0; marker < pool->getNumMarkers(); ++marker){
	for(unsigned int marker = 0; marker < profile->getNumMarkers(); ++marker){
//		uniform_int_distribution<> marker_dist(0, pool->getNumAlleles(marker) - 1);
		// En lugar del numero de alelos actual del pool, usare el un rand en el numero inicial de profile
		// De este modo, una nueva poblacion también tendra alelos de ese pool inicial
		uniform_int_distribution<> marker_dist(0, profile->getMarker(marker).getInitialAlleles() - 1);
		alleles_dist.push_back(marker_dist);
	}
	
	cout<<"Population - Randomizing Individuals\n";
	for(unsigned int i = 0; i < inds.size(); ++i){
		inds[i].prepare(profile);
		for(unsigned int marker = 0; marker < profile->getNumMarkers(); ++marker){
			// escogo al azar un alelo para este marcador
			unsigned int pos = alleles_dist[marker](generator);
			inds[i].setAllele(marker, pool->getAllele(marker, pos));
		}
	}
	cout << "Population - End\n";
}

//Population::Population(const Population &original){
//	pool = new Pool();
//}

//Population& Population::operator=(const Population& original){
//	if (this != &original){
//		pool = new Pool();
//	
//	}
//	return *this;
//}

//Population *Population::clone(){
//	return new Population(*this);
//}

Population::~Population(){
	// Ahora el pool es de la simulacion
//	if( pool != NULL ){
//		delete pool;
//		pool = NULL;
//	}
	pool = NULL;
	// profile es de la simulacion
	profile = NULL;
}

unsigned int Population::size(){
	return inds.size();
}

void Population::increase(unsigned int num, mt19937 &generator){
	// por ahora lo marco con n_inds
//	n_inds += num;
	uniform_int_distribution<> parent_dist(0, inds.size() - 1);
	unsigned int parent;
	for(unsigned int i = 0; i < num; ++i){
		parent = parent_dist(generator);
		Individual new_ind;
		new_ind.setParent(inds[parent]);
		inds.push_back(new_ind);
	}
}

void Population::decrease(unsigned int num, mt19937 &generator){
	if( num > inds.size() ){
		num = inds.size(); 
	}
	// remover efectivamente los elementos
	// por ahora lo marco con n_inds
//	n_inds -= num;
	shuffle(inds.begin(), inds.end(), generator);
	for(unsigned int i = 0; i < num; ++i){
		inds.pop_back();
	}
}
	
void Population::add(Population *pop, unsigned int num, mt19937 &generator){
	if( num > pop->size() ){
		cerr<<"Population::add - Warning, num greater than population size.\n";
		num = pop->size();
	}
	cout<<"Population::add - Moving " << num << " from " << pop->size() << ".\n";
	
	// Copio los primeros num individuos aqui
	// Borro esos individuos (por ejemplo, pasando el resto a un nuevo vector y un swap)
	// por ahora simplemente lo marco con un decrease, pero esto es temporeal
	// TODO: Remover el decrease que sigue
//	pop->decrease(num);
	shuffle(pop->getIndividuals().begin(), pop->getIndividuals().end(), generator);
	for(unsigned int i = 0; i < num; ++i){
		inds.push_back( pop->getIndividuals().back() );
		pop->getIndividuals().pop_back();
	}
	
}
	
void Population::add(Individual &individual){
	inds.push_back(individual);
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













