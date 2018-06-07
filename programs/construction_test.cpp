
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"
#include "ProfileMarker.h"
#include "Statistics.h"

using namespace std;

int main(int argc,char** argv) {

	if(argc != 4){
		cout<<"\nUsage: ./test project_file simulation_file total_tests\n";
		cout<<"\n";
		return 0;
	}
	const char *project_file = argv[1];
	const char *simulation_file = argv[2];
	unsigned int total_tests = atoi(argv[3]);
	
	cout<<"Test - Inicio\n";
	
	cout<<"Test - Cargando jsons\n";
	json project_json;
	json simulation_json;
	
	ifstream reader(project_file, ifstream::in);
	reader >> project_json;
	reader.close();
	
	reader.open(simulation_file, ifstream::in);
	reader >> simulation_json;
	reader.close();
	
	cout<<"Test - Creando SimulatorFactory\n";
	SimulatorFactory factory(project_json, simulation_json);
	
	NanoTimer timer;
	
//	unsigned int n = 100000;
//	unsigned int *arr = new unsigned int[n];
//	for(unsigned int i = 0; i < n; ++i){
//		for(unsigned int j = 0; j < n; ++j){
//			arr[j] = rand();
//		}
//	}
//	delete [] arr;

	cout << "Test - Prueba de Reload\n";
	SimulatorFactory test_factory(project_json, simulation_json);
	Simulator *s1 = test_factory.getInstance();
	s1->print();
	
	cout << "Test - Recargando\n";
	// Por ahroa voy a OMITIR los parametros fixed
	// Notar que esto implica no incluirlos si tienen stddev 0
	// Quizas seria mejor un metodo mas seguro que ese
	vector<pair<double, double>> new_params;
	new_params.push_back(pair<double, double>(0.005, 0.001));
	new_params.push_back(pair<double, double>(100, 0));
	new_params.push_back(pair<double, double>(5000, 1000));
	new_params.push_back(pair<double, double>(1000, 100));
	new_params.push_back(pair<double, double>(0.2, 0.1));
	new_params.push_back(pair<double, double>(2000, 100));
	new_params.push_back(pair<double, double>(4000, 1000));
	new_params.push_back(pair<double, double>(0.5, 0.1));
	new_params.push_back(pair<double, double>(8000, 1000));
	test_factory.reloadParameters(new_params);
	
	Simulator *s2 = test_factory.getInstance();
	s2->print();
	
	
//	cout<<"Test - Probando ProfileMarker\n";
//	MarkerType type = MARKER_SEQUENCE;
//	unsigned int len = 10;
//	unsigned int init_alleles = 1000;
//	MutationType mut_type = MUTATION_BASIC;
//	vector<double> params;
//	params.push_back(0.5);
//	params.push_back(20.1);
//	ProfileMarker marker_1(type, len, init_alleles, mut_type, params);
//	ProfileMarker marker_2 = marker_1;
//	cout<<"Marker 1: ";
//	marker_1.print();
//	cout<<"Marker 2: ";
//	marker_2.print();
	
	NanoTimer sim_timer;
	cout<<"Test - Probando " << total_tests << " simulaciones\n";
	cout<<"-----     -----\n";
	
	for(unsigned int i = 0; i < total_tests; ++i){
		cout<<"Test - Creando nuevo Simulator " << i << "\n";
		sim_timer.reset();
		Simulator *sim = factory.getInstance();
		double factory_milisec = sim_timer.getMilisec();
	
		cout<<"Test - Corriendo Simulator\n";
		sim_timer.reset();
		sim->run();
		double run_milisec = sim_timer.getMilisec();
	
//		cout<<"Test - Verificando Estadisticos\n";
//		float sampling = 0.05;
//		Statistics stats(sim, sampling);
//		map<string, vector< map<string, double> > > statistics = stats.getStatistics();
//		for( auto &par : statistics ){
//			cout<<"Test - Stats Population \""<< par.first <<"\"\n";
//			unsigned int marker_pos = 0;
//			for( map<string, double> &marker_stats : par.second ){
//				cout<<"Test - [ Marker "<<marker_pos++<<" ]\n";
//				for( auto &stat : marker_stats ){
//					cout<<"Test - " << stat.first << ": "<< stat.second <<"\n";
//				}
//				cout<<"-----\n";
//			}
//		}
	
		delete sim;
		cout<<"Test - factory_milisec: " << factory_milisec << "\n";
		cout<<"Test - run_milisec: " << run_milisec << "\n";
		cout<<"-----     -----\n";
	}
	
	
	cout<<"Test - Fin (tpo total: "<<timer.getMilisec()<<" ms)\n";

	return 0;
	
	
	
	
	
	
	
	
	
}















