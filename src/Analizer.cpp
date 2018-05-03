#include "Analizer.h"

Analizer::Analizer(){
	manager = NULL;
	n_threads = 0;
	percentage = 0;
	file_base = "";
	target_base = "";
	results_base = "";
}

Analizer::Analizer(WorkManager *_manager, unsigned int _n_threads, float _percentage, string _file_base, string _target_base, string _results_base){
	manager = _manager;
	n_threads = _n_threads;
	percentage = _percentage;
	file_base = _file_base;
	target_base = _target_base;
	results_base = _results_base;
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
	
	ResultsReader reader;
	reader.readThreadsData(sim_file_base, n_threads, n_stats, n_params);
	
	// Potencialmente agrupar los archivos en un solo archivo consolidado (equivalente al de generate_statistics)
	
	// Cargar el target (o pedirlo al manager, aun no esta decidido)
	string target_file = target_base;
	target_file += to_string(sim_id);
	target_file += ".txt";
	reader.setTarget(target_file);
	
	// Normaliza tanto los datos como el target
	reader.normalize();
	
	// Calcula la distancia de cada dato normalizado al target
	reader.computeDistances();
	
	// Ordena por distancia y selecciona los mejores resultados para el training
	reader.selectBestResults(percentage);
	
	// Retorna pares <mean, stddev> de cada parametro calculado de los datos seleccionados
	vector< pair<double, double> > dist_post = reader.getPosteriori();
	
	// Guardar resultados (para registro en caso de seguir entrenando, o como resultado final)
	// Notar que el servicio de query deberia usar los datos disponibles para calcular online
	// En ese caso, es posible que haya datos incompletos, simplemente los omito en ese caso
	// Tambien notar que para guardar los resultados seria util darle nombre a cada parametro
	// El metodo siguiente asume el mismo orden de lectura en el builder
	vector<string> params_names = manager->getParams(sim_id);
	unsigned int feedback = manager->getFeedback(sim_id);
	
	assert( params_names.size() == dist_post.size() );
	
	string results_file = results_base;
	results_file += to_string(sim_id);
	results_file += "_f";
	results_file += feedback;
	results_file += ".txt";
	
	fstream writer(results_file, fstream::out | fstream::trunc);
	for( unsigned int i = 0; i < params_names.size(); ++i ){
		writer << params_names[i] << "\t";
		writer << dist_post[i].first << "\t";
		writer << dist_post[i].second << "\t";
		writer << "\n";
	}
	writer.close();
	
	// Esta primera version, simplemente realiza una unica pasada de feedback, asi de simple
	// El batch size se determina al principio (como parametro, por el servicio de inicio, o fijo por el demonio)
	// De este modo, siempre se realizaran 2 * batch_size simulaciones
	if( feedback == 0 ){
		// Actualizar distribuciones y agregar nuevas simulaciones
		
	}
	else{
		// los resultados ya estan guardados, bastaria con terminar aqui
		
	}
	
	
}












