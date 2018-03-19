
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <vector>

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"
#include "Statistics.h"
#include "ResultsReader.h"

using namespace std;

// Mutex global para controlar acceso a cout u otros objetos compartidos
mutex global_mutex;
// Cola de trabajo consumida por los threads procesadores

// vector<Simulator*> work_queue;
vector<char*> work_queue;
unsigned int global_pos = 0;

vector<double> get_statistics(Simulator *sim, float sampling){
	vector<double> res;
	Statistics stats(sim, sampling);
	map<string, vector< map<string, double> > > statistics = stats.getStatistics();
	// Agrupo los estadisticos en el mismo orden, population_name primero, n marcadores, y stat name al final
	for( auto &par : statistics ){
		for( map<string, double> &marker_stats : par.second ){
			for( auto &stat : marker_stats ){
				res.push_back(stat.second);
			}
		}
	}
	return res;
}

vector<double> get_params(Simulator *sim){
	vector<double> res;
	
	// No voy a comparar parametros punto a punto con los datos antiguos, solo las distribuciones
	// Por esto, en este caso el orden NO esta atado a los nombres json de las variables
	// Simplemente extraere los parametros del profile, y luego de cada evento
	// Esto asume obviamente que todas las simulaciones comparadas tienen los mismos parametros
	
	Profile *profile = sim->getProfile();
	for(unsigned int i = 0; i < profile->getNumMarkers(); ++i){
		ProfileMarker marker = profile->getMarker(i);
		for(unsigned int j = 0; j < marker.getNumParam(); ++j){
			res.push_back( marker.getParam(j) );
		}
	}
	
	// Obviamente en el caso de los eventos solo considero la generacion y los parametros numericos
	EventList *events = sim->getEvents();
	for(unsigned int i = 0; i < events->size(); ++i){
		Event *event = events->getEvent(i);
		res.push_back( (double)(event->getGeneration()) );
		for( double value : event->getNumParams() ){
			res.push_back(value);
		}
	}
	
	return res;
}

void SimultionThread(unsigned int pid, unsigned int n_threads, string output_base){
	
	global_mutex.lock();
	cout << "SimultionThread["<<pid<<"] - Inicio\n";
	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	unsigned int cur_pos = 0;
	
	string file_name = output_base;
	file_name += std::to_string(pid);
	file_name += ".txt";
	
	unsigned int max_storage = 10;
	vector< vector<double> > statistics_storage;
	vector< vector<double> > params_storage;
	
	while(true){
		global_mutex.lock();
		cur_pos = global_pos++;
		global_mutex.unlock();
		
		if( cur_pos >= work_queue.size() ){
			global_mutex.lock();
			cout << "SimultionThread["<<pid<<"] - Cola de trabajo vacia, saliendo\n";
			global_mutex.unlock();
			break;
		}
		
		char *serialized = work_queue[cur_pos];
		Simulator sim;
		sim.loadSerialized(serialized);
		++procesados;
		
		sim.run();
		
		// Parte local del analyzer
		// Esto requiere el target 
		// Falta definir e implementar la normalizacion
		
		vector<double> statistics = get_statistics(&sim, 0.05);
		vector<double> params = get_params(&sim);
		
		statistics_storage.push_back(statistics);
		params_storage.push_back(params);
		
		if( statistics_storage.size() >= max_storage ){
			
//			global_mutex.lock();
//			cout << "SimultionThread["<<pid<<"] - statistics_storage.size: "<<statistics_storage.size()<<", guardando...\n";
//			global_mutex.unlock();
			
			fstream writer(file_name, fstream::out | fstream::app);
			for(unsigned int i = 0; i < statistics_storage.size(); ++i){
				for( double value : statistics_storage[i] ){
					writer << value << "\t";
				}
				for( double value : params_storage[i] ){
					writer << value << "\t";
				}
				writer << "\n";
			}
			writer.close();
			statistics_storage.clear();
			params_storage.clear();
		}
		
	}
	
	// Si quedaron resultados pendientes, los guardo aqui
	if( statistics_storage.size() > 0 ){
		
//		global_mutex.lock();
//		cout << "SimultionThread["<<pid<<"] - statistics_storage.size: "<<statistics_storage.size()<<", guardando...\n";
//		global_mutex.unlock();
		
		fstream writer(file_name, fstream::out | fstream::app);
		for(unsigned int i = 0; i < statistics_storage.size(); ++i){
			for( double value : statistics_storage[i] ){
				writer << value << "\t";
			}
			for( double value : params_storage[i] ){
				writer << value << "\t";
			}
			writer << "\n";
		}
		writer.close();
		statistics_storage.clear();
		params_storage.clear();
	}
	
	global_mutex.lock();
	cout << "SimultionThread["<<pid<<"] - Fin (Total trabajos: "<<procesados<<", Total ms: "<<timer.getMilisec()<<")\n";
	global_mutex.unlock();
	
}

// Notar que recibo una COPIA del target para normalizar de modo independiente
vector<pair<double, double>> getDistributions(string &feedback_output, unsigned int n_threads, unsigned int n_stats, unsigned int n_params, float f_training, string &target_file){
	
	ResultsReader reader;
	reader.readThreadsData(feedback_output, n_threads, n_stats, n_params);
	
	// Normalizar los datos y el target
	reader.setTarget(target_file);
	reader.normalize();
	
	// Calcular distancia y agregar a cada dato
	reader.computeDistances();
	
	// Ordenar por distancia y escoger los mejores
	reader.selectBestResults(f_training);
	
	// Calcular estadisticos de cada parametro del muestreo
	// Generar resultado con medias y stddev de cada parametro
	vector< pair<double, double> > distributions = reader.getPosteriori();
	
	return distributions;
	
}

int main(int argc,char** argv){

	if(argc != 10){
		cout << "\nUsage: ./test json_file total_jobs max_feedback n_threads output_base n_stats n_params f_training target_file\n";
		cout << "\n";
		return 0;
	}
	
	string settings_file = argv[1];
	unsigned int total = atoi(argv[2]);
	unsigned int max_feedback = atoi(argv[3]);
	unsigned int n_threads = atoi(argv[4]);
	string output_base = argv[5];
	unsigned int n_stats = atoi(argv[6]);
	unsigned int n_params = atoi(argv[7]);
	float f_training = atof(argv[8]);
	string target_file = argv[9];
	
	cout << "Test - Inicio (total: "<<total<<", n_threads: "<<n_threads<<", output_base: "<<output_base<<")\n";
	
//	cout << "Test - Revisando trabajos terminados\n";
//	unsigned int terminados = 0;
//	unsigned int buff_size = 1020*1024;
//	char buff[buff_size];
//	for(unsigned int pid = 0; pid < n_threads; ++pid){
//		string file_name = output_base;
//		file_name += std::to_string(pid);
//		file_name += ".txt";
//		fstream reader(file_name, fstream::in);
//		
//		while(true){
//			reader.getline(buff, buff_size);
//			if( !reader.good() || strlen(buff) < 1 ){
//				break;
//			}
//			++terminados;
//		}
//		
//		reader.close();
//	}
//	if(terminados < total){
//		total -= terminados;
//	}
//	else{
//		total = 0;
//	}
//	cout << "Test - Terminados: "<<terminados<<", Total efectivo: "<<total<<"\n";
	
	cout << "Test - Preparando Cola de Trabajo\n";
	SimulatorFactory factory(settings_file);
	
	// Iterar por max_feedback
	for( unsigned int feedback = 0; feedback < max_feedback; ++feedback ){
		
		NanoTimer timer;
		string feedback_output = output_base;
		feedback_output += "f" + to_string(feedback) + "_";
	
		cout << "Test - Agregando " << total << " jobs a la cola, feedback " << feedback << "\n";
		for(unsigned int i = 0; i < total; ++i){
			work_queue.push_back(factory.getInstanceSerialized());
		}
		double ms_preparation = timer.getMilisec();
		cout << "Test - Preparacion terminada en " << ms_preparation << " ms\n";
		
		
		cout << "Test - Iniciando " << n_threads << " threads\n";
		vector<thread> threads_list;
		for(unsigned int i = 0; i < n_threads; ++i){
			threads_list.push_back( thread(SimultionThread, i, n_threads, feedback_output) );
		}
		for(unsigned int i = 0; i < n_threads; ++i){
			threads_list[i].join();
		}
		double ms_processing = timer.getMilisec();
		cout << "Test - Procesamiento terminado en "<<(ms_processing - ms_preparation)<<" ms\n";
		
		// Training
		cout << "Test - Procesando datos para entrenamiento\n";
		vector<pair<double, double>> distributions = getDistributions(feedback_output, n_threads, n_stats, n_params, f_training, target_file);
		
		cout << "Test - Actualizando Parametros\n";
		factory.reloadParameters(distributions);
		
		
		
		
		cout << "Test - Vaciando cola de trabajo\n";
		for(unsigned int i = 0; i < total; ++i){
			delete [] work_queue[i];
		}
		work_queue.clear();
		
	}
	
	cout << "Test - Fin\n";
	
	
	return 0;
}













