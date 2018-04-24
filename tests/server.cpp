
#include <iostream>
#include <random>

#include <thread>
#include <mutex>

#include <vector>
#include <list>

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"
#include "Statistics.h"

using namespace std;

// Mutex global para controlar acceso a cout u otros objetos compartidos
mutex global_mutex;

// Cola de trabajo consumida por los threads procesadores
list<char*> work_queue;

void addWork(string &json_file, unsigned int n_sims){
	
	cout << "addWork - Inicio (" << n_sims << " sims desde \"" << json_file << "\")\n";
	
	SimulatorFactory factory(json_file);
	
	cout << "addWork - Factory preparado, tomando lock\n";
	lock_guard<mutex> lock(global_mutex);
	
	unsigned int old_size = work_queue.size();
	
	if( old_size == 0 ){
		// Llenado inicial
		cout << "addWork - Llenado directo, Cola de trabajo vacia\n";
		for( unsigned int i = 0; i < n_sims; ++i ){
			work_queue.push_back(factory.getInstanceSerialized());
		}
	}
	else if( old_size >= n_sims ){
		// Llenado disperso
		unsigned int s = (unsigned int)(old_size / n_sims);
		auto it = work_queue.begin();
//		unsigned int pos = 0;
		cout << "addWork - Llenado disperso (old_size: " << old_size << ", n_sims: " << n_sims << ", s: " << s << ")\n";
		for( unsigned int i = 0; i < n_sims; ++i ){
//			cout << "addWork - Insert en pos " << pos << "\n";
			it = work_queue.insert(it, factory.getInstanceSerialized());
			for( unsigned int j = 0; j < s+1; ++j ){
				it++;
//				++pos;
			}
		}
	}
	else{
		// Llenado denso
		unsigned int s = (unsigned int)(n_sims / old_size);
		unsigned int e = n_sims - old_size * s;
		auto it = work_queue.begin();
//		unsigned int pos = 0;
		cout << "addWork - Llenado denso (old_size: " << old_size << ", n_sims: " << n_sims << ", s: " << s << ", e: " << e << ")\n";
		for( unsigned int i = 0; i < old_size; ++i ){
			unsigned int limit = (i<e)?(s+1):(s);
			for( unsigned int j = 0; j < limit; ++j ){
//				cout << "addWork - Insert en pos " << pos << "\n";
				it = work_queue.insert(it, factory.getInstanceSerialized());
				it++;
//				++pos;
			}
			it++;
//			++pos;
		}
	}
	
	cout << "addWork - Fin (new size: " << work_queue.size() << ")\n";
	
}

vector<double> get_statistics(Simulator *sim, float sampling){
	vector<double> res;
	Statistics stats(sim, sampling);
	map<string, vector< map<string, double> > > statistics = stats.getStatistics();
	// Agrupo los estadisticos en el mismo orden, population_name primero, n marcadores, y stat name al final
	for( auto &par : statistics ){
//		cout<<"get_statistics - Stats Population \""<< par.first <<"\"\n";
//		unsigned int marker_pos = 0;
		for( map<string, double> &marker_stats : par.second ){
//			cout<<"get_statistics - [ Marker "<<marker_pos++<<" ]\n";
			for( auto &stat : marker_stats ){
//				cout<<"Test - " << stat.first << ": "<< stat.second <<"\n";
				res.push_back(stat.second);
			}
//			cout<<"-----\n";
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



// TODO: Ajustar para usar nueva lista de trabajo
void SimultionThread(unsigned int pid, unsigned int n_threads, string output_base){
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Inicio\n";
	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	
	string file_name = output_base;
	file_name += std::to_string(pid);
	file_name += ".txt";
	
	unsigned int sleep_time = 5;
	unsigned int max_storage = 10;
	vector< vector<double> > statistics_storage;
	vector< vector<double> > params_storage;
	
	while(true){
		
		char *serialized = NULL;
		
		global_mutex.lock();
		if( work_queue.empty() ){
			// Sleep
			cout<<"SimultionThread["<<pid<<"] - Durmiendo...\n";
			global_mutex.unlock();
			std::this_thread::sleep_for (std::chrono::seconds(sleep_time));
			continue;
		}
		else{
			cout<<"SimultionThread["<<pid<<"] - Tomando Simulacion...\n";
			serialized = work_queue.front();
			work_queue.pop_front();
			global_mutex.unlock();
		}
		
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
//			cout<<"SimultionThread["<<pid<<"] - statistics_storage.size: "<<statistics_storage.size()<<", guardando...\n";
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
//		cout<<"SimultionThread["<<pid<<"] - statistics_storage.size: "<<statistics_storage.size()<<", guardando...\n";
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
	cout<<"SimultionThread["<<pid<<"] - Fin (Total trabajos: "<<procesados<<", Total ms: "<<timer.getMilisec()<<")\n";
	global_mutex.unlock();
	
}



int main(int argc,char** argv){

	
	// Definiciones inciales
	
	// Preparacion de elementos compartidos
	
	// Preparacion y lanzamiento de Threads procesadores (con sleep interno)
	
	// Preparacion e inicio del sistema de comunicacion
	
	// Procesamiento de llamadas (de aqui en adelante queda escuchando como daemon)
	
	// Cada vez que recibe un nuevo json para procesar, lo agregar a la cola de trabajo
	
	
	string json = "/home/vsepulve/Escritorio/libgdrift3/data/settings_test.json";
	
	addWork(json, 4);
	cout << " ----- \n";
	
	addWork(json, 3);
	cout << " ----- \n";
	
	addWork(json, 3);
	cout << " ----- \n";
	
	addWork(json, 13);
	cout << " ----- \n";
	
	
}













