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

	unsigned int serializedSize(){
		unsigned int n_bytes = 0;
		
		// gen (int) type (1) n_num_params, num params, n_text_params, cada text
		n_bytes += sizeof(int) + 1;

		n_bytes += sizeof(int);
		n_bytes += sizeof(double) * num_params.size();
		
		n_bytes += sizeof(int);
		for(unsigned int i = 0; i < text_params.size(); ++i){
			n_bytes += sizeof(int);
			// guardo length chars mas un \0
			n_bytes += text_params[i].length() + 1;
		}
		return n_bytes;
	}
	
	void serialize(char *buff){
		memcpy(buff, (char*)&gen, sizeof(int));
		buff += sizeof(int);
		
		*buff = (char)type;
		buff += 1;

		unsigned int n_params = num_params.size();
		memcpy(buff, (char*)&n_params, sizeof(int));

	}
	
	void loadSerialized(char *buff){
		
	}
};

#endif
