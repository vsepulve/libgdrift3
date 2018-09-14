
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <sstream>
#include <fstream>
#include <string.h>
#include <random>

#include <vector>
#include <map>
#include <set>

using namespace std;

int main(int argc,char** argv) {

//	if(argc != 5){
//		cout<<"\nUsage: ./test n_params distributions_1 distributions_2 output_base\n";
//		cout<<"\n";
//		return 0;
//	}
//	unsigned int n_params = atoi(argv[1]);
//	string input_1 = argv[2];
//	string input_2 = argv[3];
//	string output_base = argv[4];
	
	cout << "BinomialTest - Inicio\n";
	
	unsigned int n_inds = 10000;
	unsigned int n_gens = 1000;
	float mut_rate = 0.000001;
	unsigned int bin_trials = 2;
	
	// Mapa de inds_old -> vector<inds_new>
	// Para cada numero inicial de individuos (inds_old), cada vez que se presenta, cuantos de esos pasan (inds_new)
	map<unsigned int, vector<unsigned int>> passed;
		
	unsigned int *ids_old = new unsigned int[n_inds];
	unsigned int *ids_new = new unsigned int[n_inds];
	
	random_device seed;
	mt19937 generator(seed());
	uniform_int_distribution<> inds_dist(0, n_inds - 1);
	
	binomial_distribution<> muts_dist(n_inds, mut_rate);
	unsigned int total_muts = 0;
	
	// Generacion inicial V1: todos diferentes
	unsigned int new_id = 0;
	for(unsigned int i = 0; i < n_inds; ++i){
		ids_old[i] = new_id++;
	}
	
	for(unsigned int i = 0; i < n_gens; ++i){
		
		// Mapas de alelos: id -> n_inds
		map<unsigned int, unsigned int> alleles_old;
		map<unsigned int, unsigned int> alleles_new;
		
		// Calculo de alleles old
		for(unsigned int j = 0; j < n_inds; ++j){
			alleles_old[ids_old[j]]++;
		}
		
		// Seleccion de padres
		for(unsigned int j = 0; j < n_inds; ++j){
			unsigned int pos = inds_dist(generator);
			ids_new[j] = ids_old[pos];
			alleles_new[ids_new[j]]++;
		}
		
		// Mutaciones
		unsigned int n_muts = muts_dist(generator);
		total_muts += n_muts;
//		cout << "BinomialTest - n_muts: " << n_muts << "\n";
		for(unsigned int j = 0; j < n_muts; ++j){
			unsigned int pos = inds_dist(generator);
//			cout << ids_new[pos] << " -> " << new_id << "\n";
			alleles_new[ids_new[pos]]--;
			ids_new[pos] = new_id++;
			alleles_new[ids_new[pos]]++;
		}
		
//		cout << "BinomialTest - Alleles Old\n";
//		for(auto par : alleles_old){
//			cout << par.first << " -> " << par.second << "\n";
//		}
		
//		cout << "BinomialTest - Alleles New\n";
//		for(auto par : alleles_new){
//			cout << par.first << " -> " << par.second << "\n";
//		}
		
//		cout << "BinomialTest - Conversion a Passed\n";
		for(auto par : alleles_old){
			unsigned int id = par.first;
			unsigned int n_old = par.second;
			unsigned int n_new = 0;
			auto it = alleles_new.find(id);
			if(it != alleles_new.end()){
				n_new = it->second;
			}
			passed.emplace(n_old, vector<unsigned int>());
			passed[n_old].push_back(n_new);
//			cout << n_old << " -> " << n_new << "\n";
		}
		
		unsigned int *tmp = ids_old;
		ids_old = ids_new;
		ids_new = tmp;
		
//		cout << "-----\n";
		
	}
	
	cout << "BinomialTest - Passed Directo (total_muts: " << total_muts << ")\n";
	for(auto par : passed){
		cout << par.first << " -> ";
		double mean = 0.0;
		for( unsigned int n : par.second ){
			mean += n;
//			cout << n << " ";
		}
		mean /= par.second.size();
		double var = 0.0;
		for( unsigned int n : par.second ){
			double nd = n;
			var += (nd-mean) * (nd-mean);
		}
		var /= par.second.size();
		cout << "mean: " << mean << ", var: " << var << " ";
		cout << "\n";
	}
	
	map<unsigned int, vector<unsigned int>> passed_bin;
	map<unsigned int, unsigned int> alleles_old;
	map<unsigned int, unsigned int> alleles_new;
	total_muts = 0;
	
	// Generacion inicial V1: todos diferentes
	new_id = 0;
	for(unsigned int i = 0; i < n_inds; ++i){
		alleles_old[new_id++]++;
	}
	
	unsigned int old_pop_size = n_inds;
	unsigned int new_pop_size = 0;
	
	for(unsigned int i = 0; i < n_gens; ++i){
		
		// Distribucion de alleles de nueva generacion
		alleles_new.clear();
		new_pop_size = 0;
		for(auto allele : alleles_old){
			double frec = allele.second;
			frec /= old_pop_size;
			binomial_distribution<> inds_dist(old_pop_size, frec);
			alleles_new[allele.first] = inds_dist(generator);

//			double bin_continuos = 0.0;
//			for(unsigned int j = 0; j < bin_trials; ++j){
//				bin_continuos += inds_dist(generator);
//			}
//			bin_continuos /= bin_trials;
//			alleles_new[allele.first] = static_cast<unsigned int>(round(bin_continuos));
			
			new_pop_size += alleles_new[allele.first];
//			cout << "bin_continuos[" << allele.second << "] -> " << bin_continuos << " (" << alleles_new[allele.first] << ")\n";
			if( alleles_new[allele.first] == 0 ){
				alleles_new.erase(allele.first);
			}
		}
		
		cout << "pop_size: " << old_pop_size << " -> " << new_pop_size << "\n";
		
		// Mutaciones
//		unsigned int n_muts = muts_dist(generator);
//		total_muts += n_muts;
//		cout << "BinomialTest - n_muts: " << n_muts << "\n";
//		for(unsigned int j = 0; j < n_muts; ++j){
//			unsigned int pos = inds_dist(generator);
//			unsigned int acum = 0;
//			unsigned int id_decrease = 0;
//			for(auto allele : alleles_old){
//				acum += allele.second;
//				if( pos < acum ){
//					id_decrease = allele.first;
//					break;
//				}
//			}
//			cout << "id_decrease: " << id_decrease << " -> " << new_id << " (" << alleles_new[id_decrease] << "--)\n";
//			alleles_new[id_decrease]--;
//			if( alleles_new[id_decrease] == 0 ){
//				alleles_new.erase(id_decrease);
//			}
//			alleles_new[new_id++]++;
//		}
		
//		cout << "BinomialTest - Alleles Old\n";
//		for(auto par : alleles_old){
//			cout << par.first << " -> " << par.second << "\n";
//		}
//		
//		cout << "BinomialTest - Alleles New\n";
//		for(auto par : alleles_new){
//			cout << par.first << " -> " << par.second << "\n";
//		}
		
		cout << "BinomialTest - Conversion a Passed\n";
		for(auto par : alleles_old){
			unsigned int id = par.first;
			unsigned int n_old = par.second;
			unsigned int n_new = 0;
			auto it = alleles_new.find(id);
			if(it != alleles_new.end()){
				n_new = it->second;
			}
			passed_bin.emplace(n_old, vector<unsigned int>());
			passed_bin[n_old].push_back(n_new);
//			cout << n_old << " -> " << n_new << "\n";
		}
		
		map<unsigned int, unsigned int> tmp = alleles_old;
		alleles_old = alleles_new;
		alleles_new = tmp;
		
		old_pop_size = new_pop_size;
		
		cout << "-----\n";
		
	}
	
	cout << "BinomialTest - Passed Binomial (total_muts: " << total_muts << ")\n";
	for(auto par : passed_bin){
		cout << par.first << " -> ";
		double mean = 0.0;
		for( unsigned int n : par.second ){
			mean += n;
//			cout << n << " ";
		}
		mean /= par.second.size();
		double var = 0.0;
		for( unsigned int n : par.second ){
			double nd = n;
			var += (nd-mean) * (nd-mean);
		}
		var /= par.second.size();
		cout << "mean: " << mean << ", var: " << var << " ";
		cout << "\n";
	}
	
	
	
	
	cout << "BinomialTest - Fin\n";
	
	delete [] ids_old;
	delete [] ids_new;
	
	return 0;
	
}















