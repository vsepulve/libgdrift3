#ifndef _EVENT_H_
#define _EVENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <vector>

using namespace std;

enum EventType { NO=0, CREATE=1, SPLIT=2, MIGRATE=3, MERGE=4, INCREASE=5, DECREASE=6, EXTINCT=7, ENDSIM=10 };

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
	
	void setGeneration(unsigned int _gen){
		gen = _gen;
	}
	
	void setType(EventType _type){
		type = _type;
	}
	
	void addNumParam(double val){
		num_params.push_back(val);
	}
	
	void addTextParam(string &val){
		text_params.push_back(val);
	}
	
	void print(){
		// print type
		cout<<"";
		if( type == CREATE ){
			cout<<"CREATE ";
		}
		else if( type == SPLIT ){
			cout<<"SPLIT ";
		}
		else if( type == MIGRATE ){
			cout<<"MIGRATE ";
		}
		else if( type == MERGE ){
			cout<<"MERGE ";
		}
		else if( type == INCREASE ){
			cout<<"INCREASE ";
		}
		else if( type == DECREASE ){
			cout<<"DECREASE ";
		}
		else if( type == EXTINCT ){
			cout<<"EXTINCT ";
		}
		else if( type == ENDSIM ){
			cout<<"ENDSIM ";
		}
		else{
			cout<<"UNKNOWN ";
		}
		
		cout << "Generation (" << gen << ") ";
		
		// print params
		if( text_params.size() > 0 ){
			cout << "Text Params (\"" << text_params[0] << "\"";
			for(unsigned int i = 1; i < text_params.size(); ++i){
				cout << ", \"" << text_params[i] << "\"";
			}
			cout<<") ";
		}
		if( num_params.size() > 0 ){
			cout << "Num Params (" << num_params[0] << "";
			for(unsigned int i = 1; i < num_params.size(); ++i){
				cout << ", " << num_params[i] << "";
			}
			cout<<") ";
		}
		
		cout<<"\n";
	}
	
};

#endif
