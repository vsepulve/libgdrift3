#ifndef _RESULTS_READER_H_
#define _RESULTS_READER_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include <math.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#include <vector>

using namespace std;

class ResultsReader{

private:

	vector< vector<double> > stats;
	vector< vector<double> > stats_norm;
	vector< vector<double> > params;
	
	vector<double> target;
	
	vector<double> min_stats;
	vector<double> max_stats;
	
	vector< pair<double, unsigned int> > distancias;
	
	vector< vector<double> > samples;
	
	// Parametros y marcadores
	
	bool data_read;
	bool target_read;
	bool data_normalized;
	bool distances_computed;
	
	unsigned int n_threads;
	unsigned int n_stats;
	unsigned int n_params;
	
	double min_distance;
	double max_distance;
	double cut_distance;
	
	double getMean(vector<double> &data);
	// Notar que realizo una copia local de data para ordenarlo sin modificar los originales
	double getMedian(vector<double> &data);
	// Si se recibe mean, se usa. De otro modo, se calcula.
	double getVariance(vector<double> &data, double mean);
	
public:

	ResultsReader();
	virtual ~ResultsReader();
	
	// Agrega los datos de TODOS los n_threads resultados parciales
	void readThreadsData(string &output_base, unsigned int n_threads, unsigned int n_stats, unsigned int n_params);
	
	// Agrega el target
	void setTarget(string &target_file);
	
	// Normaliza tanto los datos como el target
	void normalize();
	
	// Calcula la distancia de cada dato normalizado al target
	void computeDistances();
	
	// Ordena por distancia y selecciona los mejores resultados para el training
	void selectBestResults(double f_training);
	
	// Retorna pares <mean, stddev> de cada parametro calculado de los datos seleccionados
	vector< pair<double, double> > getPosteriori();
	
	vector< pair<double, unsigned int> > &getDistances(){
		return distancias;
	}
	
	vector<double> &getStats(unsigned int pos){
		return stats[pos];
	}
	
	vector<double> &getParams(unsigned int pos){
		return params[pos];
	}
	
	void addWorstDistance(double _max_distance){
		if( _max_distance > max_distance ){
			max_distance = _max_distance;
		}
	}
	
	double getMinDistance(){
		return min_distance;
	}
	
	double getMaxDistance(){
		return max_distance;
	}
	
	double getCutDistance(){
		return cut_distance;
	}
	
	
	
	
	
	
	
	
	
	
};

#endif
