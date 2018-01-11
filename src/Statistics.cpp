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
	
	cout << "Statistics - Preparing mutations table\n";
	mutation_table['A'] = {'C', 'G', 'T'};
	mutation_table['C'] = {'A', 'G', 'T'};
	mutation_table['G'] = {'A', 'C', 'T'};
	mutation_table['T'] = {'A', 'C', 'G'};
	
	// Preparo tablas y cualquier otro dato necesario
	alleles_tables.resize(profile->getNumMarkers());
	
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

	
// Procesa todos los estadisticos y los agrega a statistics[name][stat]
void Statistics::processStatistics(Population *pop, string name, float sampling){
	
	cout << "Statistics::processStatistics - Start (population " << name << ", size " << pop->size() << ", sampling " << sampling << ")\n";
	
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
		cout << "Statistics::processStatistics - alleles_table: " << alleles_tables[pos_marker].size() 
			<< ", " << alleles.size() << " strings generated from " << pool->getNextAllele(pos_marker) <<"\n";
		
		cout << "Statistics::processStatistics - Processing Statistics\n";
		
		// TODO: procesar todos los estadisticos con el vector de strings
		// Notar que aqui seria conveniente tambien tener una tabla de mutaciones
		// Notar que estoy usando los nombres antiguos por estadistico para conservar el orden
		
		// "number-of-haplotypes"
		double num_haplotypes = statNumHaplotypes(alleles);
		stats["number-of-haplotypes"] = num_haplotypes;
		
		// "number-of-segregating-sites"
		double num_segregating_sites = statNumSegregatingSites(alleles);
		stats["number-of-segregating-sites"] = num_segregating_sites;
		
		vector<unsigned int> pairwise_differences = statPairwiseDifferences(alleles);
	
		// "mean-of-the-number-of-pairwise-differences"
		double mean_pairwise_diff = statMeanPairwiseDifferences(pairwise_differences);
		stats["mean-of-the-number-of-pairwise-differences"] = mean_pairwise_diff;
		
		// "variance-of-the-number-of-pairwise-differences"
		double var_segregating = statVariancePairwiseDifferences(pairwise_differences, mean_pairwise_diff);
		stats["variance-of-the-number-of-pairwise-differences"] = var_segregating;
		
		// "tajima-d-statistics"
		double tajima_d = statTajimaD(alleles, num_segregating_sites, mean_pairwise_diff);
		stats["tajima-d-statistics"] = tajima_d;
		
		
//		findices.put("number-of-haplotypes",this->number_of_haplotypes(sequences_str[cid][gid]));
//		findices.put("number-of-segregating-sites",this->number_of_segregating_sites(sequences_str[cid][gid]));
//		tie(mean_of_the_number_of_pairwise_differences, variance_of_the_number_of_pairwise_differences) = this->pairwise_statistics_seq(sequences[cid][gid]);
//		
//		findices.put("mean-of-the-number-of-pairwise-differences", mean_of_the_number_of_pairwise_differences);
//		findices.put("variance-of-the-number-of-pairwise-differences", variance_of_the_number_of_pairwise_differences);

//		findices.put("tajima-d-statistics", this->tajima_d_statistics(double(sequences[cid][gid].size()),
//																						 findices.get<double>("number-of-segregating-sites"),
//																						 mean_of_the_number_of_pairwise_differences));
		
		
		stats_vector.push_back(stats);
	}
	statistics[name] = stats_vector;
	
}

// Busca el alelo en la tabla, lo genere recursivamente si no lo encuentra
string &Statistics::getAllele(unsigned int marker_pos, unsigned int id, ProfileMarker &marker){
//	cout << "Statistics::getAllele - Start (marker_pos: " << marker_pos << ", allele " << id << ", alleles_tables.size " << alleles_tables.size() << ")\n";
	map<unsigned int, string>::iterator it = alleles_tables[marker_pos].find(id);
	if( it != alleles_tables[marker_pos].end() ){
//		cout << "Statistics::getAllele - Case 1\n";
		return it->second;
	}
	else if( id == 0 ){
		cout << "Statistics::getAllele - Case 2 (Creating Origin)\n";
		alleles_tables[marker_pos][id] = generateAllele(marker_pos, marker);
		return alleles_tables[marker_pos][id];
	}
	else{
		cout << "Statistics::getAllele - Case 3\n";
		// Aplicar mutacion
		// Notar que en la practica, esto depende del tipo de marcador
		string parent = getAllele(marker_pos, pool->getParent(marker_pos, id), marker);
		if( marker.getType() == MARKER_SEQUENCE 
			&& marker.getMutationType() == MUTATION_BASIC){
			uniform_int_distribution<> pos_dist(0, marker.getLength() - 1);
			unsigned int pos = pos_dist(generator);
			uniform_int_distribution<> mut_dist(0, 2);
			unsigned int mut = mut_dist(generator);
			char new_value = mutation_table[ parent[pos] ][mut];
			cout << "Statistics::getAllele - Marker " << marker_pos 
				<< ", allele " << id 
				<< ", mutation \'" << parent[pos] 
				<< "\' -> \'" << new_value 
				<< "\' in position " << pos << "\n";
			parent[pos] = new_value;
		}
		else{
			cerr << "Statistics::getAllele - Mutation model not implemented.\n";
		}
		
		
		alleles_tables[marker_pos][id] = parent;
		return alleles_tables[marker_pos][id];
	}
}

string Statistics::generateAllele(unsigned int marker_pos, ProfileMarker &marker){
	cout << "Statistics::generateAllele - Start\n";
	string s;
	// Generar el string considerando profile
	// Esto tambien depende del tipo de marcador
	if( marker.getType() == MARKER_SEQUENCE ){
		unsigned int nucleo = 0;
		uniform_int_distribution<> nucleo_dist(0, 3);
		for(unsigned int i = 0; i < marker.getLength(); ++i){
			nucleo = nucleo_dist(generator);
			if( nucleo == 0 ){
				s.push_back('A');
			}
			else if( nucleo == 1 ){
				s.push_back('C');
			}
			else if( nucleo == 2 ){
				s.push_back('G');
			}
			else{
				s.push_back('T');
			}
		}
	}
	else{
		cerr << "Statistics::generateAllele - Sequence model not implemented.\n";
	}
	
	cout << "Statistics::generateAllele - End (" << s << ", " << s.length() << ")\n";
	return s;
}


	
double Statistics::statNumHaplotypes(vector<string> &alleles){
//	cout<<"Statistics::statNumHaplotypes - Inicio\n";
//	NanoTimer timer;
	if(alleles.size() <= 1){
		return 0.0;
	}
	
	map<string, unsigned int> haplotypes;
	
	for( string &allele : alleles ){
		if( haplotypes.find(allele) == haplotypes.end() ){
			haplotypes[allele] = 1;
		}
		else{
			++haplotypes[allele];
		}
	}
	
	double sum = 0.0;
	double N = alleles.size();
	for( auto& par : haplotypes ){
		double f = (double)(par.second);
		f /= N;
		sum += (f*f);
	}
	double res = ((N/(N-1.0))*(1.0-sum));
	
//	cout<<"Statistics::statNumHaplotypes - Fin ("<<timer.getMilisec()<<" ms)\n";
	return res;
}
	
double Statistics::statNumSegregatingSites(vector<string> &alleles){
//	cout<<"Statistics::statNumSegregatingSites - Inicio\n";
//	NanoTimer timer;
	if(alleles.size() <= 1){
		return 0.0;
	}
	
	double res = 0;

	string ref = alleles[0];
	for(unsigned int i = 0; i < ref.length(); i++){
		for(unsigned int j = 1; j < alleles.size(); j++){
			if( ref.at(i) != alleles[j].at(i) ){
				++res;
				break;
			}
		}
	}
//	cout<<"Statistics::statNumSegregatingSites - Fin ("<<timer.getMilisec()<<" ms)\n";
	return res;
}

vector<unsigned int> Statistics::statPairwiseDifferences(vector<string> &alleles){
//	cout<<"Statistics::statMeanPairwiseDifferences - Inicio\n";
//	NanoTimer timer;
	vector<unsigned int> pairwise_differences;
	for(unsigned int i = 0; i < alleles.size(); ++i){
		for(unsigned int j = i+1; j < alleles.size(); ++j){
			unsigned int diff = 0;
			for(unsigned int k = 0; k < alleles[i].length(); ++k){
				if( alleles[i][k] != alleles[j][k] ){
					++diff;
				}
			}
			pairwise_differences.push_back(diff);
		}
	}
//	cout<<"Sample::pairwise_statistics - Fin ("<<timer.getMilisec()<<" ms)\n";
	return pairwise_differences;
}

double Statistics::statMeanPairwiseDifferences(vector<unsigned int> &differences){
	if(differences.size() <= 1){
		return 0.0;
	}
	double mean = 0.0;
	for(unsigned int diff : differences){
		mean += diff;
	}
	mean /= double(differences.size());
	return mean;
}	

double Statistics::statVariancePairwiseDifferences(vector<unsigned int> &differences, double mean){
	if(differences.size() <= 1){
		return 0.0;
	}
	double variance = 0.0;
	for(unsigned int diff : differences){
		variance += (diff-mean) * (diff-mean);
	}
	variance /= (double)(differences.size());
	return 0.0;
}

double Statistics::statTajimaD(vector<string> &alleles, double num_segregating_sites, double mean_pairwise_diff){
	return 0.0;
}










