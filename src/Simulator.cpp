#include "Simulator.h"

Simulator::Simulator(){
	model = NULL;
	events = NULL;
}

Simulator::Simulator(const Simulator &original){
	model = original.getModel()->clone();
	events = original.getEvents()->clone();
	// De momento omito la copia de poblaciones
	// populations = original.getPopulations();
}

Simulator& Simulator::operator=(const Simulator& original){
	if (this != &original){
		model = original.getModel()->clone();
		events = original.getEvents()->clone();
		// De momento omito la copia de poblaciones
		// populations = original.getPopulations();
	}
	return *this;
}

Simulator *Simulator::clone(){
	return new Simulator(*this);
}

Simulator::~Simulator(){
	if(model != NULL){
		delete model;
		model = NULL;
	}
	if(events != NULL){
		delete events;
		events = NULL;
	}
	for(auto it : populations){
		if( it.second != NULL ){
			delete it.second;
			it.second = NULL;
		}
	}
	populations.clear();
}


Model *Simulator::getModel() const{
	return model;
}

EventList *Simulator::getEvents() const{
	return events;
}

Population *Simulator::getPopulation(const string &name) const{
	auto it = populations.find(name);
	if(it == populations.end()){
		return NULL;
	}
	return it->second;
}

void Simulator::run(){
	assert(model != NULL);
	assert(events != NULL);
	
	cout<<"Simulator::run - Starting Simulation";
	
	// ITerar por los eventos de la lista o por generacion
	
	// Algunos eventos agregan poblaciones nuevas o las descartan
	
	// Al final se tiene un conjunto de poblaciones resultantes
	
	// Un primer enfoque es tomar el numero total de generaciones (o las inicial/final)
	
	for(unsigned int gen = events->start(); gen < events->finish(); ++gen){
		
		if( events->hasEvent(gen) ){
			executeEvent( events->getEvent(gen) );
		}
		
		for(auto it : populations){
			model->run( it.second );
		}
		
	}
	
	
}

void Simulator::executeEvent(Event &event){
	EventType type = event.getType();
	const vector<double> num_params = event.getNumParams();
	const vector<string> text_params = event.getTextParams();
	if(type == CREATE){
		assert(text_params.size() == 1);
		assert(num_params.size() == 1);
		string name = text_params[0];
		unsigned int size = static_cast<unsigned int>(num_params[0]);
		if( populations.find(name) != populations.end() ){
			cerr<<"Simulator::executeEvent - Warning, already used population name ("<<name<<")\n";
			delete populations[name];
		}
		// Aqui tambien habria que indicar la especie u otras propiedades, quzias del pool
		// En ese caso, quiza sea reazonable que el pool sea DE la poblacion
		populations[name] = new Population(size);
	}
	
}























