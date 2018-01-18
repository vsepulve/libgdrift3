
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <vector>

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"

using namespace std;

// Mutex global para controlar acceso a cout u otros objetos compartidos
mutex global_mutex;
// Cola de trabajo consumida por los threads procesadores
vector<Simulator*> work_queue;
unsigned int global_pos = 0;

void SimultionThread(unsigned int pid, unsigned int n_threads, string output_base){
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Inicio\n";
	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	unsigned int cur_pos = 0;
	
	string file_name = output_base;
	file_name += std::to_string(pid);
	file_name += ".txt";
	
//	unsigned int max_storage = 10;
//	vector< vector<double> > statistics_storage;
//	vector< vector<double> > params_storage;
	
	while(true){
		global_mutex.lock();
		cur_pos = global_pos++;
		global_mutex.unlock();
		
		if( cur_pos >= work_queue.size() ){
			global_mutex.lock();
			cout<<"SimultionThread["<<pid<<"] - Cola de trabajo vacia, saliendo\n";
			global_mutex.unlock();
			break;
		}
		
		Simulator *sim = work_queue[cur_pos];
		++procesados;
		
		sim->run();
		
		// Parte local del analyzer
		// Esto requiere el target 
		// Falta definir e implementar la normalizacion
		
//		if( sim->detectedErrors() == 0 ){
//			vector<double> statistics = get_statistics(sim.samples());
//			vector<double> params = get_params(fjob);
//			
//			statistics_storage.push_back(statistics);
//			params_storage.push_back(params);
//			
//			if( statistics_storage.size() >= max_storage ){
//				
////				global_mutex.lock();
////				cout<<"SimultionThread["<<pid<<"] - statistics_storage.size: "<<statistics_storage.size()<<", guardando...\n";
////				global_mutex.unlock();
//				
//				fstream writer(file_name, fstream::out | fstream::app);
//				for(unsigned int i = 0; i < statistics_storage.size(); ++i){
//					for( double value : statistics_storage[i] ){
//						writer << value << "\t";
//					}
//					for( double value : params_storage[i] ){
//						writer << value << "\t";
//					}
//					writer << "\n";
//				}
//				writer.close();
//				statistics_storage.clear();
//				params_storage.clear();
//			}
//			
//			
//		}
		
	}
	
	// Si quedaron resultados pendientes, los guardo aqui
//	if( statistics_storage.size() > 0 ){
//		
////		global_mutex.lock();
////		cout<<"SimultionThread["<<pid<<"] - statistics_storage.size: "<<statistics_storage.size()<<", guardando...\n";
////		global_mutex.unlock();
//		
//		fstream writer(file_name, fstream::out | fstream::app);
//		for(unsigned int i = 0; i < statistics_storage.size(); ++i){
//			for( double value : statistics_storage[i] ){
//				writer << value << "\t";
//			}
//			for( double value : params_storage[i] ){
//				writer << value << "\t";
//			}
//			writer << "\n";
//		}
//		writer.close();
//		statistics_storage.clear();
//		params_storage.clear();
//	}
	
	
	global_mutex.lock();
	cout<<"SimultionThread["<<pid<<"] - Fin (Total trabajos: "<<procesados<<", Total ms: "<<timer.getMilisec()<<")\n";
	global_mutex.unlock();
	
}

void DummyThread(unsigned int pid, unsigned int local_jobs){
	
	global_mutex.lock();
	cout<<"DummyThread["<<pid<<"] - Inicio\n";
	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	
	unsigned int n_gens = 100;
	unsigned int pop_size = 100000;
	unsigned int *src = new unsigned int[pop_size];
	unsigned int *dst = new unsigned int[pop_size];
	
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dist(1, pop_size);
	
	for(unsigned int i = 0; i < local_jobs; ++i){
		++procesados;
		for(unsigned int g = 0; g < n_gens; ++g){
			for(unsigned int k = 0; k < pop_size; ++k){
				dst[k] = dist(gen);
				src[k] = dist(gen);
			}
		}
	}
	
	delete [] src;
	delete [] dst;
	
	global_mutex.lock();
	cout<<"DummyThread["<<pid<<"] - Fin (Total trabajos: "<<procesados<<", Total ms: "<<timer.getMilisec()<<")\n";
	global_mutex.unlock();
	
}

int main(int argc,char** argv){

	if(argc != 5){
		cout<<"\nUsage: ./test json_file total_jobs n_threads output_base\n";
		cout<<"\n";
		return 0;
	}
	
	const char *settings_file = argv[1];
	unsigned int total = atoi(argv[2]);
	unsigned int n_threads = atoi(argv[3]);
	const char *output_base = argv[4];
	
	cout<<"Test - Inicio (total: "<<total<<", n_threads: "<<n_threads<<", output_base: "<<output_base<<")\n";
	NanoTimer timer;
	
//	cout<<"Test - Revisando trabajos terminados\n";
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
//	cout<<"Test - Terminados: "<<terminados<<", Total efectivo: "<<total<<"\n";
	
	cout<<"Test - Preparando Cola de Trabajo\n";
//	fill_queue(fsettings, total, generator);
	SimulatorFactory factory(settings_file);
	for(unsigned int i = 0; i < total; ++i){
		work_queue.push_back(factory.getInstance());
	}
	
	double ms_preparation = timer.getMilisec();
	cout<<"Test - Preparacion terminada en "<<ms_preparation<<" ms\n";
	
	cout<<"Test - Iniciando "<<n_threads<<" threads\n";
	vector<thread> threads_list;
	for(unsigned int i = 0; i < n_threads; ++i){
		threads_list.push_back( thread(SimultionThread, i, n_threads, output_base) );
//		threads_list.push_back( thread(DummyThread, i, total/n_threads) );
		
//		// Tomar pthread de este thread
//		pthread_t current_thread = threads_list.back().native_handle();
//		// Preparar datos para setear afinidad
//		cpu_set_t cpuset;
//		CPU_ZERO(&cpuset);
//		CPU_SET(i, &cpuset);
//		// Setear afinidad
//		pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
		
	}
	for(unsigned int i = 0; i < n_threads; ++i){
		threads_list[i].join();
	}
	double ms_processing = timer.getMilisec();
	cout<<"Test - Procesamiento terminado en "<<(ms_processing - ms_preparation)<<" ms\n";
	
//	// Analyzer
//	
//	double ms_analysis = timer.getMilisec();
//	cout<<"Test - Analisis terminado en "<<(ms_analysis - ms_processing)<<" ms\n";
	
//	cout<<"Test - Tiempo total: "<<ms_analysis<<" ms\n";
	
	for(unsigned int i = 0; i < work_queue.size(); ++i){
		delete work_queue[i];
	}
	work_queue.clear();
	
	return 0;
}













