#include "Statistics.h"

Statistics::Statistics(){
}

Statistics::Statistics(Simulator *sim, float sampling)
: generator((std::random_device())()) {
	assert(sim != NULL);
	assert(sampling >= 0.0);
	assert(sampling <= 1.0);
	
//	cout << "Statistics - Start\n";
	
	pool = sim->getPool();
	profile = sim->getProfile();
	
//	cout << "Statistics - Preparing mutations table\n";
	mutation_table['A'] = {'C', 'G', 'T'};
	mutation_table['C'] = {'A', 'G', 'T'};
	mutation_table['G'] = {'A', 'C', 'T'};
	mutation_table['T'] = {'A', 'C', 'G'};
	
	// Preparo tablas y cualquier otro dato necesario
	alleles_tables.resize(profile->getNumMarkers());
	alleles_mutations_tables.resize(profile->getNumMarkers());
	
	Population summary(0, profile, pool, generator);
	
	vector<string> pop_names = sim->getPopulationNames();
	
//	cout << "Statistics - Processing " << pop_names.size() << " populations\n";
	for(string name : pop_names){
//		cout << "Statistics - Population " << name << "\n";
		Population *pop = sim->getPopulation(name);
		processStatistics(pop, name, sampling);
		
//		cout << "Statistics - Adding individuals to summary\n";
		for(unsigned int i = 0; i < pop->size(); ++i){
			summary.add( pop->get(i) );
		}
	}
	
//	cout << "Statistics - Processing the combined population\n";
	processStatistics(&summary, "summary", sampling);
	
//	cout << "Statistics - End\n";
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
	
//	cout << "Statistics::processStatistics - Start (population " << name << ", size " << pop->size() << ", sampling " << sampling << ")\n";
	
	unsigned int n_inds = pop->size() * sampling;
	if(n_inds < min_sampling){
		n_inds = min_sampling;
	}
	if(n_inds > pop->size()){
		n_inds = pop->size();
	}
	
	// Necesito un shuffle de la poblacion
	// Para simularlo, puedo desordenar las posiciones
//	cout << "Statistics::processStatistics - Preparing selected individuals (n_inds: " << n_inds << ")\n";
	vector<unsigned int> inds_usados;
	for(unsigned int i = 0; i < pop->size(); ++i){
		inds_usados.push_back(i);
	}
	shuffle(inds_usados.begin(), inds_usados.end(), generator);
	
	// Para esta poblacion:
	// Iterar por los marcadores
	// Para cada marcador, calcular SU mapa de estadisticos y agregarlo
	
	vector<map<string, double>> stats_vector;
//	cout << "Statistics::processStatistics - Processing " << profile->getNumMarkers() << " markers\n";
	for(unsigned int pos_marker = 0; pos_marker < profile->getNumMarkers(); ++pos_marker){
		map<string, double> stats;
		ProfileMarker marker = profile->getMarker(pos_marker);
		
//		cout << "Statistics::processStatistics - Preparing String Vector\n";
		vector<string> alleles;
		vector< map<unsigned int, char> > alleles_mutations;
		set<unsigned int> added_alleles;
		for(unsigned int ind = 0; ind < n_inds; ++ind){
			unsigned int pos_ind = inds_usados[ind];
			unsigned int id_allele = pop->get(pos_ind).getAllele(pos_marker);
			alleles.push_back( getAllele(pos_marker, id_allele, marker) );
			alleles_mutations.push_back( alleles_mutations_tables[pos_marker][id_allele] );
			
		}
//		cout << "Statistics::processStatistics - alleles_table: " << alleles_tables[pos_marker].size() 
//			<< ", " << alleles.size() << " strings generated from " << pool->getNextAllele(pos_marker) <<"\n";
		
//		cout << "Statistics::processStatistics - Processing Statistics\n";
		
		// Notar que estoy usando los nombres antiguos por estadistico para conservar el orden
		
		// "number-of-haplotypes"
		NanoTimer timer;
		double num_haplotypes = statNumHaplotypes(alleles);
		stats["number-of-haplotypes"] = num_haplotypes;
//		cout << "Statistics::processStatistics - num_haplotypes en " << timer.getMilisec() << " ms\n";
		
		// "number-of-segregating-sites"
		timer.reset();
		double num_segregating_sites = statNumSegregatingSites(alleles);
		stats["number-of-segregating-sites"] = num_segregating_sites;
//		cout << "Statistics::processStatistics - num_segregating_sites en " << timer.getMilisec() << " ms\n";
		
		timer.reset();
//		vector<unsigned int> pairwise_differences = statPairwiseDifferences(alleles);
//		vector<unsigned int> pairwise_differences_muts = statPairwiseDifferencesMutations(alleles_mutations);
		vector<unsigned int> pairwise_differences = statPairwiseDifferencesMutations(alleles_mutations);
		
//		cout << "----- TEST -----\n";
//		if( pairwise_differences.size() != pairwise_differences_muts.size() ){
//			cout << "Statistics::processStatistics - ERROR, Largos diferentes (" << pairwise_differences.size() << " != " << pairwise_differences_muts.size() << ")\n";
//		}
//		cout << "Statistics::processStatistics - Comparando " << pairwise_differences.size() << " posiciones\n";
//		for(unsigned int i = 0; i < pairwise_differences.size(); ++i){
//			if( pairwise_differences[i] != pairwise_differences_muts[i] ){
//				cout << "Statistics::processStatistics - ERROR, Valores diferentes (" << pairwise_differences[i] << " != " << pairwise_differences_muts[i] << ")\n";
//			}
//		}
//		cout << "----- TEST -----\n";
		
//		cout << "Statistics::processStatistics - pairwise_differences (oculto) en " << timer.getMilisec() << " ms\n";
	
		// "mean-of-the-number-of-pairwise-differences"
		timer.reset();
		double mean_pairwise_diff = statMeanPairwiseDifferences(pairwise_differences);
		stats["mean-of-the-number-of-pairwise-differences"] = mean_pairwise_diff;
//		cout << "Statistics::processStatistics - mean_pairwise_diff en " << timer.getMilisec() << " ms\n";
		
		// "variance-of-the-number-of-pairwise-differences"
		timer.reset();
		double var_pairwise_diff = statVariancePairwiseDifferences(pairwise_differences, mean_pairwise_diff);
		stats["variance-of-the-number-of-pairwise-differences"] = var_pairwise_diff;
//		cout << "Statistics::processStatistics - var_pairwise_diff en " << timer.getMilisec() << " ms\n";
		
		// "tajima-d-statistics"
		timer.reset();
		double tajima_d = statTajimaD(alleles, num_segregating_sites, mean_pairwise_diff);
		stats["tajima-d-statistics"] = tajima_d;
//		cout << "Statistics::processStatistics - tajima_d en " << timer.getMilisec() << " ms\n";
		
		stats_vector.push_back(stats);
	}
	statistics[name] = stats_vector;
	
}

// Procesa todos los estadisticos de un genepop y los agrega a statistics[name][stat]
void Statistics::processStatistics(string filename, string name, unsigned int n_markers){
	
	cout << "Statistics::processStatistics - Start (filename " << filename << ", pop_name: " << name << ")\n";
	
	// Leer y parsear el genepop
	// Notar que lo ideal seria dejarlo esto a un objeto lecto de genepop
	
	// En esta version se llena el vector de alelos directamente con strings
	vector<string> alleles;
	
	// Primero la lectura directa del archivo que tenemos, despues cambio esto a una lectura generica en un GenepopReader
	
	ifstream lector(filename, ifstream::in);
	if( ! lector.is_open() ){
		cerr << "Statistics::processStatistics - Can't open file \"" << filename << "\"\n";
		return;
	}
	
	unsigned int buff_size = 1024*1024*10;
	char buff[buff_size];
	memset(buff, 0, buff_size);
	
	// Titulo
	lector.getline(buff, buff_size);
	cout << "Statistics::processStatistics - Title: " << buff << "\n";
	
	// Identificadores de Loci (separados por , en el formato estandar)
	lector.getline(buff, buff_size);
	cout << "Statistics::processStatistics - Loci: " << buff << " (Temporalmente asumo 1)\n";
	
	// Pop
	lector.getline(buff, buff_size);
	cout << "Statistics::processStatistics - Pop: " << buff << "\n";
	
	// Inicio de datos
	while( lector.good() ){
		lector.getline(buff, buff_size);
		unsigned int lectura = lector.gcount();
		// Cada linea debe tener al menos id + , + data
		if( !lector.good() || lectura < 3 ){
			break;
		}
		
		string line(buff);
		if( line.find(',') == string::npos ){
			break;
		}
		
		stringstream toks(line);
		string id = "";
		string data = "";
		
		toks >> id;
		toks >> data;
		
		cout << "Statistics::processStatistics - id: " << id << ", data : " << (string(data, 10)) << "\n";
		
		
	
	}
	
	
	
	
	vector<map<string, double>> stats_vector;
//	cout << "Statistics::processStatistics - Processing " << profile->getNumMarkers() << " markers\n";
	for(unsigned int pos_marker = 0; pos_marker < n_markers; ++pos_marker){
		map<string, double> stats;
		
		// Notar que estoy usando los nombres antiguos por estadistico para conservar el orden
		
		double num_haplotypes = statNumHaplotypes(alleles);
		stats["number-of-haplotypes"] = num_haplotypes;
		
		double num_segregating_sites = statNumSegregatingSites(alleles);
		stats["number-of-segregating-sites"] = num_segregating_sites;
		
		vector<unsigned int> pairwise_differences = statPairwiseDifferences(alleles);
		
		double mean_pairwise_diff = statMeanPairwiseDifferences(pairwise_differences);
		stats["mean-of-the-number-of-pairwise-differences"] = mean_pairwise_diff;
		
		double var_pairwise_diff = statVariancePairwiseDifferences(pairwise_differences, mean_pairwise_diff);
		stats["variance-of-the-number-of-pairwise-differences"] = var_pairwise_diff;
		
		double tajima_d = statTajimaD(alleles, num_segregating_sites, mean_pairwise_diff);
		stats["tajima-d-statistics"] = tajima_d;
		
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
//		cout << "Statistics::getAllele - Case 2 (Creating Origin)\n";
		alleles_tables[marker_pos][id] = generateAllele(marker_pos, marker);
		
		// Mapa de mutaciones para el alelo
		// TODO: este mapa solo es valido para datos de tipo secuencia
		map<unsigned int, char> mutations_map;
		alleles_mutations_tables[marker_pos][id] = mutations_map;
		
		return alleles_tables[marker_pos][id];
	}
	else{
//		cout << "Statistics::getAllele - Case 3\n";
		// Aplicar mutacion
		// Notar que en la practica, esto depende del tipo de marcador
		unsigned int parent_id = pool->getParent(marker_pos, id);
		string parent = getAllele(marker_pos, parent_id, marker);
		
		// Aqui ya puedo tomar el mapa de mutaciones del padre porque se genero en getParen
		// TODO: este mapa solo es valido para datos de tipo secuencia
		map<unsigned int, char> mutations_map;
		map<unsigned int, char> mutations_map_parent = alleles_mutations_tables[marker_pos][parent_id];
		mutations_map.insert(mutations_map_parent.begin(), mutations_map_parent.end());
		
		if( marker.getType() == MARKER_SEQUENCE 
			&& marker.getMutationType() == MUTATION_BASIC){
			uniform_int_distribution<> pos_dist(0, marker.getLength() - 1);
			unsigned int pos = pos_dist(generator);
			uniform_int_distribution<> mut_dist(0, 2);
			unsigned int mut = mut_dist(generator);
			char new_value = mutation_table[ parent[pos] ][mut];
//			cout << "Statistics::getAllele - Marker " << marker_pos 
//				<< ", allele " << id 
//				<< ", mutation \'" << parent[pos] 
//				<< "\' -> \'" << new_value 
//				<< "\' in position " << pos << "\n";
			
			mutations_map[pos] = new_value;
			// En forma especial borro la mutacion si regreso al valor original del primer ancestro
			// El id del primero (la raiz del arbol) esta fijo en 0
			if( alleles_tables[marker_pos][0][pos] == new_value ){
				mutations_map.erase(pos);
			}
			
			parent[pos] = new_value;
		}
		else{
			cerr << "Statistics::getAllele - Mutation model not implemented.\n";
		}
		
		alleles_mutations_tables[marker_pos][id] = mutations_map;
		
		alleles_tables[marker_pos][id] = parent;
		return alleles_tables[marker_pos][id];
	}
}

string Statistics::generateAllele(unsigned int marker_pos, ProfileMarker &marker){
//	cout << "Statistics::generateAllele - Start\n";
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
//		cerr << "Statistics::generateAllele - Sequence model not implemented.\n";
	}
	
//	cout << "Statistics::generateAllele - End (" << s << ", " << s.length() << ")\n";
	return s;
}


	
double Statistics::statNumHaplotypes(vector<string> &alleles){
//	cout << "Statistics::statNumHaplotypes - Inicio\n";
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
	
//	cout << "Statistics::statNumHaplotypes - Fin ("<<timer.getMilisec()<<" ms)\n";
	return res;
}
	
double Statistics::statNumSegregatingSites(vector<string> &alleles){
//	cout << "Statistics::statNumSegregatingSites - Inicio\n";
//	NanoTimer timer;
	if(alleles.size() <= 1){
		return 0.0;
	}
	
//	for( string allele : alleles ){
//		cout << "Statistics::statNumSegregatingSites - \"" << allele << "\"\n";
//	}
	
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
//	cout << "Statistics::statNumSegregatingSites - Fin\n";
	return res;
}

vector<unsigned int> Statistics::statPairwiseDifferences(vector<string> &alleles){
//	cout << "Statistics::statMeanPairwiseDifferences - Inicio\n";
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
//	cout << "Sample::pairwise_statistics - Fin ("<<timer.getMilisec()<<" ms)\n";
	return pairwise_differences;
}

vector<unsigned int> Statistics::statPairwiseDifferencesMutations(vector< map<unsigned int, char> > &alleles){
//	cout << "Statistics::statMeanPairwiseDifferences - Inicio\n";
//	NanoTimer timer;
	vector<unsigned int> pairwise_differences;
	
	for(unsigned int i = 0; i < alleles.size(); ++i){
		for(unsigned int j = i+1; j < alleles.size(); ++j){
			unsigned int diff = 0;
			
			// Agrego la diferencia simetrica de las mutaciones
			// Primero agrego desde i, omitiendo solo los pares IDENTICOS con j
			// Si una mutacion en i y en j afectan la misma posicion (pero con diferente caracter) conservo el primero
			// Luego agrego desde j omitiendo TODAS las que afecten a posiciones tocadas por i
			// Asi omito las identicas en ambos sentidos, y las semiidenticas solo en el segundo
			
//			for( auto it : alleles[i]){
//				unsigned int pos = it.first;
//				char c = it.second;
//				auto it_j = alleles[j].find(pos);
//				if( it_j != alleles[j].end() && c == it_j->second ){
//					// omito por ser identica
//				}
//				else{
//					++diff;
//				}
//			}
//			
//			for( auto it : alleles[j]){
//				unsigned int pos = it.first;
//				auto it_i = alleles[i].find(pos);
//				if( it_i == alleles[i].end() ){
//					++diff;
//				}
//			}
			
			// Una forma mas rapida deberia ser usando una diferencia simetrica lineal
			// Se sacan las posiciones replicadas a posteriori
			vector< pair<unsigned int, char> > res;
			std::set_symmetric_difference(alleles[i].begin(), alleles[i].end(), alleles[j].begin(), alleles[j].end(), back_inserter(res));
//			unsigned int diff2 = 0;
			unsigned int last = 0xffffffff;
			for( pair<unsigned int, char> it : res ){
				if( it.first != last ){
					++diff;
					last = it.first;
				}
			}
			
//			if( diff != diff2 ){
//				cout << "Statistics::statPairwiseDifferencesMutations - Error, " << diff << " vs " << diff2 << "\n";
//			}
			
			
			pairwise_differences.push_back(diff);
		}
	}
//	cout << "Sample::pairwise_statistics - Fin ("<<timer.getMilisec()<<" ms)\n";
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
	return variance;
}

double Statistics::statTajimaD(vector<string> &alleles, double num_segregating_sites, double mean_pairwise_diff){
//	return 0.0;
	
	double n = alleles.size();
	double ss = num_segregating_sites;
	double mpd = mean_pairwise_diff;
	
	if( n <= 1 || ss == 0 || mpd <= 0){
		return 0.0;
	}
	
	double a1 = 0.0;
	double a2 = 0.0;

	for(unsigned int k = 1; k < (unsigned int)n; ++k){
		a1 += 1.0 / (double)k;
		a2 += 1.0 / (double)(k*k);
	}

	double b1 = (n + 1.0) / ( 3.0 * (n - 1.0) );
	double b2 = (2.0 * (n*n + n + 3.0)) / ( (9.0 * n) * (n - 1.0) );
	double c1 = b1 - (1.0 / a1);
	double c2 = b2 + ((n + 2.0) / (a1 * n)) + a2 / (a1 * a1);
	double e1 = c1 / a1;
	double e2 = c2 / (a1*a1 + a2);
	double res = ( (mpd - (ss / a1)) / sqrt(e1*ss + e2*ss*(ss - 1.0)) );

	return res;
}










