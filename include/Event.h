#ifndef _EVENT_H_
#define _EVENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

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
		cout << "";
		if( type == CREATE ){
			cout << "CREATE ";
		}
		else if( type == SPLIT ){
			cout << "SPLIT ";
		}
		else if( type == MIGRATE ){
			cout << "MIGRATE ";
		}
		else if( type == MERGE ){
			cout << "MERGE ";
		}
		else if( type == INCREASE ){
			cout << "INCREASE ";
		}
		else if( type == DECREASE ){
			cout << "DECREASE ";
		}
		else if( type == EXTINCT ){
			cout << "EXTINCT ";
		}
		else if( type == ENDSIM ){
			cout << "ENDSIM ";
		}
		else{
			cout << "UNKNOWN ";
		}
		
		cout << "Generation (" << gen << ") ";
		
		// print params
		if( text_params.size() > 0 ){
			cout << "Text Params (\"" << text_params[0] << "\"";
			for(unsigned int i = 1; i < text_params.size(); ++i){
				cout << ", \"" << text_params[i] << "\"";
			}
			cout << ") ";
		}
		if( num_params.size() > 0 ){
			cout << "Num Params (" << num_params[0] << "";
			for(unsigned int i = 1; i < num_params.size(); ++i){
				cout << ", " << num_params[i] << "";
			}
			cout << ") ";
		}
		
		cout << "\n";
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
			n_bytes += text_params[i].length();
		}
		return n_bytes;
	}
	
	void serialize(char *buff){
//		cout << "Event::serialize - Inicio\n";
		memcpy(buff, (char*)&gen, sizeof(int));
		buff += sizeof(int);
		
		*buff = (char)type;
		buff += 1;

		unsigned int n_params = num_params.size();
		memcpy(buff, (char*)&n_params, sizeof(int));
		buff += sizeof(int);
		for(double param : num_params){
//			cout << "Event::serialize - Guardando num_params: " << param << "\n";
			memcpy(buff, (char*)&param, sizeof(double));
			buff += sizeof(double);
		}
		
		n_params = text_params.size();
		memcpy(buff, (char*)&n_params, sizeof(int));
		buff += sizeof(int);
		for(string param : text_params){
//			cout << "Event::serialize - Guardando text_params: " << param << " (" << param.length() << ")\n";
			unsigned int length = param.length();
			// Primero guardo length en un int
			memcpy(buff, (char*)&length, sizeof(int));
			buff += sizeof(int);
			// Despues guardo los length chars de texto
			memcpy(buff, param.data(), length);
			buff += length;
		}
		
//		cout << "Event::serialize - Fin\n";
	}
	
	unsigned int loadSerialized(char *buff){
//		cout << "Event::loadSerialized - Inicio\n";
		
		// Guardo el original para calcular desplazamiento
		char *buff_original = buff;
		
		memcpy((char*)&gen, buff, sizeof(int));
		buff += sizeof(int);
		
		char type_char = *buff;
		type = (EventType)type_char;
		buff += 1;
		
		unsigned int n_params = 0;
		memcpy((char*)&n_params, buff, sizeof(int));
		buff += sizeof(int);
		for(unsigned int i = 0; i < n_params; ++i){
			double param = 0;
			memcpy((char*)&param, buff, sizeof(double));
			buff += sizeof(double);
//			cout << "Event::loadSerialized - Cargando num_params: " << param << "\n";
			num_params.push_back(param);
		}
		
		n_params = 0;
		memcpy((char*)&n_params, buff, sizeof(int));
		buff += sizeof(int);
		for(unsigned int i = 0; i < n_params; ++i){
			// Primero cargo length en un int
			unsigned int length = 0;
			memcpy((char*)&length, buff, sizeof(int));
			buff += sizeof(int);
			// Preparo un buffer local (tambien puede ser pedido con new antes y verificando el largo para repedirlo)
//			cout << "Event::loadSerialized - Preparando param_buff: " << (length+1) << "\n";
			char param_buff[length + 1];
			// Despues cargo los length chars de texto
			memcpy(param_buff, buff, length);
			buff += length;
			param_buff[length] = 0;
			// Finalmente preparo el string real
			string param(param_buff);
//			cout << "Event::loadSerialized - Cargando text_params: " << param << "\n";
			text_params.push_back(param);
		}
		
//		cout << "Event::loadSerialized - Fin\n";
		return (buff-buff_original);
	}
};

#endif
