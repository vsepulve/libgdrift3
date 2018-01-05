#include "Simulator.h"

Simulator::Simulator() 
	: generator((std::random_device())()) {
	model = NULL;
	events = NULL;
	profile = NULL;
}

Simulator::Simulator(const Simulator &original) 
	: generator((std::random_device())()) {
	model = original.getModel()->clone();
	events = original.getEvents()->clone();
	profile = original.getProfile()->clone();
	// De momento omito la copia de poblaciones
	// populations = original.getPopulations();
}

Simulator& Simulator::operator=(const Simulator& original){
	if (this != &original){
		model = original.getModel()->clone();
		events = original.getEvents()->clone();
		profile = original.getProfile()->clone();
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
	if(profile != NULL){
		delete profile;
		profile = NULL;
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

void Simulator::setProfile(Profile *_profile){
	profile = _profile;
}

Model *Simulator::getModel() const{
	return model;
}

EventList *Simulator::getEvents() const{
	return events;
}

Profile *Simulator::getProfile() const{
	return profile;
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
	
	cout<<"Simulator::run - Starting Simulation\n";
	
	// ITerar por los eventos de la lista o por generacion
	
	// Algunos eventos agregan poblaciones nuevas o las descartan
	
	// Al final se tiene un conjunto de poblaciones resultantes
	
	// Un primer enfoque es tomar el numero total de generaciones (o las inicial/final)
	
//	for(unsigned int gen = events->start(); gen < events->finish(); ++gen){
//		
//		if( events->hasEvent(gen) ){
//			executeEvent( events->getEvent(gen) );
//		}
//		
//		for(auto it : populations){
//			model->run( it.second );
//		}
//		
//	}
	
	// Verificacion de seguridad para asegurar que la lista tenga al menos un evento
	// El primero evento DEBE ademas ser un create
	// Quizas tambien el ultimo evento deba ser un endsim
	
	if( events->size() < 2 
		|| events->getFirst()->getType() != CREATE
		|| events->getLast()->getType() != ENDSIM ){
		cerr<<"Simulator::run - Bad Events List, omitting ("<<events->size()<<", "<<events->getFirst()->getType()<<", "<<events->getLast()->getType()<<")\n";
		return;
	}
	
	events->reset();
	Event *event = events->next();
	// El primer evento es un create
	executeEvent( event );
	Event *last = event;
	unsigned int gens = 0;
	while( events->hasNext() ){
		event = events->next();
		// Asegurar gens > 0
		gens = event->getGeneration() - last->getGeneration();
		
		for(auto it : populations){
			cout<<"Simulator::run - Running " << gens << " generations for population " << it.first << "\n";
			for( unsigned int gen = 0; gen < gens; ++gen ){
				model->run(it.second, profile, generator);
			}
		}
		
		// El ultimo evento es un endsim, no son necesarias mas generaciones
		executeEvent( event );
		last = event;
	}
	
	cout<<"Simulator::run - End";
	
	
}

void Simulator::executeEvent(Event *event){

	EventType type = event->getType();
	const vector<double> num_params = event->getNumParams();
	const vector<string> text_params = event->getTextParams();
	
	if(type == CREATE){
		cout<<"Simulator::executeEvent - CREATE.\n";
		// nombre de la nueva poblacion
		assert(text_params.size() == 1);
		string name = text_params[0];
		
		// tamaño de la nueva poblacion
		assert(num_params.size() == 1);
		unsigned int size = static_cast<unsigned int>(num_params[0]);
		
		if( populations.find(name) != populations.end() ){
			cerr<<"Simulator::executeEvent - CREATE Warning, already used population name ("<<name<<").\n";
			delete populations[name];
			populations.erase(name);
		}
		// Aqui tambien habria que indicar la especie u otras propiedades, quzias del pool
		// En ese caso, quiza sea reazonable que el pool sea DE la poblacion
		populations[name] = new Population(size, profile, generator);
	}
	else if(type == SPLIT){
		cout<<"Simulator::executeEvent - SPLIT.\n";
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
			populations.erase(dst1);
		}
		if( populations.find(dst2) != populations.end() ){
			cerr<<"Simulator::executeEvent - SPLIT Warning, already used population name ("<<dst2<<").\n";
			delete populations[dst2];
			populations.erase(dst2);
		}
		// Aqui tambien habria que indicar la especie u otras propiedades, quzias del pool
		// En ese caso, quiza sea reazonable que el pool sea DE la poblacion
		
		populations[dst1] = new Population(0, profile, generator);
		populations[dst2] = new Population(0, profile, generator);
		
		populations[dst1]->add(populations[src], size1, generator);
		populations[dst2]->add(populations[src], size2, generator);
		
		delete populations[src];
		populations.erase(src);
		
	}
	else if(type == MIGRATE){
		cout<<"Simulator::executeEvent - MIGRATE.\n";
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
			populations.erase(dst);
		}
		
		// Aqui tambien habria que indicar la especie u otras propiedades, quzias del pool
		// En ese caso, quiza sea reazonable que el pool sea DE la poblacion
		populations[dst] = new Population(0, profile, generator);
		populations[dst]->add(populations[src], size, generator);
		
	}
	else if(type == MERGE){
		cout<<"Simulator::executeEvent - MERGE.\n";
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
			populations.erase(dst);
		}
		
		// Aqui tambien habria que indicar la especie u otras propiedades, quzias del pool
		// En ese caso, quiza sea reazonable que el pool sea DE la poblacion
		populations[dst] = new Population(0, profile, generator);
		populations[dst]->add(populations[src1], populations[src1]->size(), generator);
		populations[dst]->add(populations[src2], populations[src1]->size(), generator);
		
		delete populations[src1];
		delete populations[src2];
		populations.erase(src1);
		populations.erase(src2);
	}
	else if(type == INCREASE){
		cout<<"Simulator::executeEvent - INCREASE.\n";
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
		populations[src]->increase(size, generator);
	}
	else if(type == DECREASE){
		cout<<"Simulator::executeEvent - DECREASE.\n";
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
		populations[src]->decrease(size, generator);
	}
	else if(type == EXTINCT){
		cout<<"Simulator::executeEvent - EXTINCT.\n";
		// nombre del origen
		assert(text_params.size() == 1);
		string src = text_params[0];
		
		if( populations.find(src) == populations.end() ){
			cerr<<"Simulator::executeEvent - EXTINCT Warning, src population not found ("<<src<<"), omitting.\n";
			return;
		}
		
		delete populations[src];
		populations.erase(src);
	}
	else if(type == ENDSIM){
		cout<<"Simulator::executeEvent - ENDSIM, finishing simulation.\n";
	}
	
}























