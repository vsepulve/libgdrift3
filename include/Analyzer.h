#ifndef _ANALIZER_H_
#define _ANALIZER_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

#include <thread>
#include <mutex>

#include "WorkManager.h"
#include "ResultsReader.h"

using namespace std;

class Analyzer{

protected:

private:
	WorkManager *manager;
	string file_base;
	string target_base;
	string results_base;
	// Numero de threads de procesamiento
	unsigned int n_threads;
	// Porcentage de los topK para calcular las dist posteriori
	float percentage;

public:

	Analyzer();
	Analyzer(WorkManager *_manager, unsigned int _n_threads, float _percentage, string _file_base, string _target_base, string _results_base);
	virtual ~Analyzer();
	
	void execute(unsigned int sim_id);
	
	void partialResults(unsigned int sim_id);
	
	
	
	
};

#endif
