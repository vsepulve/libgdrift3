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
	vector<Event*> events;
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
	
	Event *getEvent(unsigned int pos);
	
	Event *getFirst();
	
	Event *getLast();
	
	void reset();
	
	bool hasNext();
	
	Event *next();
	
	void setId(unsigned int _id);
	
	unsigned int getId() const;
	
//	void resize(unsigned int size){
//		events.resize(size);
//	}

	void addEvent(Event *event){
		events.push_back(event);
	}
	
	unsigned int serializedSize(){
		// return 0;
		unsigned int n_bytes = 0;
		// id, n_events, cada evento
		n_bytes += sizeof(int);
		n_bytes += sizeof(int);
		for(Event *e : events){
			n_bytes += e->serializedSize();
		}
		return n_bytes;
	}

	void serialize(char *buff){
		cout<<"EventList::serialize - Inicio\n";
	
		memcpy(buff, (char*)&id, sizeof(int));
		buff += sizeof(int);
		
		unsigned int n_events = events.size();
		memcpy(buff, (char*)&n_events, sizeof(int));
		buff += sizeof(int);
		
		for(Event *e : events){
			e->serialize(buff);
			buff += e->serializedSize();
		}
		
		cout<<"EventList::serialize - Fin\n";
	}

	unsigned int loadSerialized(char *buff){
		cout<<"EventList::loadSerialized - Inicio\n";
		
		// Guardo el original para calcular desplazamiento
		char *buff_original = buff;
	
		memcpy((char*)&id, buff, sizeof(int));
		buff += sizeof(int);
		
		unsigned int n_events = 0;
		memcpy((char*)&n_events, buff, sizeof(int));
		buff += sizeof(int);
		
		for(unsigned int i = 0; i < n_events; ++i){
			Event *e = new Event();
			buff += e->loadSerialized(buff);
			events.push_back(e);
		}
		
		cout<<"EventList::loadSerialized - Fin\n";
		return (buff - buff_original);
	}

};

#endif
