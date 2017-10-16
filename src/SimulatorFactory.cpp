#include "SimulatorFactory.h"

SimulatorFactory::SimulatorFactory(const char *json_file){
	ifstream reader (json_file, ifstream::in);
	json settings;
	reader >> settings;
	
	cout << settings << "\n";
}

SimulatorFactory::SimulatorFactory(const SimulatorFactory &original){
}

SimulatorFactory& SimulatorFactory::operator=(const SimulatorFactory& original){
	if (this != &original){
	
	}
	return *this;
}

SimulatorFactory *SimulatorFactory::clone(){
	return new SimulatorFactory(*this);
}

SimulatorFactory::~SimulatorFactory(){
	
}

Simulator *SimulatorFactory::getInstance(){
	return NULL;
}

char *SimulatorFactory::getInstanceSerialized(){
	return NULL;
}

