#ifndef _EVENT_H_
#define _EVENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <vector>

using namespace std;

enum EventType { NO=0, CREATE=1, SPLIT=2, ENDSIM=3 };

class Event{

private:

	unsigned int gen;
	EventType type;
	vector<double> num_params;
	vector<string> text_params;
	
public:

	Event();
	Event(const Event &original);
	Event& operator=(const Event& original);
	virtual Event *clone();
	virtual ~Event();
	
	unsigned int getGeneration() const{
		return gen;
	}
	
	EventType getType() const{
		return type;
	}
	
	const vector<double> &getNumParams() const{
		return num_params;
	}
	
	const vector<string> &getTextParams() const{
		return text_params;
	}
	
};

#endif
