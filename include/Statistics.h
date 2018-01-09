#ifndef _STATISTICS_H_
#define _STATISTICS_H_

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
	
};

#endif
