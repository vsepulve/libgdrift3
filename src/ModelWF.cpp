#include "ModelWF.h"

ModelWF::ModelWF(){
//	cout<<"ModelWF - Creating new ModelWF\n";
	type = 1;
}

ModelWF::ModelWF(const ModelWF &original){
	type = original.type;
}

ModelWF& ModelWF::operator=(const ModelWF& original){
	if (this != &original){
		type = original.type;
	
	}
	return *this;
}

Model *ModelWF::clone(){
	return (Model*)(new ModelWF(*this));
}

ModelWF::~ModelWF(){
	
}

void ModelWF::run(Population *population, Profile *profile, mt19937 &generator){
	
//	cout<<"ModelWF::run - Iterando por "<<population->size()<<" individuos\n";
	
	// Notar que model requiere un dst para guardar los hijos de la nueva generacion
	// La estructura debe ser la misma que la interna de population, o una population efectiva
	// Por ahora, usare un vector explicito, asumiendo vector en population y un swap al final
	// Notar que el resize deberia ser eficiente si se procesa una misma poblacion varias generaciones antes de pasar a la siguiente
	
	dst.resize( population->size() );
	
//	NanoTimer timer;
	unsigned int mutations = 0;
	uniform_int_distribution<> parent_dist(0, population->size() - 1);
	
	// Fase de Recombinacion
	if(profile->getPloidy() == 1){
		// Posicion del padre elegido para el individuo de dst
		unsigned int parent = 0;
		// Creando nueva generacion
		for(unsigned int id = 0; id < dst.size(); ++id){
			parent = parent_dist(generator);
			dst[id].setParent(population->get(parent));
		}
	}
	else if(profile->getPloidy() == 2){
		unsigned int parent_1 = 0;
		unsigned int parent_2 = 0;
		// Creando nueva generacion
		for(unsigned int id = 0; id < dst.size(); ++id){
			parent_1 = parent_dist(generator);
			parent_2 = parent_dist(generator);
			dst[id].setParent(population->get(parent_1), population->get(parent_2));
		}
	}
	else{
		cerr << "ModelWF::run - Ploidy " << profile->getPloidy() << " not implemented.\n";
		exit(EXIT_FAILURE);
	}
	
	// Fase de Mutaciones
	// Siguiendo el modelo 2.0, aqui habria que iterar por los genes o marcadores geneticos
	// El procesamiento de cada gen se realiza en orden para no iterar por individuo, sino por mutaciones
	
//	cout<<"ModelWF::run - Procesando mutaciones\n";
	
	for(unsigned int i = 0; i < profile->getNumMarkers(); ++i){
		ProfileMarker marker = profile->getMarker(i);
		if( marker.getType() == MARKER_SEQUENCE ){
			mutations += processDNAGenes(i, marker, population->getPool(), generator);
		}
		else if( marker.getType() == MARKER_MS ){
			mutations += processMSGenes(i, marker, profile->getPloidy(), population->getPool(), generator);
		}
		else{
			cerr << "ModelWF::run - Genetic Marker not implemented.\n";
		}
	}
	
	population->getIndividuals().swap(dst);
	
//	cout<<"ModelWF::run - Terminado ("<<timer.getMilisec()<<" ms, mutations: "<<mutations<<")\n";
	
}

unsigned int ModelWF::processDNAGenes(unsigned int marker_pos, ProfileMarker &marker, Pool *pool, mt19937 &generator){
	
	assert(marker.getLength() > 0);
	assert(marker.getMutationType() == MUTATION_BASIC);
	assert(marker.getNumParam() == 1);
	
	uniform_int_distribution<> individuals_dist(0, dst.size() - 1);
	
	unsigned int mutations = 0;

	double rate = marker.getParam(0);
	unsigned long long length = marker.getLength();

	binomial_distribution<unsigned long long> binomial_dist(length * dst.size(), rate);
	unsigned int total_muts = static_cast<unsigned int>(binomial_dist(generator));

//	cout << "ModelWF::processDNAGenes - total_muts: " << total_muts << " (" << (length * dst.size()) << ", " << rate << ")\n";
	for(unsigned int mut = 0; mut < total_muts; ++mut){
		// Escoger individuo para mutar
		unsigned int individual_pos = individuals_dist(generator);
//		cout<<"ModelWF::processDNAGenes - mut_pos: " << individual_pos << " de " << dst.size() << "\n";
		
		// Crear nuevo alelo del marcador marker_pos del individuo individual_pos
		unsigned int new_allele = pool->getNewAllele(marker_pos, dst[individual_pos].getAllele(marker_pos));
		dst[individual_pos].setAllele(marker_pos, new_allele);
		++mutations;

	}
	
//	cout << "ModelWF::processDNAGenes - Fin\n";
	
	return mutations;	
}

unsigned int ModelWF::processMSGenes(unsigned int marker_pos, ProfileMarker &marker, unsigned int ploidy, Pool *pool, mt19937 &generator){
	
	assert(ploidy > 0);
	assert(marker.getMutationType() == MUTATION_BASIC);
	// Lo que sigue puede cambiar si se agregan parametros adicionales
	assert(marker.getNumParam() == 1);
	
	unsigned int mutations = 0;
	double rate = marker.getParam(0);
	
	// Notar que considero population_size x plody (es decir, que cada alelo recibido puede haber mutado por separado)
	binomial_distribution<unsigned long long> binomial_dist(dst.size() * ploidy, rate);
	unsigned int total_muts = static_cast<unsigned int>(binomial_dist(generator));
	
	uniform_int_distribution<> individuals_dist(0, dst.size() - 1);
	uniform_int_distribution<> ploidy_dist(0, ploidy - 1);
	uniform_int_distribution<> sum_dist(0, 1);
	geometric_distribution<> repeats_dist(0.5);
	
//	cout << "ModelWF::processMSGenes - total_muts: " << total_muts << " (" << dst.size() << ", " << rate << ")\n";
	for(unsigned int mut = 0; mut < total_muts; ++mut){
		// Escoger individuo para mutar
		unsigned int individual_pos = individuals_dist(generator);
		unsigned int ploidy_pos = ploidy_dist(generator);
		bool sum = (sum_dist(generator) == 1);
		unsigned int mod_repeats = 1 + repeats_dist(generator);
//		cout<<"ModelWF::processMSGenes - mut_pos: " << individual_pos << " / " << dst.size() << ", ploidy_pos: " << ploidy_pos << ", sum: " << sum << ", mod_repeats: " << mod_repeats << "\n";
		
		// Crear nuevo alelo del marcador marker_pos del individuo individual_pos
		unsigned int new_allele = dst[individual_pos].getAllele(marker_pos, ploidy_pos);
//		cout<<"ModelWF::processMSGenes - old_allele: " << new_allele << "\n";
		if( sum ){
			new_allele += mod_repeats;
		}
		else if( new_allele > mod_repeats ){
			new_allele -= mod_repeats;
		}
		else{
			new_allele = 1;
		}
//		cout<<"ModelWF::processMSGenes - new_allele: " << new_allele << "\n";
		dst[individual_pos].setAllele(marker_pos, ploidy_pos, new_allele);
		++mutations;
	}
	
//	cout << "ModelWF::processMSGenes - Fin\n";
	
	return mutations;	
}
















