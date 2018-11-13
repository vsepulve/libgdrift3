
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

#include "ResultsReader.h"

using namespace std;

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
	
	
	ResultsReader reader;
	
	// Agrega los datos de TODOS los n_threads resultados parciales
	reader.readThreadsData(results_base, n_threads, n_stats, n_params);
	
	// Agrega el target
	reader.setTarget(target_file);
	
	// Normaliza tanto los datos como el target
	reader.normalize();
	
	// Calcula la distancia de cada dato normalizado al target
	reader.computeDistances();
	
	// Ordena por distancia y selecciona los mejores resultados para el training
	reader.selectBestResults(percentage);
	
	// Retorna pares <mean, stddev> de cada parametro calculado de los datos seleccionados
	vector< pair<double, double> > dist_post = reader.getPosteriori();
	
	fstream writer(distributions_file, fstream::out | fstream::trunc);
	
	// En la primera linea pongo las distancias
	writer << "Distances\t";
	writer << reader.getMinDistance() << "\t";
	writer << reader.getMaxDistance() << "\t";
	writer << reader.getCutDistance() << "\t";
	writer << "\n";
	
	// En la primera linea pongo las distancias
	writer << "DistancesNorm\t";
	writer << (reader.getMinDistance() / reader.getMaxDistance()) << "\t";
	writer << (reader.getMaxDistance() / reader.getMaxDistance()) << "\t";
	writer << (reader.getCutDistance() / reader.getMaxDistance()) << "\t";
	writer << "\n";
	
	// Siguen las distribuciones
	for(unsigned int i = 0; i < dist_post.size(); ++i){
		double mean = dist_post[i].first;
		double stddev = dist_post[i].second;
		cout << "Posteriori[" << i << "]\t" << mean << "\t" << stddev << "\n";
		writer << "Posteriori[" << i << "]\t" << mean << "\t" << stddev << "\n";
		
	}
	writer.close();
	
	vector< pair<double, unsigned int> > distancias = reader.getDistances();
	writer.open(final_results, fstream::out | fstream::trunc);
	for(unsigned int i = 0; i < distancias.size(); ++i){
		double d = distancias[i].first;
		unsigned pos = distancias[i].second;
		
		writer << d << "\t";
		
//		vector<double> res_stats = reader.getStats(pos);
//		for(unsigned int j = 0; j < n_stats; ++j){
//			writer << res_stats[j] << "\t";
//		}
		
		vector<double> res_params = reader.getParams(pos);
		for(unsigned int j = 0; j < n_params; ++j){
			writer << res_params[j] << "\t";
		}
		
		writer << "\n";
		
		
	}
	writer.close();
	
	
	cout << "Statistics - Fin\n";
	
	
	
	
	
	return 0;
}













