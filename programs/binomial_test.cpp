
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
	
	unsigned int n_inds = 100000;
	unsigned int n_gens = 10000;
	float mut_rate = 0.00001;
	unsigned int n_sims = 1000;
	string direct_data_file = "simulations_direct.txt";
	string binomial_data_file = "simulations_binomial.txt";
	fstream writer(direct_data_file, fstream::out | fstream::trunc);
	writer.close();
	writer.open(binomial_data_file, fstream::out | fstream::trunc);
	writer.close();
	
	
	cout << "BinomialTest - Inicio (pop_size: " << n_inds << ", n_gens: " << n_gens << ", n_sims: " << n_sims << ")\n";
	
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
	unsigned int new_id = 0;
	
	// Estrucutras para almacenar los estadisticos por simulacion
	vector<double> est1_direct;
	vector<double> est2_direct;
	vector<double> est3_direct;
	vector<double> est1_binomial;
	vector<double> est2_binomial;
	vector<double> est3_binomial;
	pair<double, double> par_stat;
	NanoTimer timer;
	
	cout << "----- Probando Direct Simulation -----\n";
	
	for(unsigned int sim = 0; sim < n_sims; ++sim){
		total_muts = 0;
		
		// Generacion inicial V1: todos diferentes
		new_id = 0;
		for(unsigned int i = 0; i < n_inds; ++i){
			ids_old[i] = new_id++;
		}
		
		timer.reset();
		for(unsigned int i = 0; i < n_gens; ++i){
			// Seleccion de padres
			for(unsigned int j = 0; j < n_inds; ++j){
				unsigned int pos = inds_dist(generator);
				ids_new[j] = ids_old[pos];
			}
			// Mutaciones
			unsigned int n_muts = muts_dist(generator);
			total_muts += n_muts;
			// cout << "BinomialTest - n_muts: " << n_muts << "\n";
			for(unsigned int j = 0; j < n_muts; ++j){
				unsigned int pos = inds_dist(generator);
				// cout << ids_new[pos] << " -> " << new_id << "\n";
				ids_new[pos] = new_id++;
			}
			unsigned int *tmp = ids_old;
			ids_old = ids_new;
			ids_new = tmp;
			// cout << "-----\n";
		}
		
		// Mapas de alelos: id -> n_inds
		// Notar que tomo de ids_old, por el swap al final de cada generacion
		map<unsigned int, unsigned int> alleles;
		for(unsigned int j = 0; j < n_inds; ++j){
			alleles[ids_old[j]]++;
		}
		
		// Calculo de estadisticos de alleles para la generacion final
		double num_alleles = alleles.size();
		double heterozygosity = statHeterozygosity(alleles);
		double effective_num_alleles = statEffectiveNumAlleles(heterozygosity);
		est1_direct.push_back(num_alleles);
		est2_direct.push_back(heterozygosity);
		est3_direct.push_back(effective_num_alleles);
		
		cout << "BinomialTest - Directo en " << timer.getMilisec() << " ms (" << num_alleles << ", " << heterozygosity << ", " << effective_num_alleles << ")\n";
		fstream writer(direct_data_file, fstream::out | fstream::app);
		writer << num_alleles << "\t" << heterozygosity << "\t" << effective_num_alleles << "\n";
		writer.close();
	}
	
	cout << "----- Probando Binomial Simulation -----\n";
	
	for(unsigned int sim = 0; sim < n_sims; ++sim){
		map<unsigned int, unsigned int> alleles_new;
		map<unsigned int, unsigned int> alleles_old;
		total_muts = 0;
	
		// Generacion inicial V1: todos diferentes
		new_id = 0;
		for(unsigned int i = 0; i < n_inds; ++i){
			alleles_old[new_id++] = 1;
		}
		unsigned int old_pop_size = n_inds;
		unsigned int new_pop_size = 0;
	
		timer.reset();
		for(unsigned int i = 0; i < n_gens; ++i){
			// Distribucion de alleles de nueva generacion
			alleles_new.clear();
			new_pop_size = 0;
			for(auto allele : alleles_old){
				double frec = allele.second;
				frec /= old_pop_size;
				// Correccion de escala v1 (pre binomial)
				frec = (frec * n_inds) / (double)old_pop_size;
				binomial_distribution<> inds_binomial(old_pop_size, frec);
				alleles_new[allele.first] = inds_binomial(generator);
				// Correccion de escala v2 (post binomial)
				// alleles_new[allele.first] = (unsigned int)(((double)alleles_new[allele.first] * n_inds) / (double)old_pop_size);
				new_pop_size += alleles_new[allele.first];
				if( alleles_new[allele.first] == 0 ){
					alleles_new.erase(allele.first);
				}
			}
			// cout << "pop_size: " << old_pop_size << " -> " << new_pop_size << "\n";
			// Mutaciones
			unsigned int n_muts = muts_dist(generator);
			total_muts += n_muts;
			uniform_int_distribution<> inds_dist_adjusted(0, new_pop_size - 1);
			// cout << "BinomialTest - n_muts: " << n_muts << "\n";
			for(unsigned int j = 0; j < n_muts; ++j){
				unsigned int pos = inds_dist_adjusted(generator);
				unsigned int acum = 0;
				unsigned int id_decrease = alleles_new.rbegin()->first;
				for(auto allele : alleles_new){
					acum += allele.second;
					if( pos < acum ){
						id_decrease = allele.first;
						break;
					}
				}
				// cout << "id_decrease: " << id_decrease << " -> " << new_id << " (" << alleles_new[id_decrease] << "--)\n";
				alleles_new[id_decrease]--;
				if( alleles_new[id_decrease] == 0 ){
					alleles_new.erase(id_decrease);
				}
				alleles_new[new_id++]++;
			}
			alleles_old.swap(alleles_new);
			old_pop_size = new_pop_size;
			// cout << "-----\n";
		}
	
		// Notar que tomo old, por el swap al final de cada generacion
		double num_alleles_bin = alleles_old.size();
		double heterozygosity_bin = statHeterozygosity(alleles_old);
		double effective_num_alleles_bin = statEffectiveNumAlleles(heterozygosity_bin);
		est1_binomial.push_back(num_alleles_bin);
		est2_binomial.push_back(heterozygosity_bin);
		est3_binomial.push_back(effective_num_alleles_bin);
		
		cout << "BinomialTest - Binomial en " << timer.getMilisec() << " ms (" << num_alleles_bin << ", " << heterozygosity_bin << ", " << effective_num_alleles_bin << ")\n";
		fstream writer(binomial_data_file, fstream::out | fstream::app);
		writer << num_alleles_bin << "\t" << heterozygosity_bin << "\t" << effective_num_alleles_bin << "\n";
		writer.close();
	}
	
	cout << "----- Resultados Direct -----\n";
	
	par_stat = getStatPair(est1_direct);
	cout << "BinomialTest - Direct Stat 1: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est2_direct);
	cout << "BinomialTest - Direct Stat 2: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est3_direct);
	cout << "BinomialTest - Direct Stat 3: (" << par_stat.first << ", " << par_stat.second << ")\n";
	
	cout << "----- Resultados Binomial -----\n";
	
	par_stat = getStatPair(est1_binomial);
	cout << "BinomialTest - Binomial Stat 1: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est2_binomial);
	cout << "BinomialTest - Binomial Stat 2: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est3_binomial);
	cout << "BinomialTest - Binomial Stat 3: (" << par_stat.first << ", " << par_stat.second << ")\n";
	
	
	cout << "BinomialTest - Fin\n";
	
	delete [] ids_old;
	delete [] ids_new;
	
	return 0;
	
}















