
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <vector>

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"
#include "ServerThreads.h"

using namespace std;

int main(int argc,char** argv){

	if(argc != 8){
		cout<<"\nUsage: ./bin/threads_test total_jobs n_threads project_json simulation_json output_base target_base results_base\n";
		cout<<"\nExample: ./bin/threads_test 100 4 ./data/project_1.json ./data/simulation_10.json ./data/data_ ./data/target_ ./data/results_\n";
		cout<<"\n";
		return 0;
	}
	
	unsigned int total = atoi(argv[1]);
	unsigned int n_threads = atoi(argv[2]);
	string project_json_file = argv[3];
	string simulation_json_file = argv[4];
	string output_base = argv[5];
	string target_base = argv[6];
	string results_base = argv[7];
	
	cout<<" ----- Inicio (total: " << total << ", n_threads: " << n_threads << ", project: " << project_json_file << ", simulation: " << simulation_json_file << ") ----- \n";
	NanoTimer timer;
	
	// Preparacion de elementos compartidos
	cout << "Iniciando WorkManager\n";
	WorkManager manager;
	cout << "Iniciando Analyzer\n";
	Analyzer analyzer(&manager, n_threads, 0.05, output_base, target_base, results_base);
	
	// Preparacion del proyecto (y con eso, el target)
	cout << " ----- Cargando json del proyecto ----- \n";
	ifstream reader(project_json_file, ifstream::in);
	json project_json;
	reader >> project_json;
	reader.close();
	unsigned int project_id = project_json["Id"];
	
	// Leer N_populations del json
	unsigned int n_pops = project_json["N_populations"];
	cout << "Project " << project_id << " - n_pops: " << n_pops << "\n";
	
	// Iterar por Individual -> Markers
	Profile perfil(project_json["Individual"]);
	unsigned int n_markers = perfil.getNumMarkers();
	cout << "Project " << project_id << " - n_markers: " << n_markers << "\n";
	
	// Cada uno tiene N_populations rutas en el arreglo Sample_path
	// Proceso esto en el orden de estadisticos: por cada poblacion (ordenada por nombre) => los datos de cada marcador
	// El mapa siguiente es pop_name -> vector por marcador
	map<string, vector<string> > sample_paths;
	for( unsigned int i = 0; i < n_pops; ++i ){
		// Considerando el nuevo statistics, quiza un solo archivo deberia tener todos los marcadores para la poblacion
		string pop_name = project_json["Populations"][i]["Name"];
		for( unsigned int j = 0; j < n_markers; ++j ){
			string sample_path = project_json["Populations"][i]["Sample_path"][j];
			cout << "Project " << project_id << " - sample[" << pop_name << "][" << j << "]: " << sample_path << "\n";
			sample_paths[pop_name].push_back(sample_path);
		}
	}
	
	// Estructura temporal para almacenar summary populations
	vector<vector<string>> summary_alleles;
	Statistics stats;
	for( auto samples : sample_paths ){
		stats.processStatistics(samples.second[0], samples.first, &perfil, &summary_alleles);
	}
	stats.processStatistics("summary", &perfil, &summary_alleles);
	
	// Crear el target con los estadisticos generados
	string target_file = target_base;
	target_file += to_string(project_id);
	target_file += ".txt";
	
	// Preparar escritor y crear el archivo 
	fstream writer(target_file, fstream::out | fstream::trunc);
	if( writer.good() ){
		char stat_buff[1024];
		memset(stat_buff, 0, 1024);
//		map<string, vector< map<string, double> > > statistics = stats.getStatistics();
		for( auto it_stats_pop : stats.getStatistics() ){
			string pop_name = it_stats_pop.first;
			unsigned int marker_pos = 0;
			for( map<string, double> stats_marker : it_stats_pop.second ){
				for( auto it_stat : stats_marker ){
					string stat_name = it_stat.first;
					double value = it_stat.second;
					cout << "Project " << project_id << " - pop " << pop_name << " - marker " << marker_pos << " - stat " << stat_name <<" -> " << value << "\n";
					sprintf(stat_buff + strlen(stat_buff), "%f\t", value);
				}
				++marker_pos;
			}
		
		}
		sprintf(stat_buff + strlen(stat_buff), "\n");
		writer.write(stat_buff, strlen(stat_buff));
		writer.close();
	}
	else{
		cerr << "Error opening file \"" << target_file << "\"\n";
	}
	// Preparacion terminada y target almacenado
	
	// Cargando Simulation
	cout << " ----- Cargando json de simulacion ----- \n";
	reader.open(simulation_json_file, ifstream::in);
	json simulation_json;
	reader >> simulation_json;
	unsigned int sim_id = simulation_json["Id"];
	
	cout << "Iniciando manager->addWork de Simulation " << sim_id << "\n";
	manager.addWork(sim_id, project_json, simulation_json, total);
	
	cout << " ----- Preparacion terminada en " << timer.getMilisec() << " ms ----- \n";
	timer.reset();
	
	cout << " ----- Iniciando Threads de procesamiento ----- \n";
	vector<thread> threads_list;
	unsigned int pids[n_threads];
	for(unsigned int i = 0; i < n_threads; ++i){
		pids[i] = i;
		// TODO: Esta version de processing_thread SIEMPRE se queda esperando mas trabajo, no retorna
		// Esta pensada para lanzarse con thread(...).detach()
		threads_list.push_back( thread(processing_thread, pids[i], output_base, &manager, &analyzer) );
	}
	for(unsigned int i = 0; i < n_threads; ++i){
		threads_list[i].join();
	}
	
	cout << " ----- Procesamiento terminado en " << timer.getMilisec() << " ms ----- \n";
	
	return 0;
}













