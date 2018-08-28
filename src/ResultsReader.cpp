#include "ResultsReader.h"

ResultsReader::ResultsReader(){

	data_read = false;
	target_read = false;
	data_normalized = false;
	distances_computed = false;
	
	n_threads = 0;
	n_stats = 0;
	n_params = 0;
	
}

ResultsReader::~ResultsReader(){
}


void ResultsReader::readThreadsData(string &output_base, unsigned int _n_threads, unsigned int _n_stats, unsigned int _n_params){
	
	data_read = true;
	n_threads = _n_threads;
	n_stats = _n_stats;
	n_params = _n_params;
	
	unsigned int buff_size = 1020*1024;
	char buff[buff_size];
	fstream lector;
	double value = 0;
	
	// Cargar todos los resultados de este feedback
	cout << "ResultsReader::readThreadsData - Leyendo resultados\n";
	for(unsigned int pid = 0; pid < n_threads; ++pid){
		string file_name = output_base;
		file_name += std::to_string(pid);
		file_name += ".txt";
		lector.open(file_name, fstream::in);
		if( ! lector.is_open() ){
			cerr << "ResultsReader::readThreadsData - Problemas al abrir archivo \"" << file_name << "\"\n";
			continue;
		}
		
		cout << "ResultsReader::readThreadsData - Cargando datos de \"" << file_name << "\"\n";
	
		while(true){
			lector.getline(buff, buff_size);
			if( !lector.good() || strlen(buff) < 1 ){
				break;
			}
			
			string line(buff);
			stringstream toks(line);
			
			vector<double> res_stats;
			vector<double> res_stats_norm;
			for(unsigned int i = 0; i < n_stats; ++i){
				value = 0.0;
				toks >> value;
				res_stats.push_back(value);
				res_stats_norm.push_back(value);
			}
			stats.push_back(res_stats);
			stats_norm.push_back(res_stats_norm);
			
			vector<double> res_params;
			for(unsigned int i = 0; i < n_stats; ++i){
				value = 0.0;
				toks >> value;
				res_params.push_back(value);
			}
			params.push_back(res_params);
			
		}
		
		lector.close();
	}
	
}

void ResultsReader::setTarget(string &target_file){
	
	target_read = true;
	
	unsigned int buff_size = 1020*1024;
	char buff[buff_size];
	fstream lector;
	double value = 0;
	
	cout << "ResultsReader::setTarget - Cargando target de \"" << target_file << "\"\n";
	lector.open(target_file, ifstream::in);
	if( ! lector.is_open() ){
		cerr << "ResultsReader::setTarget - Problemas al abrir archivo\n";
		return;
	}
	else{
		lector.getline(buff, buff_size);
		
		//Linea valida de largo > 0
		string line(buff);
		stringstream toks(line);
		
		// stats
		for(unsigned int i = 0; i < n_stats; ++i){
			value = 0.0;
			toks >> value;
			target.push_back( value );
		}
		
		cout << "Target: ";
		for(unsigned int i = 0; i < target.size(); ++i){
			cout << target[i] << " | ";
		}
		cout << "\n";
	
	}
	lector.close();
}

// Normaliza tanto los datos como el target
void ResultsReader::normalize(){
	assert( data_read == true);
	assert( target_read == true);
	
	data_normalized = true;
	
	double value = 0;
	
	cout << "ResultsReader::normalize - Calculando minimos y maximos por stat\n";

	// Inicializo con los valores del target y busco en los resultados
	for( double d : target ){
		min_stats.push_back(d);
		max_stats.push_back(d);
	}
	
	unsigned int procesados = 0;
	for( vector<double> &res_stats : stats ){
		++procesados;
		for(unsigned int i = 0; i < res_stats.size(); ++i){
			value = res_stats[i];
			if( value < min_stats[i] ){
				min_stats[i] = value;
			}
			if( value > max_stats[i] ){
				max_stats[i] = value;
			}
		}
	}
	cout << "ResultsReader::normalize - Procesados para min/max: " << procesados << "\n";
	
	// Verificacion
	cout << "Min: ";
	for(unsigned int i = 0; i < n_stats; ++i){
		cout << min_stats[i] << " | ";
	}
	cout << "\n";
	cout << "Max: ";
	for(unsigned int i = 0; i < n_stats; ++i){
		cout << max_stats[i] << " | ";
	}
	cout << "\n";
	
	cout << "ResultsReader::normalize - Normalizando target y stats de resultados\n";
	for(unsigned int i = 0; i < target.size(); ++i){
		target[i] = ( target[i] - min_stats[i] )/(max_stats[i] - min_stats[i]);
	}
	for( vector<double> &res_stats : stats_norm ){
		for(unsigned int i = 0; i < res_stats.size(); ++i){
			res_stats[i] = ( res_stats[i] - min_stats[i] )/(max_stats[i] - min_stats[i]);
		}
	}
	
	// Verificacion
//	cout << "Target: ";
//	for(unsigned int i = 0; i < n_stats; ++i){
//		cout << target[i] << " | ";
//	}
//	cout << "\n";
//	for(unsigned int k = 0; k < 10 && k < stats.size(); ++k){
//		cout << "Res[" << k << "]: ";
//		for(unsigned int i = 0; i < n_stats; ++i){
//			cout << stats[k][i] << " | ";
//		}
//		cout << "\n";
//		cout << "Res_norm[" << k << "]: ";
//		for(unsigned int i = 0; i < n_stats; ++i){
//			cout << stats_norm[k][i] << " | ";
//		}
//		cout << "\n";
//	}
	
}

// Calcula la distancia de cada dato normalizado al target
void ResultsReader::computeDistances(){
	assert( data_normalized == true);
	
	distances_computed = true;
	
	unsigned int pos = 0;
	for( vector<double> &res_stats : stats_norm ){
		double d = 0.0;
		for(unsigned int i = 0; i < target.size(); ++i){
			d += pow( res_stats[i] - target[i], 2.0 );
		}
		d = pow(d, 0.5);
		distancias.push_back( pair<double, unsigned int>(d, pos++) );
	}
	
}

// Ordena por distancia y selecciona los mejores resultados para el training
void ResultsReader::selectBestResults(double f_training){
	assert( distances_computed == true);
	
	double value = 0.0;
	
	sort(distancias.begin(), distancias.end());
	
	// Verificacion
	for(unsigned int k = 0; k < 10 && k < distancias.size(); ++k){
		cout << "Distancia[" << k << "]: (" << distancias[k].first <<", " << distancias[k].second <<")\n";
	}
	cout<<"-----\n";
	
	// Verificacion
	for(unsigned int k = 0; k < 10 && k < distancias.size(); ++k){
		unsigned int pos = distancias.size() - k - 1;
		cout << "Distancia[" << pos << "]: (" << distancias[pos].first <<", " << distancias[pos].second <<")\n";
	}
	cout<<"-----\n";
	
	// Seleccion efectiva de resultados
	unsigned int topk = (unsigned int)(f_training * distancias.size());
	if( topk < 10 ){
		topk = ((10<distancias.size())?10:distancias.size());
	}
	
	cout << "ResultsReader::selectBestResults - Topk: " << topk << " / " << distancias.size() << "\n";
	for(unsigned int i = 0; i < n_params; ++i){
		vector<double> sample;
		samples.push_back(sample);
	}
	for(unsigned int i = 0; i < topk; ++i){
		unsigned int pos_res = distancias[i].second;
		for(unsigned int j = 0; j < n_params; ++j){
			value = params[pos_res][j];
			samples[j].push_back(value);
		}
	}
	
}

// Retorna pares <mean, stddev> de cada parametro calculado de los datos seleccionados
vector< pair<double, double> > ResultsReader::getPosteriori(){
	vector< pair<double, double> > res;
	
	// Calculo de estadisticos reales
	for(unsigned int i = 0; i < samples.size(); ++i){
		double mean = getMean(samples[i]);
		double median = getMedian(samples[i]);
		double variance = getVariance(samples[i], mean);
		double stddev = pow(variance, 0.5);
		
		cout << "Statistics - Posteriori[" << i << "]: (mean: " << mean << ", median: " << median << ", variance: " << variance << ", stddev: " << stddev << ")\n";
//		writer << "Posteriori[" << i << "]: mean: " << mean << " | median: " << median << " | variance: " << variance << " | stddev: " << stddev << "\n";
		res.push_back( pair<double, double>(mean, stddev) );
	}
	
	return res;
}

double ResultsReader::getMean(vector<double> &data){
	if( data.size() < 1 ){
		return 0.0;
	}
	double suma = 0.0;
	for(unsigned int i = 0; i < data.size(); ++i){
		suma += data[i];
	}
	return suma/data.size();
}

// Notar que realizo una copia local de data para ordenarlo sin modificar los originales
double ResultsReader::getMedian(vector<double> &data){
	if( data.size() < 1 ){
		return 0.0;
	}
	double median;
	vector<double> copia = data;
	size_t size = copia.size();
	sort(copia.begin(), copia.end());
	// Si es par, promedio los dos elementos medios
	if( (size % 0x1) == 0 ){
		median = copia[ (size / 2) - 1 ] + copia[ size / 2 ];
		median /= 2.0;
	}
	else{
		median = copia[ size / 2 ];
	}
	return median;
}

// Si se recibe mean, se usa. De otro modo, se calcula.
double ResultsReader::getVariance(vector<double> &data, double mean){
	if( data.size() < 1 ){
		return 0.0;
	}
	double suma = 0.0;
	for(unsigned int i = 0; i < data.size(); ++i){
		suma += pow(data[i] - mean, 2.0);
	}
	return suma/data.size();
}








