#include "Statistics.h"

const unsigned int Statistics::repeat_ini;

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
	alleles_ms_tables.resize(profile->getNumMarkers());
	alleles_repeats_tables.resize(profile->getNumMarkers());
	
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
		
		// Lo que sigue depende del tipo de marcador
		
		if( marker.getType() == MARKER_SEQUENCE ){
		
			vector<string> alleles;
			vector< map<unsigned int, char> > alleles_mutations;
			if( profile->getPloidy() == 1 ){
				for(unsigned int ind = 0; ind < n_inds; ++ind){
					unsigned int pos_ind = inds_usados[ind];
					unsigned int id_allele = pop->get(pos_ind).getAllele(pos_marker);
					alleles.push_back( getAllele(pos_marker, id_allele, marker) );
					alleles_mutations.push_back( alleles_mutations_tables[pos_marker][id_allele] );
				}
			}
			else{
				cerr << "Statistics::processStatistics - Error, Ploidy not supported (" << profile->getPloidy() << ")\n";
			}
			
			NanoTimer timer;
			double num_haplotypes = statNumHaplotypes(alleles);
			stats["num-haplotypes"] = num_haplotypes;
		
			timer.reset();
			double num_segregating_sites = statNumSegregatingSites(alleles);
			stats["num-segregating-sites"] = num_segregating_sites;
		
			// vector<unsigned int> pairwise_differences = statPairwiseDifferences(alleles);
			vector<unsigned int> pairwise_differences = statPairwiseDifferencesMutations(alleles_mutations);
		
			double mean_pairwise_diff = statMeanPairwiseDifferences(pairwise_differences);
			stats["mean-pairwise-diff"] = mean_pairwise_diff;

			double var_pairwise_diff = statVariancePairwiseDifferences(pairwise_differences, mean_pairwise_diff);
			stats["var-pairwise-diff"] = var_pairwise_diff;

			double tajima_d = statTajimaD(alleles, num_segregating_sites, mean_pairwise_diff);
			stats["tajima-d"] = tajima_d;
		}
		else if( marker.getType() == MARKER_MS ){
			
			cout << "Statistics::processStatistics - Preparando Data de microsatellites...\n";
			
			vector<string> alleles;
			if( profile->getPloidy() == 2 ){
				unsigned int min_id = 0xffffffff;
				unsigned int max_id = 0;
				unsigned int pos_ind = 0;
				unsigned int id_allele = 0;
				// Primero busco min y max para determinar limites al string
				for(unsigned int ind = 0; ind < n_inds; ++ind){
					pos_ind = inds_usados[ind];
					id_allele = pop->get(pos_ind).getAllele(pos_marker, 0);
					if( id_allele < min_id ){
						min_id = id_allele;
					}
					if( id_allele > max_id ){
						max_id = id_allele;
					}
					id_allele = pop->get(pos_ind).getAllele(pos_marker, 1);
					if( id_allele < min_id ){
						min_id = id_allele;
					}
					if( id_allele > max_id ){
						max_id = id_allele;
					}
				}
				string str_base = std::to_string( 1 + max_id - min_id );
				unsigned int str_len = str_base.length();
				if( str_len < 3 ){
					str_len = 3;
				}
				cout << "Statistics::processStatistics - min_id: " << min_id << ", max_id: " << max_id << ", str_base: \"" << str_base << "\", str_len: " << str_len << "\n";
				for(unsigned int ind = 0; ind < n_inds; ++ind){
					pos_ind = inds_usados[ind];
					id_allele = pop->get(pos_ind).getAllele(pos_marker, 0);
					string str_part = std::to_string( 1 + id_allele - min_id );
					while( str_part.length() < str_len ){
						str_part = "0" + str_part;
					}
					string str_allele = str_part;
					id_allele = pop->get(pos_ind).getAllele(pos_marker, 1);
					str_part = std::to_string( 1 + id_allele - min_id );
					while( str_part.length() < str_len ){
						str_part = "0" + str_part;
					}
					str_allele += str_part;
//					cout << "Statistics::processStatistics - str_allele: \"" << str_allele << "\"\n";
					alleles.push_back(str_allele);
				}
				
				
			}
			else{
				cerr << "Statistics::processStatistics - Error, Ploidy not supported (" << profile->getPloidy() << ")\n";
			}
			
			cout << "Statistics::processStatistics - Preparando statistics de microsatellites...\n";
			
			map<unsigned int, unsigned int> ids = statAllelesData(alleles);
			
			double num_alleles = ids.size();
			stats["num-alleles"] = num_alleles;
			
			double effective_num_alleles = 0;
			stats["effective-num-alleles"] = effective_num_alleles;
			
			double heterozygosity = 0;
			stats["heterozygosity"] = heterozygosity;
			
		}
		else{
			cerr << "Statistics::processStatistics - Error, Genetic Marker not supperted.\n";
		}
		
		
		stats_vector.push_back(stats);
	}
	statistics[name] = stats_vector;
	
}

// Procesa todos los estadisticos de un genepop y los agrega a statistics[name][stat]
// Si summary_alleles es diferente de NULL, se agregan los de esta poblacion para generar los stats de Summary
void Statistics::processStatistics(string filename, string name, Profile *external_profile, vector<vector<string>> *summary_alleles){
	
	cout << "Statistics::processStatistics - Start (filename " << filename << ", pop_name: " << name << ")\n";
	unsigned int n_markers = external_profile->getNumMarkers();
	
	// Leer y parsear el genepop
	// Notar que lo ideal seria dejarlo esto a un objeto lecto de genepop
	
	// En esta version se llena el vector de alelos directamente con strings
//	vector<string> alleles;
//	cout << "Statistics::processStatistics - Preparando allele_markers\n";
	vector<vector<string>> alleles_marker;
	alleles_marker.resize(n_markers);
	
	// Primero la lectura directa del archivo que tenemos, despues cambio esto a una lectura generica en un GenepopReader
	
//	cout << "Statistics::processStatistics - Preparando Lector\n";
	ifstream lector(filename, ifstream::in);
	if( ! lector.is_open() ){
		cerr << "Statistics::processStatistics - Can't open file \"" << filename << "\"\n";
		return;
	}
	
//	cout << "Statistics::processStatistics - Preparando Buffer\n";
	unsigned int buff_size = 1024*1024*10;
	char *buff = new char[buff_size + 1];
	memset(buff, 0, buff_size + 1);
	
//	cout << "Statistics::processStatistics - Iniciando Lectura\n";
	
	// Titulo
	lector.getline(buff, buff_size);
//	cout << "Statistics::processStatistics - Title: " << buff << "\n";
	
	while( lector.good() ){
		lector.getline(buff, buff_size);
		if( lector.gcount() >= 3 && toupper(buff[0]) == 'P' && toupper(buff[1]) == 'O' && toupper(buff[2]) == 'P' ){
			break;
		}
//		cout << "Statistics::processStatistics - Loci: " << buff << "\n";
	}
	
//	// Pop
//	cout << "Statistics::processStatistics - Pop: " << buff << "\n";
	
	// Inicio de datos
	while( lector.good() ){
		lector.getline(buff, buff_size);
		unsigned int lectura = lector.gcount();
		// Cada linea debe tener al menos id + , + data
		if( !lector.good() || lectura < 3 ){
			break;
		}
		
		string line(buff);
		replace(line.begin(), line.end(), ',', ' ');
		replace(line.begin(), line.end(), ';', ' ');
//		cout << "Statistics::processStatistics - line: " << line << "\n";
		
		stringstream toks(line);
		string id = "";
		string separator = "";
		string data = "";
		
		toks >> id;
//		cout << "Statistics::processStatistics - id: \"" << id << "\"\n";
		for( unsigned int i = 0; i < n_markers; ++i ){
			toks >> data;
//			cout << "Statistics::processStatistics - data : \"" << data << "\"\n";
			alleles_marker[i].push_back(data);
		}
		
	}
	
	delete [] buff;
	
	if(summary_alleles != NULL){
//		cout << "Statistics::processStatistics - Adding alleles to summary\n";
		if( summary_alleles->size() != n_markers ){
			summary_alleles->resize(n_markers);
		}
		for( unsigned int i = 0; i < n_markers; ++i ){
			summary_alleles->at(i).insert( 
				summary_alleles->at(i).begin(), 
				alleles_marker[i].begin(), 
				alleles_marker[i].end() 
			);
		}
	}
	
	processStatistics(name, external_profile, &alleles_marker);
	
}

// Procesa los stats directamente de una muestra de alelos como string y los asocia a la poblacion "name"
void Statistics::processStatistics(string name, Profile *external_profile, vector<vector<string>> *alleles_marker){
	unsigned int n_markers = external_profile->getNumMarkers();
	assert(n_markers == alleles_marker->size());
	
	cout << "Statistics::processStatistics - Start Internal for pop_name: \"" << name << "\"\n";
	
	vector<map<string, double>> stats_vector;
//	cout << "Statistics::processStatistics - Processing " << profile->getNumMarkers() << " markers\n";
	for(unsigned int pos_marker = 0; pos_marker < n_markers; ++pos_marker){
		map<string, double> stats;
		vector<string> alleles = alleles_marker->at(pos_marker);
		ProfileMarker marker = external_profile->getMarker(pos_marker);
		
		if( marker.getType() == MARKER_SEQUENCE ){
			double num_haplotypes = statNumHaplotypes(alleles);
			stats["num-haplotypes"] = num_haplotypes;
		
			double num_segregating_sites = statNumSegregatingSites(alleles);
			stats["num-segregating-sites"] = num_segregating_sites;
		
			vector<unsigned int> pairwise_differences = statPairwiseDifferences(alleles);
		
			double mean_pairwise_diff = statMeanPairwiseDifferences(pairwise_differences);
			stats["mean-pairwise-diff"] = mean_pairwise_diff;
		
			double var_pairwise_diff = statVariancePairwiseDifferences(pairwise_differences, mean_pairwise_diff);
			stats["var-pairwise-diff"] = var_pairwise_diff;
		
			double tajima_d = statTajimaD(alleles, num_segregating_sites, mean_pairwise_diff);
			stats["tajima-d"] = tajima_d;
		}
		else if( marker.getType() == MARKER_MS ){
			cout << "Statistics::processStatistics - Preparando statistics de microsatellites...\n";
			
			map<unsigned int, unsigned int> ids = statAllelesData(alleles);
			
			double num_alleles = ids.size();
			stats["num-alleles"] = num_alleles;
			
			double effective_num_alleles = 0;
			stats["effective-num-alleles"] = effective_num_alleles;
			
			double heterozygosity = 0;
			stats["heterozygosity"] = heterozygosity;
			
		}
		else{
			cerr << "Statistics::processStatistics - Error, Genetic Marker not supperted.\n";
		}
		
		
		
		
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
		string seq = generateAllele(marker_pos, marker);
		alleles_tables[marker_pos][id] = seq;
		if( marker.getType() == MARKER_SEQUENCE ){
			map<unsigned int, char> mutations_map;
			alleles_mutations_tables[marker_pos][id] = mutations_map;
		}
		else if( marker.getType() == MARKER_MS ){
			alleles_ms_tables[marker_pos][seq] = repeat_ini;
			alleles_repeats_tables[marker_pos][repeat_ini] = seq;
		}
		else{
			cerr << "Statistics::getAllele - Error, Genetic Marker not supperted.\n";
		}
		return alleles_tables[marker_pos][id];
	}
	else{
//		cout << "Statistics::getAllele - Case 3\n";
		
		// Tomar el allelo padre, y aplicar mutacion
		// Notar que en la practica, esto depende del tipo de marcador
		
		unsigned int parent_id = pool->getParent(marker_pos, id);
		string parent = getAllele(marker_pos, parent_id, marker);
		
		if( marker.getType() == MARKER_SEQUENCE 
			&& marker.getMutationType() == MUTATION_BASIC ){
			
			// Aqui ya puedo tomar el mapa de mutaciones del padre porque se genero en getParen
			// TODO: este mapa solo es valido para datos de tipo secuencia
			map<unsigned int, char> mutations_map;
			map<unsigned int, char> mutations_map_parent = alleles_mutations_tables[marker_pos][parent_id];
			mutations_map.insert(mutations_map_parent.begin(), mutations_map_parent.end());
			
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
			alleles_mutations_tables[marker_pos][id] = mutations_map;
			
			parent[pos] = new_value;
		}
		else if( marker.getType() == MARKER_MS 
			&& marker.getMutationType() == MUTATION_BASIC ){
			
			cout << "Statistics::getAllele - parent id: " << parent << " (" << std::stoi(parent) << ")\n";
			
			
		}
		else{
			cerr << "Statistics::getAllele - Error, Mutation model not implemented.\n";
		}
		
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
	else if( marker.getType() == MARKER_MS ){
		s = "001";
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
			// Una forma mas rapida deberia ser usando una diferencia simetrica lineal
			// Se sacan las posiciones replicadas a posteriori
			vector< pair<unsigned int, char> > res;
			std::set_symmetric_difference(alleles[i].begin(), alleles[i].end(), alleles[j].begin(), alleles[j].end(), back_inserter(res));
			unsigned int last = 0xffffffff;
			for( pair<unsigned int, char> it : res ){
				if( it.first != last ){
					++diff;
					last = it.first;
				}
			}
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

map<unsigned int, unsigned int> Statistics::statAllelesData(vector<string> &alleles){
	map<unsigned int, unsigned int> ids;
	
	// Verificacion de seguridad
	if( alleles.size() < 1 ){
		return ids;
	}
	unsigned int len = alleles[0].length();
	if( (len & 0x1) != 0 ){
		cerr << "Statistics::statAllelesData - Error, Preliminary length is NOT even (" << len << ")\n";
		return ids;
	}
	for( string allele : alleles ){
		if( allele.length() != len ){
			cerr << "Statistics::statAllelesData - Error, alleles of different length (" << allele.length() << " != " << len << ")\n";
		}
	}
	len /= 2;
	
	// Asumo que el largo es igual, par y fijo
	for( string allelle : alleles ){
		string str1 = allelle.substr(0, len);
		string str2 = allelle.substr(len, len);
		unsigned int id1 = std::stoi( str1 );
		unsigned int id2 = std::stoi( str2 );
		ids[id1]++;
		ids[id2]++;
	}
	for( auto par : ids ){
		cout << "Statistics::statAllelesData - allele[" << par.first << "]: " << par.second<< "\n";
	}
	
	return ids;
}

double Statistics::statEffectiveNumAlleles(vector<string> &alleles){
	return 0.0;
}

double Statistics::statHeterozygosity(vector<string> &alleles){
	return 0.0;
}









