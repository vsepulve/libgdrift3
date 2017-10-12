#include "Event.h"

Event::Event(){
	gen = 0;
	type = NO;
}

Event::Event(const Event &original){
	gen = original.getGeneration();
	type = original.getType();
	num_params = original.getNumParams();
	text_params = original.getTextParams();
}

Event& Event::operator=(const Event& original){
	if (this != &original){
		gen = original.getGeneration();
		type = original.getType();
		num_params = original.getNumParams();
		text_params = original.getTextParams();
	}
	return *this;
}

Event *Event::clone(){
	return new Event(*this);
}

Event::~Event(){
	gen = 0;
	type = NO;
	num_params.clear();
	text_params.clear();
}


