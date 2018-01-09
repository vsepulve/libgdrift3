
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

	if(argc != 2){
		cout<<"\nUsage: ./test json_file\n";
		cout<<"\n";
		return 0;
	}
	const char *json_file = argv[1];
	
	cout<<"Test - Inicio\n";
	
	cout<<"Test - Creando SimulatorFactory\n";
	SimulatorFactory factory(json_file);
	
	NanoTimer timer;
	
//	unsigned int n = 100000;
//	unsigned int *arr = new unsigned int[n];
//	for(unsigned int i = 0; i < n; ++i){
//		for(unsigned int j = 0; j < n; ++j){
//			arr[j] = rand();
//		}
//	}
//	delete [] arr;
	
	cout<<"Test - Probando ProfileMarker\n";
	MarkerType type = MARKER_SEQUENCE;
	unsigned int len = 10;
	unsigned int init_alleles = 1000;
	MutationType mut_type = MUTATION_BASIC;
	vector<double> params;
	params.push_back(0.5);
	params.push_back(20.1);
	ProfileMarker marker_1(type, len, init_alleles, mut_type, params);
	ProfileMarker marker_2 = marker_1;
	cout<<"Marker 1: ";
	marker_1.print();
	cout<<"Marker 2: ";
	marker_2.print();
	
	cout<<"Test - Creando nuevo Simulator\n";
	Simulator *sim = factory.getInstance();
	
	cout<<"Test - Corriendo Simulator\n";
	sim->run();
	
	cout<<"Test - Verificando Estadisticos\n";
	float sampling = 0.05;
	Statistics stats(sim, sampling);
	
	delete sim;
	
	cout<<"Test - Fin (tpo total: "<<timer.getMilisec()<<" ms)\n";

	return 0;
	
	
	
	
	
	
	
	
	
}















