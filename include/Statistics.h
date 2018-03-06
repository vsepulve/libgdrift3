#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include <set>
#include <map>
#include <vector>
#include <random>
#include <assert.h>

#include "NanoTimer.h"
#include "Population.h"
#include "Profile.h"
#include "Individual.h"
#include "Simulator.h"

using namespace std;

class Statistics{

private:
//	map<string, map<string, double> > statistics;
	// Mapa de population_name -> vector de estadisticos (por cada marcador)
	// Los estadisticos tienen nombre -> valor
	map<string, vector< map<string, double> > > statistics;
	// Notar que es un vector de tablas, una por cada marcador
	vector<map<unsigned int, string>> alleles_tables;
	
	// Mapa de mutaciones aplicadas a cada alelo
	// TODO: este mapa solo es valido para datos de tipo secuencia
	vector<map<unsigned int, map<unsigned int, char> >> alleles_mutations_tables;
	
	// Notar que el profile y el pool son de la simulacion, guardo punteros por comodidad
	Profile *profile;
	Pool *pool;
	mt19937 generator;
	
	map<char, vector<char> > mutation_table;
	
	const static unsigned int min_sampling = 50;

public:

	Statistics();
	Statistics(Simulator *sim, float sampling);
//	Statistics(const Statistics &original);
//	Statistics& operator=(const Statistics& original);
//	virtual Statistics *clone();
	virtual ~Statistics();
	
	// Procesa TODOS los estadisticos y los agrega a statistics[name][stat]
	void processStatistics(Population *pop, string name, float sampling);
	
	// Busca el alelo en la tabla, lo genere recursivamente si no lo encuentra
	string &getAllele(unsigned int marker_pos, unsigned int id, ProfileMarker &marker);
	
	// Notar que este metodo retorna una COPIA del nuevo string
	string generateAllele(unsigned int marker_pos, ProfileMarker &marker);
	
	map<string, vector< map<string, double> > > &getStatistics(){
		return statistics;
	}
	
	// Metodos de estadisticos particulares
	// Estos son estaticos, pues no requieren el contexto de la instancia
	// Tambien notar que algunos de estos pueden preferir los vectores de mutaciones en lugar de los textos
	
	static vector<unsigned int> statPairwiseDifferences(vector<string> &alleles);
	
	// Esta version se basa en el mapa de mutaciones, EXCLUSIVO para secuencias
	static vector<unsigned int> statPairwiseDifferencesMutations(vector< map<unsigned int, char> > &alleles);
	
	static double statNumHaplotypes(vector<string> &alleles);
		
	// "number-of-segregating-sites"
	static double statNumSegregatingSites(vector<string> &alleles);
	
	// "mean-of-the-number-of-pairwise-differences"
	static double statMeanPairwiseDifferences(vector<unsigned int> &differences);
	
	// "variance-of-the-number-of-pairwise-differences"
	static double statVariancePairwiseDifferences(vector<unsigned int> &differences, double mean_pairwise_diff);
	
	// "tajima-d-statistics"
	static double statTajimaD(vector<string> &alleles, double num_segregating_sites, double mean_pairwise_diff);
	
};

#endif
