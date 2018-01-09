#include "Statistics.h"

Statistics::Statistics(){
}

Statistics::Statistics(Simulator *sim, float sampling)
: generator((std::random_device())()) {
	assert(sim != NULL);
	assert(sampling >= 0.0);
	assert(sampling <= 1.0);
	
	cout << "Statistics - Start\n";
	
	pool = sim->getPool();
	profile = sim->getProfile();
	
	Population summary(0, profile, pool, generator);
	
	vector<string> pop_names = sim->getPopulationNames();
	
	cout << "Statistics - Processing " << pop_names.size() << " populations\n";
	for(string name : pop_names){
		cout << "Statistics - Population " << name << "\n";
		Population *pop = sim->getPopulation(name);
		processStatistics(pop, name, sampling);
		
		cout << "Statistics - Adding individuals to summary\n";
		for(unsigned int i = 0; i < pop->size(); ++i){
			summary.add( pop->get(i) );
		}
	}
	
	cout << "Statistics - Processing the combined population\n";
	processStatistics(&summary, "summary", sampling);
	
	cout << "Statistics - End\n";
}
	
//Statistics::Statistics(const Statistics &original){
//}

//Statistics& Statistics::operator=(const Statistics& original){
//	if (this != &original){
//	
//	}
//	return *this;
//}

//Statistics *Statistics::clone(){
//	return new Statistics(*this);
//}

Statistics::~Statistics(){
	
}

	
// Procesa TODOS los estadisticos y los agrega a statistics[name][stat]
void Statistics::processStatistics(Population *pop, string name, float sampling){
	
	cout << "Statistics::processStatistics - Start (population " << name << ", sampling " << sampling << ")\n";
	
	unsigned int n_inds = pop->size() * sampling;
	if(n_inds < min_sampling){
		n_inds = min_sampling;
	}
	if(n_inds > pop->size()){
		n_inds = pop->size();
	}
	
	// Necesito un shuffle de la poblacion
	// Para simularlo, puedo desordenar las posiciones
	cout << "Statistics::processStatistics - Preparing selected individuals (n_inds: " << n_inds << ")\n";
	vector<unsigned int> inds_usados;
	for(unsigned int i = 0; i < pop->size(); ++i){
		inds_usados.push_back(i);
	}
	shuffle(inds_usados.begin(), inds_usados.end(), generator);
	
	// Para esta poblacion:
	// Iterar por los marcadores
	// Para cada marcador, calcular SU mapa de estadisticos y agregarlo
	
	vector<map<string, double>> stats_vector;
	cout << "Statistics::processStatistics - Processing " << profile->getNumMarkers() << " markers\n";
	for(unsigned int pos_marker = 0; pos_marker < profile->getNumMarkers(); ++pos_marker){
		map<string, double> stats;
		ProfileMarker marker = profile->getMarker(pos_marker);
		
		cout << "Statistics::processStatistics - Preparing String Vector\n";
		vector<string> alleles;
		for(unsigned int ind = 0; ind < n_inds; ++ind){
			unsigned int pos_ind = inds_usados[ind];
			unsigned int id_allele = pop->get(pos_ind).getAllele(pos_marker);
			alleles.push_back( getAllele(pos_marker, id_allele, marker) );
		}
		
		cout << "Statistics::processStatistics - Processing Statistics\n";
		// TODO: procesar todos los estadisticos con el vector de strings
		// Notar que aqui seria conveniente tambien tene una tabla de mutaciones
		
		
		stats_vector.push_back(stats);
	}
	statistics[name] = stats_vector;
	
}

// Busca el alelo en la tabla, lo genere recursivamente si no lo encuentra
string &Statistics::getAllele(unsigned int marker_pos, unsigned int id, ProfileMarker &marker){
	map<unsigned int, string>::iterator it = alleles_tables[marker_pos].find(id);
	if( it != alleles_tables[marker_pos].end() ){
		return it->second;
	}
	else if( id == 0 ){
		alleles_tables[marker_pos][id] = generateAllele(marker_pos, marker);
		return alleles_tables[marker_pos][id];
	}
	else{
		// Aplicar mutacion
		// Notar que en la practica, esto depende del tipo de marcador
		string parent = getAllele(marker_pos, pool->getParent(marker_pos, id), marker);
		// TODO: Mutar el padre
		// ...
		if( marker.getType() == MARKER_SEQUENCE 
			&& marker.getMutationType() == MUTATION_BASIC){
			
			
			
		}
		else{
			cerr << "Statistics::getAllele - Mutation model not implemented.\n";
		}
		
		
		alleles_tables[marker_pos][id] = parent;
		return alleles_tables[marker_pos][id];
	}
}

string Statistics::generateAllele(unsigned int marker_pos, ProfileMarker &marker){
	string s;
	// Generar el string considerando profile
	// Esto tambien depende del tipo de marcador
	
	
	return s;
}












