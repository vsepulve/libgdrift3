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


void Simulator::setModel(Model *_model){
	model = _model;
}

void Simulator::setEvents(EventList *_events){
	events = _events;
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
		// nombre de la nueva poblacion
		assert(text_params.size() == 1);
		string name = text_params[0];
		
		// tamaño de la nueva poblacion
		assert(num_params.size() == 1);
		unsigned int size = static_cast<unsigned int>(num_params[0]);
		
		if( populations.find(name) != populations.end() ){
			cerr<<"Simulator::executeEvent - CREATE Warning, already used population name ("<<name<<").\n";
			delete populations[name];
		}
		// Aqui tambien habria que indicar la especie u otras propiedades, quzias del pool
		// En ese caso, quiza sea reazonable que el pool sea DE la poblacion
		populations[name] = new Population(size);
	}
	else if(type == SPLIT){
		// nombre del origen y de los 2 destinos
		assert(text_params.size() == 3);
		string src = text_params[0];
		string dst1 = text_params[1];
		string dst2 = text_params[2];
		
		// porcentaje del primer destino
		assert(num_params.size() == 1);
		double percentage = num_params[0];
		assert(percentage => 0.0 && percentage <= 1.0);
		unsigned int size1 = 0;
		unsigned int size2 = 0;
		
		if( populations.find(src) == populations.end() ){
			cerr<<"Simulator::executeEvent - SPLIT Warning, src population not found ("<<src<<"), omitting.\n";
			return;
		}
		unsigned int size = populations[src]->size();
		size1 = static_cast<unsigned int>(percentage * size);
		size2 = size - size1;
			
		if( populations.find(dst1) != populations.end() ){
			cerr<<"Simulator::executeEvent - SPLIT Warning, already used population name ("<<dst1<<").\n";
			delete populations[dst1];
		}
		if( populations.find(dst2) != populations.end() ){
			cerr<<"Simulator::executeEvent - SPLIT Warning, already used population name ("<<dst2<<").\n";
			delete populations[dst2];
		}
		// Aqui tambien habria que indicar la especie u otras propiedades, quzias del pool
		// En ese caso, quiza sea reazonable que el pool sea DE la poblacion
		populations[dst1] = new Population(size1);
		populations[dst2] = new Population(size2);
	}
	else if(type == MIGRATE){
		// nombre del origen y del destino
		assert(text_params.size() == 2);
		string src = text_params[0];
		string dst = text_params[1];
		
		// porcentaje del destino
		assert(num_params.size() == 1);
		double percentage = num_params[0];
		assert(percentage => 0.0 && percentage <= 1.0);
		
		if( populations.find(src) == populations.end() ){
			cerr<<"Simulator::executeEvent - MIGRATE Warning, src population not found ("<<src<<"), omitting.\n";
			return;
		}
		unsigned int size = static_cast<unsigned int>(percentage * populations[src]->size());
			
		if( populations.find(dst) != populations.end() ){
			cerr<<"Simulator::executeEvent - MIGRATE Warning, already used population name ("<<dst<<").\n";
			delete populations[dst];
		}
		
		// Aqui tambien habria que indicar la especie u otras propiedades, quzias del pool
		// En ese caso, quiza sea reazonable que el pool sea DE la poblacion
		populations[dst] = new Population(size);
		
		populations[src]->decrease(size);
	}
	else if(type == MERGE){
		// nombre del origen 1 y 2, y del destino
		assert(text_params.size() == 3);
		string src1 = text_params[0];
		string src2 = text_params[1];
		string dst = text_params[2];
		
		if( populations.find(src1) == populations.end() ){
			cerr<<"Simulator::executeEvent - MERGE Warning, src population not found ("<<src1<<"), omitting.\n";
			return;
		}
		if( populations.find(src2) == populations.end() ){
			cerr<<"Simulator::executeEvent - MERGE Warning, src population not found ("<<src2<<"), omitting.\n";
			return;
		}
		
		if( populations.find(dst) != populations.end() ){
			cerr<<"Simulator::executeEvent - MERGE Warning, already used population name ("<<dst<<").\n";
			delete populations[dst];
		}
		
		// Aqui tambien habria que indicar la especie u otras propiedades, quzias del pool
		// En ese caso, quiza sea reazonable que el pool sea DE la poblacion
		populations[dst] = new Population();
		populations[dst]->add( populations[src1] );
		populations[dst]->add( populations[src2] );
		
		delete populations[src1];
		delete populations[src2];
	}
	else if(type == INCREASE){
		// nombre del origen
		assert(text_params.size() == 1);
		string src = text_params[0];
		
		// porcentaje del incremento
		assert(num_params.size() == 1);
		double percentage = num_params[0];
		assert(percentage => 0.0);
		
		if( populations.find(src) == populations.end() ){
			cerr<<"Simulator::executeEvent - INCREASE Warning, src population not found ("<<src<<"), omitting.\n";
			return;
		}
		
		unsigned int size = static_cast<unsigned int>(percentage * populations[src]->size());
		populations[src]->increase( size );
	}
	else if(type == DECREASE){
		// nombre del origen
		assert(text_params.size() == 1);
		string src = text_params[0];
		
		// porcentaje del decremento
		assert(num_params.size() == 1);
		double percentage = num_params[0];
		assert(percentage => 0.0 && percentage <= 1.0);
		
		if( populations.find(src) == populations.end() ){
			cerr<<"Simulator::executeEvent - DECREASE Warning, src population not found ("<<src<<"), omitting.\n";
			return;
		}
		
		unsigned int size = static_cast<unsigned int>(percentage * populations[src]->size());
		populations[src]->decrease( size );
	}
	else if(type == EXTINCT){
		// nombre del origen
		assert(text_params.size() == 1);
		string src = text_params[0];
		
		if( populations.find(src) == populations.end() ){
			cerr<<"Simulator::executeEvent - EXTINCT Warning, src population not found ("<<src<<"), omitting.\n";
			return;
		}
		
		delete populations[src];
	}
	
}























