#include "Analizer.h"

Analizer::Analizer(){
	manager = NULL;
	n_threads = 0;
	file_base = "";
}

Analizer::Analizer(WorkManager *_manager, unsigned int _n_threads, string _file_base, string _target_base){
	manager = _manager;
	n_threads = _n_threads;
	file_base = _file_base;
	target_base = _target_base;
}

Analizer::~Analizer(){
	// notar que el manager es del daemon
	manager = NULL;
}

void Analizer::execute(unsigned int sim_id){

	assert( manager != NULL);
	assert( n_threads > 0);

	// Leer los archivos de los threads de la simulacion
	string sim_file_base = file_base;
	sim_file_base += to_string(sim_id);
	sim_file_base += "_";
	
	unsigned int n_stats = manager->getNumStatistics(sim_id);
	unsigned int n_params = manager->getNumParams(sim_id);
	
	ResultsReader results;
	results.readThreadsData(sim_file_base, n_threads, n_stats, n_params);
	
	// Potencialmente agrupar los archivos en un solo archivo consolidado (equivalente al de generate_statistics)
	
	// Cargar el target (o pedirlo al manager, aun no esta decidido)
	string target_file = target_base;
	target_file += to_string(sim_id);
	results.setTarget(target_file);
	
	
	
}












