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
	
};









#endif
