#include "EventList.h"

EventList::EventList(){
}

EventList::EventList(const EventList &original){
}

EventList& EventList::operator=(const EventList& original){
	if (this != &original){
	
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

