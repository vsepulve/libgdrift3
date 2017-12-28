#include "EventList.h"

EventList::EventList(){
	id = 0;
}

EventList::EventList(const EventList &original){
	id = original.getId();
}

EventList& EventList::operator=(const EventList& original){
	if (this != &original){
		id = original.getId();
	}
	return *this;
}

EventList *EventList::clone(){
	return new EventList(*this);
}

EventList::~EventList(){
	events.clear();
}

bool EventList::hasEvent(unsigned int gen){
	return false;
}

Event &EventList::getEvent(unsigned int gen){
	if(gen >= events.size()){
		cerr<<"Event::getEvent - Error, invalid generation ("<<gen<<" >= "<<events.size()<<")\n";
		exit(EXIT_FAILURE);
	}
	return events[gen];
}

unsigned int EventList::start(){
	return 0;
}

unsigned int EventList::finish(){
	return 0;
}
	
void EventList::setId(unsigned int _id){
	id = _id;
}

unsigned int EventList::getId() const{
	return id;
}

