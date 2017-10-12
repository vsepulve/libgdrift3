#ifndef _EVENT_LIST_H_
#define _EVENT_LIST_H_

#include <map>

#include "NanoTimer.h"
#include "Event.h"

using namespace std;

//enum EventType { CREATE = 1, SPLIT = 2, ENDSIM = 10 };

class EventList{

private:
	vector<Event> events;

public:

	EventList();
	EventList(const EventList &original);
	EventList& operator=(const EventList& original);
	virtual EventList *clone();
	virtual ~EventList();
	
	bool hasEvent(unsigned int gen);
	
	Event &getEvent(unsigned int gen);
	
	unsigned int start();
	
	unsigned int finish();
	
};

#endif
