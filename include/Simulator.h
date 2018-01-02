#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

#include <map>

#include "NanoTimer.h"
#include "Model.h"
#include "Event.h"
#include "EventList.h"
#include "Population.h"

using namespace std;

class Simulator{

private:

	Model *model;
	EventList *events;
	map<string, Population*> populations;

public:

	Simulator();
	Simulator(const Simulator &original);
	Simulator& operator=(const Simulator& original);
	virtual Simulator *clone();
	virtual ~Simulator();
	
	Model *getModel() const;
	EventList *getEvents() const;
	Population *getPopulation(const string &name) const;
	
	void setModel(Model *_model);
	void setEvents(EventList *_events);
	
	void run();
	void executeEvent(Event *event);
	
};









#endif
