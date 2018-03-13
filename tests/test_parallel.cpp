
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <vector>

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"
#include "Statistics.h"

using namespace std;

// Mutex global para controlar acceso a cout u otros objetos compartidos
// Este mutex debiera pertenecer al Controller en el modelo real
mutex global_mutex;

void SimultionThread(unsigned int pid, const char *settings_file) {
	
	// Lock completo para una version secuencial de prueba
//	lock_guard<mutex> lock(global_mutex);
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Inicio\n";
	global_mutex.unlock();
	
	NanoTimer timer;
	SimulatorFactory factory(settings_file);
	Simulator *sim = factory.getInstance();
	sim->run();
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Simulator->run Terminado en "<<timer.getMilisec()<<" ms\n";
	global_mutex.unlock();
	
	// Extrer estadisticos
	vector<double> res;
	Statistics stats(sim, 0.05);
	map<string, vector< map<string, double> > > statistics = stats.getStatistics();
	// Agrupo los estadisticos en el mismo orden, population_name primero, n marcadores, y stat name al final
	for( auto &par : statistics ){
//		unsigned int marker_pos = 0;
		for( map<string, double> &marker_stats : par.second ){
//			cout<<"get_statistics - [ Marker "<<marker_pos++<<" ]\n";
			for( auto &stat : marker_stats ){
//				cout<<"Test - " << stat.first << ": "<< stat.second <<"\n";
				res.push_back(stat.second);
			}
		}
	}
	// Aqui podrian usarse los estadisticos
	
	delete sim;
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Fin\n";
	global_mutex.unlock();
	
}

int main(int argc,char** argv)
{

	if(argc != 3){
		cout<<"\nUsage: ./test json_file n_threads\n";
		cout<<"\n";
		return 0;
	}

	const char *settings_file = argv[1];
	unsigned int n_threads = atoi(argv[2]);
	
	cout<<"Test - Inicio (Preparando "<<n_threads<<" threads)\n";
	NanoTimer timer;
	
	vector<std::thread> threads_list;
	for(unsigned int i = 0; i < n_threads; ++i){
		threads_list.push_back( std::thread(SimultionThread, i, settings_file) );
	}
	for(unsigned int i = 0; i < n_threads; ++i){
		threads_list[i].join();
	}
	
	cout<<"Test - Fin\n";
	
	return(0);
}













