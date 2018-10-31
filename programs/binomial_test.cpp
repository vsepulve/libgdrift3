
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

	if(argc != 6){
		cout << "\nUsage: ./binomial_test pop_size n_gens mut_rate n_sims output_base\n";
		cout << "The program writes the raw statistics per simulation in output_base[_direct.txt|_binomial.txt]\n";
		cout << "The 4 statistics used are: num_alleles, heterozygosity, effective_num_alleles, milisecs\n";
		cout << "\n";
		return 0;
	}
	unsigned int n_inds = atoi(argv[1]);
	unsigned int n_gens = atoi(argv[2]);
	float mut_rate = atof(argv[3]);
	unsigned int n_sims = atoi(argv[4]);
	string output_base = argv[5];
	string direct_data_file = output_base + "_direct.txt";
	string binomial_data_file = output_base + "_binomial.txt";
	fstream writer(direct_data_file, fstream::out | fstream::trunc);
	writer.close();
	writer.open(binomial_data_file, fstream::out | fstream::trunc);
	writer.close();
	
	unsigned int n_initial_inds = 10;
	
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
	vector<double> est4_direct;
	vector<double> est1_binomial;
	vector<double> est2_binomial;
	vector<double> est3_binomial;
	vector<double> est4_binomial;
	pair<double, double> par_stat;
	NanoTimer timer;
	
	cout << "----- Probando Direct Simulation -----\n";
	
	for(unsigned int sim = 0; sim < n_sims; ++sim){
		total_muts = 0;
		
		// Generacion inicial V1: todos diferentes
		new_id = 0;
		for(unsigned int i = 0; i < n_inds; ++i){
			ids_old[i] = new_id;
			if( i % n_initial_inds == 0 ){
				++new_id;
			}
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
		double milisec = timer.getMilisec();
		est1_direct.push_back(num_alleles);
		est2_direct.push_back(heterozygosity);
		est3_direct.push_back(effective_num_alleles);
		est4_direct.push_back(milisec);
		
		cout << "BinomialTest - Directo en " << milisec << " ms (" << num_alleles << ", " << heterozygosity << ", " << effective_num_alleles << ", total_muts: " << total_muts << ")\n";
		fstream writer(direct_data_file, fstream::out | fstream::app);
		writer << num_alleles << "\t" << heterozygosity << "\t" << effective_num_alleles << "\t" << milisec << "\n";
		writer.close();
	}
	
	cout << "----- Probando Binomial Simulation -----\n";
	
	for(unsigned int sim = 0; sim < n_sims; ++sim){
		map<unsigned int, unsigned int> alleles_new;
		map<unsigned int, unsigned int> alleles_old;
		total_muts = 0;
	
		// Generacion inicial V1: todos diferentes
		new_id = 0;
		for(unsigned int i = 0; i < n_inds/n_initial_inds; ++i){
			alleles_old[new_id++] = n_initial_inds;
		}
		unsigned int old_pop_size = n_initial_inds * (n_inds/n_initial_inds);
		unsigned int new_pop_size = 0;
		
		timer.reset();
		for(unsigned int i = 0; i < n_gens; ++i){
			// Distribucion de alleles de nueva generacion
			alleles_new.clear();
			new_pop_size = 0;
			unsigned int total_inds_mut = 0;
			for(auto allele : alleles_old){
				double frec = allele.second;
				frec /= old_pop_size;
				// Correccion de escala v1 (pre binomial)
				frec = (frec * n_inds) / (double)old_pop_size;
				binomial_distribution<> inds_binomial(old_pop_size, frec);
				unsigned int inds_allele = inds_binomial(generator);
				// Correccion de escala v2 (post binomial)
				// inds_allele = (unsigned int)(((double)inds_allele * n_inds) / (double)old_pop_size);
				
				
				// Mutaciones V3: Estimacion de mutaciones por alelo
				binomial_distribution<> muts_dist_allele(inds_allele, mut_rate);
				unsigned int n_muts = muts_dist_allele(generator);
				if( n_muts > inds_allele ){
					n_muts = inds_allele;
				}
				inds_allele -= n_muts;
				total_inds_mut += n_muts;
				total_muts += n_muts;
				
				
				
				alleles_new[allele.first] = inds_allele;
				new_pop_size += alleles_new[allele.first];
				if( alleles_new[allele.first] == 0 ){
					alleles_new.erase(allele.first);
				}
			}
			
			// Mutaciones V3 Parte 2: aplicacion de mutaciones acumuladas
			for( unsigned int j = 0; j < total_inds_mut; ++j ){
				alleles_new[new_id++] = 1;
				++new_pop_size;
			}
			
//			cout << "pop_size: " << old_pop_size << " -> " << new_pop_size << "\n";
			// Mutaciones
//			unsigned int n_muts = muts_dist(generator);
//			total_muts += n_muts;
//			uniform_int_distribution<> inds_dist_adjusted(0, new_pop_size - 1);
//			cout << "BinomialTest - n_muts: " << n_muts << "\n";
			
			/*
			// Mutaciones V1: Iterar por aleleos para cada mutacion
			// Con esta implementacion, el costo de las mutaciones O(n_muts * n_alelos)
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
			*/
			
			/*
			// Mutaciones V2: acumulando alelos y busqueda binaria
			// Esta version es de costo O(n_alelos + n_muts * log(n_alelos))
			// Notar que esta version es aproximada, porque la tabla acumulada se mantiene estatica tras mutaciones
			vector<pair<unsigned int, unsigned int>> acum_alleles;
			if(n_muts > 0){
				unsigned int last = 0;
//				unsigned int pos = 0;
				for(auto allele : alleles_new){
					last += allele.second;
					acum_alleles.push_back( pair<unsigned int, unsigned int>(last, allele.first) );
//					cout << "acum_alleles[" << pos++ << "]: " << last << " (por " << allele.second << ", id: " << allele.first << ")\n";
				}
			}
			
			for(unsigned int j = 0; j < n_muts; ++j){
				unsigned int pos = inds_dist_adjusted(generator);
//				cout << "pos: " << pos << "\n";
				unsigned int l = 0;
				unsigned int h = acum_alleles.size() - 1;
				unsigned int m;
				while(l < h){
					m = l + ((h-l)>>1);
					if( pos >= acum_alleles[m].first ){
						l = m+1;
					}
					else{
						h = m;
					}
				}
				m = h;
				if( (m > 0) && (pos == acum_alleles[m].first) ){
					--m;
				}
//				cout << "pos: " << pos << " -> " << m << " (acum: " << acum_alleles[m].first << ", id: " << acum_alleles[m].second << ")\n";
				
				// Notar que el arreglo acumulado es estatico, pero los alleles_new sin dinamicos
				// Eso puede generar que se escoga un allelo sin representantes
				// En ese caso, omito la mutacion por seguridad (esto deberia ser muy infrecuente)
				if( alleles_new.find(acum_alleles[m].second) == alleles_new.end() ){
					continue;
				}
				alleles_new[ acum_alleles[m].second ]--;
				if( alleles_new[ acum_alleles[m].second ] == 0 ){
					alleles_new.erase( acum_alleles[m].second );
				}
				alleles_new[new_id++]++;
			}
			*/
			
			alleles_old.swap(alleles_new);
			old_pop_size = new_pop_size;
			// cout << "-----\n";
		}
	
		// Notar que tomo old, por el swap al final de cada generacion
		double num_alleles = alleles_old.size();
		double heterozygosity = statHeterozygosity(alleles_old);
		double effective_num_alleles = statEffectiveNumAlleles(heterozygosity);
		double milisec = timer.getMilisec();
		est1_binomial.push_back(num_alleles);
		est2_binomial.push_back(heterozygosity);
		est3_binomial.push_back(effective_num_alleles);
		est4_binomial.push_back(milisec);
		
		cout << "BinomialTest - Binomial en " << milisec << " ms (" << num_alleles << ", " << heterozygosity << ", " << effective_num_alleles << ", total_muts: " << total_muts << ")\n";
		fstream writer(binomial_data_file, fstream::out | fstream::app);
		writer << num_alleles << "\t" << heterozygosity << "\t" << effective_num_alleles << "\t" << milisec << "\n";
		writer.close();
	}
	
	cout << "----- Resultados Direct -----\n";
	
	par_stat = getStatPair(est1_direct);
	cout << "BinomialTest - Direct Stat 1: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est2_direct);
	cout << "BinomialTest - Direct Stat 2: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est3_direct);
	cout << "BinomialTest - Direct Stat 3: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est4_direct);
	cout << "BinomialTest - Direct Stat 4 (Time): (" << par_stat.first << ", " << par_stat.second << ")\n";
	
	cout << "----- Resultados Binomial -----\n";
	
	par_stat = getStatPair(est1_binomial);
	cout << "BinomialTest - Binomial Stat 1: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est2_binomial);
	cout << "BinomialTest - Binomial Stat 2: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est3_binomial);
	cout << "BinomialTest - Binomial Stat 3: (" << par_stat.first << ", " << par_stat.second << ")\n";
	par_stat = getStatPair(est4_binomial);
	cout << "BinomialTest - Binomial Stat 4 (Time): (" << par_stat.first << ", " << par_stat.second << ")\n";
	
	
	cout << "BinomialTest - Fin\n";
	
	delete [] ids_old;
	delete [] ids_new;
	
	return 0;
	
}















