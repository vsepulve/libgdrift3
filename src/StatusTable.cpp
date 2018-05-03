#include "StatusTable.h"

StatusTable::StatusTable(){
}

StatusTable::~StatusTable(){
	
}

void StatusTable::addSimulation(unsigned int sim_id, SimulatorFactory *factory, unsigned int n_sims){	
	table[sim_id].factory = factory;
	table[sim_id].n_sims = n_sims;
	table[sim_id].finished = 0;
	table[sim_id].feedback = 0;
}

void StatusTable::addFinished(unsigned int sim_id){
	table[sim_id].finished++;
}

unsigned int StatusTable::getFinished(unsigned int sim_id){
	return table[sim_id].finished;
}

void StatusTable::resetFinished(unsigned int sim_id){
	table[sim_id].finished = 0;
}

unsigned int StatusTable::getTotal(unsigned int sim_id){
	return table[sim_id].n_sims;
}

void StatusTable::addFeedback(unsigned int sim_id){
	table[sim_id].feedback++;
}

unsigned int StatusTable::getFeedback(unsigned int sim_id){
	return table[sim_id].feedback;
}

SimulatorFactory *StatusTable::getFactory(unsigned int sim_id){
	return table[sim_id].factory;
}

