
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#include <algorithm>
#include <math.h>

#include <map>
#include <vector>

using namespace std;

double distance(vector<double> &stats, vector<double> &target){
	double d = 0.0;
	for(unsigned int i = 0; i < target.size(); ++i){
		d += pow( stats[i] - target[i], 2.0 );
	}
	d = pow(d, 0.5);
	return d;
}

double get_mean(vector<double> &data){
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
double get_median(vector<double> &data){
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
double get_variance(vector<double> &data, double mean){
	if( data.size() < 1 ){
		return 0.0;
	}
	double suma = 0.0;
	for(unsigned int i = 0; i < data.size(); ++i){
		suma += pow(data[i] - mean, 2.0);
	}
	return suma/data.size();
}

int main(int argc,char** argv){

	if(argc != 9){
		cout<<"\nUsage: ./generate_statistics results_base n_threads n_stats n_params target_file percentage distributions_file final_results\n";
		cout<<"\n";
		return 0;
	}
	
	string results_base = argv[1];
	unsigned int n_threads = atoi(argv[2]);
	unsigned int n_stats = atoi(argv[3]);
	unsigned int n_params = atoi(argv[4]);
	string target_file = argv[5];
	float percentage = atof(argv[6]);
	string distributions_file = argv[7];
	string final_results = argv[8];
	
	cout << "Statistics - Inicio (n_stats: " << n_stats << ", n_params: " << n_params << ", percentage: " << percentage << ")\n";
	
	vector< vector<double> > stats;
	vector< vector<double> > stats_norm;
	vector< vector<double> > params;
	unsigned int buff_size = 1020*1024;
	char buff[buff_size];
	fstream lector;
	double value = 0;
	cout << "Statistics - Leyendo resultados\n";
	for(unsigned int pid = 0; pid < n_threads; ++pid){
		string file_name = results_base;
		file_name += std::to_string(pid);
		file_name += ".txt";
		lector.open(file_name, fstream::in);
		
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
	cout << "Statistics - Resultados: " << stats.size() << "\n";
	
	
	vector<double> target;
	cout << "Statistics - Cargando target\n";
	lector.open(target_file, ifstream::in);
	if( ! lector.is_open() ){
		cout << "Statistics - Problemas al abrir archivo \"" << target_file << "\"\n";
		return 1;
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
	
	cout << "Statistics - Calculando minimos y maximos por stat\n";
	vector<double> min_stats;
	vector<double> max_stats;

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
	cout << "Statistics - Procesados para min/max: " << procesados << "\n";
	
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
	
	cout << "Statistics - Normalizando target y stats de resultados\n";
	for(unsigned int i = 0; i < target.size(); ++i){
		target[i] = ( target[i] - min_stats[i] )/(max_stats[i] - min_stats[i]);
	}
	for( vector<double> &res_stats : stats_norm ){
		for(unsigned int i = 0; i < res_stats.size(); ++i){
			res_stats[i] = ( res_stats[i] - min_stats[i] )/(max_stats[i] - min_stats[i]);
		}
	}
	
	// Verificacion
	cout << "Target: ";
	for(unsigned int i = 0; i < n_stats; ++i){
		cout << target[i] << " | ";
	}
	cout << "\n";
	for(unsigned int k = 0; k < 10 && k < stats.size(); ++k){
		cout << "Res[" << k << "]: ";
		for(unsigned int i = 0; i < n_stats; ++i){
			cout << stats[k][i] << " | ";
		}
		cout << "\n";
		cout << "Res_norm[" << k << "]: ";
		for(unsigned int i = 0; i < n_stats; ++i){
			cout << stats_norm[k][i] << " | ";
		}
		cout << "\n";
	}
	
	// Arreglo con pares <distancia, pos>
	// Eso es para ordenar y seleccionar mas facilmente
	vector< pair<double, unsigned int> > distancias;
	unsigned int pos = 0;
	for( vector<double> &res_stats : stats_norm ){
		value = distance( res_stats, target );
		distancias.push_back( pair<double, unsigned int>(value, pos++) );
	}
	
//	// Verificacion
//	for(unsigned int k = 0; k < 10 && k < distancias.size(); ++k){
//		cout << "Distancia[" << k << "]: (" << distancias[k].first <<", " << distancias[k].second <<")\n";
//	}
//	cout<<"-----\n";
	
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
	unsigned int topk = (unsigned int)(percentage * distancias.size());
	cout << "Statistics - Topk: " << topk << " / " << distancias.size() << "\n";
	vector< vector<double> > samples;
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
	
//	// Verificacion
//	cout << "Statistics - samples.size: " << samples.size() << "\n";
//	for(unsigned int i = 0; i < samples.size(); ++i){
//		cout << "Statistics - samples[" << i << "].size: " << samples[i].size() << "\n";
//	}
	
	
	fstream writer(distributions_file, fstream::out | fstream::trunc);
	// Calculo de estadisticos reales
	for(unsigned int i = 0; i < samples.size(); ++i){
		double mean = get_mean(samples[i]);
		double median = get_median(samples[i]);
		double variance = get_variance(samples[i], mean);
		double stddev = pow(variance, 0.5);
		
		cout << "Statistics - Posteriori[" << i << "]: (mean: " << mean << ", median: " << median << ", variance: " << variance << ", stddev: " << stddev << ")\n";
		writer << "Posteriori[" << i << "]: mean: " << mean << " | median: " << median << " | variance: " << variance << " | stddev: " << stddev << "\n";
		
	}
	writer.close();
	
	
	writer.open(final_results, fstream::out | fstream::trunc);
	for(unsigned int i = 0; i < distancias.size(); ++i){
		double d = distancias[i].first;
		unsigned pos = distancias[i].second;
		vector<double> res_stats = stats[pos];
		vector<double> res_params = params[pos];
		
		writer << d << "\t";
		
		for(unsigned int j = 0; j < n_stats; ++j){
			writer << res_stats[j] << "\t";
		}
		
		for(unsigned int j = 0; j < n_params; ++j){
			writer << res_params[j] << "\t";
		}
		
		writer << "\n";
		
		
	}
	writer.close();
	
	
	cout << "Statistics - Fin\n";
	
	
	
	
	
	return 0;
}













