
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <random>
#include <algorithm>

#include <vector>
#include <map>
#include <set>

#include "NanoTimer.h"

using namespace std;

double statHeterozygosity(map<unsigned int, unsigned int> &allele_data){
	double sum = 0.0;
	unsigned int total = 0;
	for( auto it : allele_data ){
		total += it.second;
	}
	for( auto it : allele_data ){
		sum += pow(((double)it.second)/total, 2.0);
	}
	return (1.0 - sum);
}

double statEffectiveNumAlleles(double h){
	return (1.0/(1.0-h));
}

pair<double, double> getStatPair(vector<double> stats){
	if( stats.size() < 1 ){
		return pair<double, double>(0.0, 0.0);
	}
	double mean = 0;
	double var = 0;
	double stddev = 0;
	for(double stat : stats){
		mean += stat;
	}
	mean /= stats.size();
	for(double stat : stats){
		var += pow(stat - mean, 2.0);
	}
	var /= stats.size();
	stddev = pow(var, 0.5);
	return pair<double, double>(mean, stddev);
}

map<unsigned int, unsigned int> statAllelesData(vector<string> &alleles){
	map<unsigned int, unsigned int> ids;
	
	// Verificacion de seguridad
	if( alleles.size() < 1 ){
		return ids;
	}
	
	// Asumo que el largo es igual, par y fijo
	for( string allelle : alleles ){
//		cout << "Statistics::statAllelesData - allelle: \"" << allelle << "\"\n";
		unsigned int len = allelle.length();
		if( (len & 0x1) != 0 ){
			cerr << "Statistics::statAllelesData - Error,length is NOT even (" << len << ")\n";
			continue;
		}
		len /= 2;
		string str1 = allelle.substr(0, len);
		string str2 = allelle.substr(len, len);
		unsigned int id1 = std::stoi( str1 );
		unsigned int id2 = std::stoi( str2 );
		ids[id1]++;
		ids[id2]++;
	}
//	for( auto par : ids ){
//		cout << "Statistics::statAllelesData - allele[" << par.first << "]: " << par.second<< "\n";
//	}
	
	return ids;
}


int main(int argc,char** argv) {

	if(argc != 3){
		cout << "\nUsage: ./statistics_ms genepop_file n_markers\n";
		cout << "\n";
		return 0;
	}
	string genepop_file = argv[1];
	unsigned int n_markers = atoi(argv[2]);
	
	// Mapa de Pop -> vector (por marcador) de vector (por individuo)
	map<string, vector<vector<string>>> alleles_map;
	
	ifstream reader(genepop_file, ifstream::in);
	if( ! reader.is_open() ){
		cerr << "Can't open file \"" << genepop_file << "\"\n";
		return 0;
	}
	
	unsigned int buff_size = 1024*1024*10;
	char *buff = new char[buff_size + 1];
	memset(buff, 0, buff_size + 1);
	
	// Titulo
	reader.getline(buff, buff_size);
//	cout << "Statistics::processStatistics - Title: " << buff << "\n";
	
	while( reader.good() ){
		reader.getline(buff, buff_size);
		if( reader.gcount() >= 3 && toupper(buff[0]) == 'P' && toupper(buff[1]) == 'O' && toupper(buff[2]) == 'P' ){
			break;
		}
	}
	
//	cout << "Statistics::processStatistics - Pop: " << buff << "\n";
	
	// Inicio de datos
	while( reader.good() ){
		reader.getline(buff, buff_size);
		unsigned int lectura = reader.gcount();
		// Cada linea debe tener al menos id + , + data
		if( !reader.good() || lectura < 3 ){
			break;
		}
		
		string line(buff);
		replace(line.begin(), line.end(), ',', ' ');
		replace(line.begin(), line.end(), ';', ' ');
//		cout << "Statistics::processStatistics - line: \"" << line << "\" (" << line.length() << ")\n";
		
		stringstream toks(line);
		string id = "";
		string separator = "";
		string data = "";
		
		toks >> id;
//		cout << "Statistics::processStatistics - id: \"" << id << "\"\n";
		if( id.length() == 3 && toupper(id[0]) == 'P' && toupper(id[1]) == 'O' && toupper(id[2]) == 'P' ){
			continue;
		}
		
		vector<string> markers_allele;
		for( unsigned int i = 0; i < n_markers; ++i ){
			toks >> data;
//			cout << "Statistics::processStatistics - data : \"" << data << "\"\n";
			markers_allele.push_back(data);
		}
		
		auto it = alleles_map.find(id);
		if( it != alleles_map.end() ){
			it->second.push_back(markers_allele);
		}
		else{
			vector<vector<string>> individuals;
			individuals.push_back(markers_allele);
			alleles_map[id] = individuals;
		}
		
	}
	delete [] buff;
	reader.close();
	
	// Procesar estadisticos por poblacion
	map<string, vector<double>> statistics;
	for( auto par_map : alleles_map ){
		string pop = par_map.first;
		vector<vector<string>> individuals = par_map.second;
//		cout << "Stats de Population " << pop << "\n";
		// Procesar por marcador
		vector<double> stats;
		for(unsigned int marker = 0; marker < n_markers; ++marker){
			vector<string> alleles_marker;
			for( vector<string> alleles_ind : individuals ){
				alleles_marker.push_back( alleles_ind[marker] );
			}
			map<unsigned int, unsigned int> alleles_data = statAllelesData(alleles_marker);
			
			double num_alleles = alleles_data.size();
			double heterozygosity = statHeterozygosity(alleles_data);
			double effective_num_alleles = statEffectiveNumAlleles(heterozygosity);
			
//			cout << "Marker " << marker << ": (" << effective_num_alleles << ", " << heterozygosity << ", " << num_alleles << ")\n";
			stats.push_back(effective_num_alleles);
			stats.push_back(heterozygosity);
			stats.push_back(num_alleles);
			
		}
		statistics[pop] = stats;
		
	}
	
//	cout << "Resultados: \n";
	for(auto par_stats : statistics){
		cout << par_stats.first << "\t";
		for(double value : par_stats.second ){
			cout << value << "\t";
		}
		cout << " \n";
	}
	
	
	
	return 0;
	
}















