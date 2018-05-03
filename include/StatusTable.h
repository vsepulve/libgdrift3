#ifndef _STATUS_TABLE_H_
#define _STATUS_TABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <thread>
#include <mutex>
#include <map>

#include "SimulatorFactory.h"

using namespace std;

// Este objeto es solo para agrupar la informacion de la simulacion
// Solo esta pensado para StatusTable (notar que podria ser una inner class)
class SimulationData{
	public: 
		SimulatorFactory *factory;
		unsigned int n_sims;
		unsigned int finished;
		unsigned int feedback;
		SimulationData(){
			factory = NULL;
			n_sims = 0;
			finished = 0;
			feedback = 0;
		}
		~SimulationData(){
			factory = NULL;
			n_sims = 0;
			finished = 0;
			feedback = 0;
		}
};

class StatusTable{

private:
	map<unsigned int, SimulationData> table;

public:

	StatusTable();
	virtual ~StatusTable();
	
	void addSimulation(unsigned int sim_id, SimulatorFactory *factory, unsigned int n_sims);
	
	void addFinished(unsigned int sim_id);
	
	unsigned int getFinished(unsigned int sim_id);
	
	void resetFinished(unsigned int sim_id);
	
	unsigned int getTotal(unsigned int sim_id);
	
	void addFeedback(unsigned int sim_id);
	
	unsigned int getFeedback(unsigned int sim_id);
	
	SimulatorFactory *getFactory(unsigned int sim_id);
	
};

#endif
