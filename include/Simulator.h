#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <random>

#include <map>
#include <vector>

#include "NanoTimer.h"
#include "Model.h"
#include "Event.h"
#include "EventList.h"
#include "Population.h"
#include "Profile.h"
#include "Pool.h"

using namespace std;

class Simulator{

private:

	Model *model;
	EventList *events;
	map<string, Population*> populations;
	Profile *profile;
	Pool *pool;
	mt19937 generator;

public:

	Simulator();
	Simulator(const Simulator &original);
	Simulator& operator=(const Simulator& original);
	virtual Simulator *clone();
	virtual ~Simulator();
	
	Model *getModel() const;
	EventList *getEvents() const;
	Profile *getProfile() const;
	Pool *getPool() const;
	// Notar que la lista de nombres de poblacion la retorno POR COPIA
	vector<string> getPopulationNames() const;
	Population *getPopulation(const string &name) const;
	
	void setModel(Model *_model);
	void setEvents(EventList *_events);
	void setProfile(Profile *_profile);
	
	void run();
	void executeEvent(Event *event);

	char *serialize(){
//		return NULL;
		
		// int para el mismo n_bytes
		unsigned int n_bytes = sizeof(int);
		n_bytes += model->serializedSize();
		n_bytes += events->serializedSize();
		n_bytes += profile->serializedSize();
		
		char *buff = new char[n_bytes];
		unsigned int pos = 0;
		memcpy(buff + pos, (char*)&n_bytes, sizeof(int));
		pos += sizeof(int);
		
		// Notar que model es polimorfico
		// Quizas esto deberia hacerlo un factory
		model->serialize(buff + pos);
		pos += model->serializedSize();

		events->serialize(buff + pos);
		pos += events->serializedSize();

		profile->serialize(buff + pos);
		pos += profile->serializedSize();
		
		return buff;
		
	}
	
	unsigned int loadSerialized(char *buff){
		if( buff == NULL ){
			return 0;
		}
		
		char *buff_original = buff;
		
		// Esto de hecho no es necesario, pero podria usarse para verificacion
		unsigned int n_bytes = 0;
		memcpy((char*)&n_bytes, buff, sizeof(int));
		buff += sizeof(int);
 		
		buff += model->loadSerialized(buff);
		
		buff += events->loadSerialized(buff);
		
		buff += profile->loadSerialized(buff);
 		
 		return (buff - buff_original);
 		
	}
	
};









#endif
