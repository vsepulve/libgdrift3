#ifndef _EVENT_LIST_H_
#define _EVENT_LIST_H_

#include <map>

#include "NanoTimer.h"
#include "Event.h"

using namespace std;

//enum EventType { CREATE = 1, SPLIT = 2, ENDSIM = 10 };

class EventList{

private:
	// Id del escenario
	unsigned int id;
	vector<Event> events;
//	unsigned int gen_start;
//	unsigned int gen_finish;
	unsigned int cur_pos;

public:

	EventList();
	EventList(const EventList &original);
	EventList& operator=(const EventList& original);
	virtual EventList *clone();
	virtual ~EventList();
	
//	bool hasEvent(unsigned int gen);
	
//	unsigned int start();
//	
//	unsigned int finish();
	
	unsigned int size();
	
	Event &getEvent(unsigned int pos);
	
	Event &getFirst();
	
	Event &getLast();
	
	void reset();
	
	bool hasNext();
	
	Event &next();
	
	void setId(unsigned int _id);
	
	unsigned int getId() const;
	
	void resize(unsigned int size){
		events.resize(size);
	}
	
};

#endif
