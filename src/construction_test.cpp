
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "NanoTimer.h"
#include "SimulatorFactory.h"

using namespace std;

int main(int argc,char** argv) {

	if(argc != 2){
		cout<<"\nUsage: ./test json_file\n";
		cout<<"\n";
		return 0;
	}
	const char *json_file = argv[1];
	
	cout<<"Test - Inicio\n";
	
	SimulatorFactory factory(json_file);
	
	NanoTimer timer;
	unsigned int n = 100000;
	unsigned int *arr = new unsigned int[n];
	for(unsigned int i = 0; i < n; ++i){
		for(unsigned int j = 0; j < n; ++j){
			arr[j] = rand();
		}
	}
	delete [] arr;
	
	
	cout<<"Test - Fin (tpo total: "<<timer.getMilisec()<<" ms)\n";

	return 0;
}
