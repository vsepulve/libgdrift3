#include "EventList.h"

EventList::EventList(){
	id = 0;
//	gen_start = 0;
//	gen_finish = 0;
	cur_pos = 0;
}

EventList::EventList(const EventList &original){
	id = original.getId();
//	gen_start = original.start();
//	gen_finish = original.finish();
	cur_pos = 0;
}

EventList& EventList::operator=(const EventList& original){
	if (this != &original){
		id = original.getId();
//		gen_start = original.start();
//		gen_finish = original.finish();
		cur_pos = 0;
	}
	return *this;
}

EventList *EventList::clone(){
	return new EventList(*this);
}

EventList::~EventList(){
	events.clear();
}

//bool EventList::hasEvent(unsigned int gen){
//	// Por ahora lineal, podria ser binario
//	for(unsigned int i = 0; i < events.size(); ++i){
//		if( events[i].getGeneration() == gen )
//	}
//	return false;
//}

//unsigned int EventList::start(){
//	return gen_start;
//}

//unsigned int EventList::finish(){
//	return gen_finish;
//}

unsigned int EventList::size(){
	return events.size();
}

Event &EventList::getEvent(unsigned int pos){
	if(pos >= events.size()){
		cerr<<"Event::getEvent - Error, invalid generation ("<<pos<<" >= "<<events.size()<<")\n";
		exit(EXIT_FAILURE);
	}
	return events[pos];
}

Event &EventList::getFirst(){
	return events.front();
}

Event &EventList::getLast(){
	return events.back();
}

void EventList::reset(){
	cur_pos = 0;
}

bool EventList::hasNext(){
	return (cur_pos < events.size());
}

Event &EventList::next(){
	if(cur_pos >= events.size()){
		cerr<<"Event::next - Error, invalid position ("<<cur_pos<<" >= "<<events.size()<<")\n";
		exit(EXIT_FAILURE);
	}
	return events[cur_pos++];
}
	
void EventList::setId(unsigned int _id){
	id = _id;
}

unsigned int EventList::getId() const{
	return id;
}

