#include "ModelWF.h"

ModelWF::ModelWF(){
}

ModelWF::ModelWF(const ModelWF &original){
}

ModelWF& ModelWF::operator=(const ModelWF& original){
	if (this != &original){
	
	}
	return *this;
}

Model *ModelWF::clone(){
	return (Model*)(new ModelWF(*this));
}

ModelWF::~ModelWF(){
	
}

void ModelWF::run(Population *population, Profile *profile, mt19937 &generator){
	
	cout<<"ModelWF::run - Iterando por "<<population->size()<<" individuos\n";
	
	// Notar que model requiere un dst para guardar los hijos de la nueva generacion
	// La estructura debe ser la misma que la interna de population, o una population efectiva
	// Por ahora, usare un vector explicito, asumiendo vector en population y un swap al final
	// Notar que el resize deberia ser eficiente si se procesa una misma poblacion varias generaciones antes de pasar a la siguiente
	
	dst.resize( population->size() );
	
	NanoTimer timer;
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
//		cout<<"ModelWF::run - Padres asignados en "<<timer.getMilisec()<<" ms\n";
	}
	else{
		cerr<<"ModelWF::run - Ploidy "<<profile->getPloidy()<<" not implemented.\n";
		exit(EXIT_FAILURE);
	}
	
	// Fase de Mutaciones
	// Siguiendo el modelo 2.0, aqui habria que iterar por los genes o marcadores geneticos
	// El procesamiento de cada gen se realiza en orden para no iterar por individuo, sino por mutaciones
	
	cout<<"ModelWF::run - Procesando mutaciones\n";
	
	for(unsigned int i = 0; i < profile->getNumMarkers(); ++i){
		ProfileMarker marker = profile->getMarker(i);
		if( marker.getType() == MARKER_SEQUENCE ){
//			mutations += processDNAGenes(i, pool, profile);
			mutations += processDNAGenes(i, marker, population->getPool(), generator);
		}
		else{
			cerr<<"ModelWF::run - Mutation model not implemented.\n";
		}
	}
	
	population->getIndividuals().swap(dst);
	
	cout<<"ModelWF::run - Terminado ("<<timer.getMilisec()<<" ms, mutations: "<<mutations<<")\n";
	
}

//unsigned int ModelWF::processDNAGenes(Population *dst, Pool *pool, Individual::Profile *profile, unsigned int plo, unsigned int chrid, unsigned int genid){
unsigned int ModelWF::processDNAGenes(unsigned int marker_pos, ProfileMarker &marker, Pool *pool, mt19937 &generator){
	
	assert(marker.getLength() > 0);
	assert(marker.getMutationType() == MUTATION_BASIC);
	assert(marker.getNumParam() == 1);
	
	uniform_int_distribution<> individuals_dist(0, dst.size() - 1);
//	uniform_int_distribution<> sequence_dist(0, marker.getLength() - 1);
	
	unsigned int mutations = 0;
	
	
//	double rate = gen->mutation_rate();
//	cout<<"ModelWF::processDNAGenes - Iniciando mutacion de chr "<<chrid<<", gen "<<genid<<"\n";
	double rate = marker.getParam(0);
//	cout<<"ModelWF::processDNAGenes - rate: "<<rate<<"\n";
	unsigned long long length = marker.getLength();
//	cout<<"ModelWF::processDNAGenes - length: "<<length<<"\n";
	// Calcular el total de mutaciones que se realizaran para este gen en TODA la poblacion
	// Notar que se puede hacer algo mas sofisticado aqui
	// Por ahora, solo multiplico la probabilidad de mutacion, por el largo del gen, por toda la poblacion
//	unsigned int total_muts = (unsigned int)(rate * length * dst->size());
	// Version binomial (Notar que considero rate como la prob de mutacion POR NUCLEOTIDO de cada individuo)
	binomial_distribution<unsigned long long> binomial_dist(length * dst.size(), rate);
	unsigned int total_muts = static_cast<unsigned int>(binomial_dist(generator));
//	pair<uint32_t, uint32_t> par(chrid, genid);
//	unsigned int reusados = 0;
//	cout<<"ModelWF::processDNAGenes - total_muts: "<<total_muts<<" ("<<(length * dst->size())<<", "<<rate<<", pool->reuse: "<<pool->reuse[par].size()<<")\n";
	for(unsigned int mut = 0; mut < total_muts; ++mut){
		// Escoger individuo para mutar
		unsigned int individual_pos = individuals_dist(generator);
//		cout<<"ModelWF::processDNAGenes - mut_pos: "<<mut_pos<<" de "<<dst->size()<<"\n";
		
		// Crear nuevo alelo del marcador marker_pos del individuo individual_pos
		unsigned int new_allele = pool->getNewAllele(marker_pos, dst[individual_pos].getAllele(marker_pos));
		dst[individual_pos].setAllele(marker_pos, new_allele);
		++mutations;
		
		/*
		// Notar que usar el pool de este modo elimina la posibilidad de que se apliquen dos mutaciones al mismo individuo
		if( pool->reuse[par].size() > 0 ){
			seq = pool->reuse[par].back();
			pool->reuse[par].pop_back();
			++reusados;
		}
		else{
			// Creo una nueva secuencia con el gen actual del individuo para mutar y reemplazar
			VirtualSequence *original = dst->at(mut_pos).getGene(genid, chrid, plo);
//			cout<<"ModelWF::processDNAGenes - Crando secuencia para mutar (original NULL? "<<(original==NULL)<<", size: "<<((original!=NULL)?original->length():0)<<")\n";
//			seq = new VirtualSequence(*original);
			seq = original->clone();
		}
		
		
//		cout<<"ModelWF::processDNAGenes - mutate...\n";
		seq->mutate(rng);
		++mutations;
//		cout<<"ModelWF::processDNAGenes - push...\n";
		pool->push(chrid, genid, seq);
//		cout<<"ModelWF::processDNAGenes - setGene...\n";
		dst->at(mut_pos).setGene(genid, chrid, 0, seq);
//		cout<<"ModelWF::processDNAGenes - Fin\n";
		*/

	}
	
//	cout<<"ModelWF::processDNAGenes - Fin (reusados: "<<reusados<<" de "<<total_muts<<", pool->reuse: "<<pool->reuse[par].size()<<")\n";
	
	return mutations;	
}
















