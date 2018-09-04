#ifndef _PROFILE_MARKER_H_
#define _PROFILE_MARKER_H_

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <assert.h>

#include <vector>

using namespace std;

enum MarkerType { MARKER_ERROR=0, MARKER_SEQUENCE=1 , MARKER_MS=2 };
enum MutationType { MUTATION_ERROR=0, MUTATION_BASIC=1 };

class ProfileMarker{

private:

	MarkerType type;
	unsigned int len;
	unsigned int initial_alleles;
	MutationType mut_type;
	vector<double> params;
	
public:
	
	
	ProfileMarker();
	ProfileMarker(MarkerType _type, unsigned int _len, unsigned int _initial_alleles, MutationType _mut_type, vector<double> &_params);
	virtual ~ProfileMarker();
	
	MarkerType getType() const;
	
	unsigned int getLength() const;
	
	unsigned int getInitialAlleles() const;
	
	MutationType getMutationType() const;
	
//	const vector<double> &getParams() const;
	
	unsigned int getNumParam() const;
	
	double getParam(unsigned int pos) const;
	
	void print() const{
		cout << "MarkerType: " << type;
		cout << ", Length: " << len;
		cout << ", InitialAlleles: " << initial_alleles;
		cout << ", MutationType: " << mut_type;
		cout << ", Params: ";
		for( double p : params )
			cout << p << " | ";
		cout << "\n";
	}
	
	// Metodos de Serializacion
	
	unsigned int serializedSize(){
//		MarkerType type;
		unsigned int n_bytes = 1;
//		unsigned int len;
		n_bytes += sizeof(int);
//		unsigned int initial_alleles;
		n_bytes += sizeof(int);
//		MutationType mut_type;
		n_bytes += 1;
		// n_params
		n_bytes += sizeof(int);
//		vector<double> params;
		n_bytes += params.size() * sizeof(double);
		return n_bytes;
	}
	
	void serialize(char *buff){
		
		*buff = (char)type;
		buff += 1;
		
		memcpy(buff, (char*)&len, sizeof(int));
		buff += sizeof(int);
		
		memcpy(buff, (char*)&initial_alleles, sizeof(int));
		buff += sizeof(int);
		
		*buff = (char)mut_type;
		buff += 1;
		
		unsigned int n_params = params.size();
		memcpy(buff, (char*)&n_params, sizeof(int));
		buff += sizeof(int);
		
		for(double param : params){
			memcpy(buff, (char*)&param, sizeof(double));
			buff += sizeof(double);
		}
		
	}
	
	unsigned int loadSerialized(char *buff){
		
		char *buff_original = buff;
		
		char type_char = *buff;
		type = (MarkerType)(type_char);
		buff += 1;
		
		memcpy((char*)&len, buff, sizeof(int));
		buff += sizeof(int);
		
		memcpy((char*)&initial_alleles, buff, sizeof(int));
		buff += sizeof(int);
		
		type_char = *buff;
		mut_type = (MutationType)(type_char);
		buff += 1;
		
		unsigned int n_params = 0;
		memcpy((char*)&n_params, buff, sizeof(int));
		buff += sizeof(int);
		
		for(unsigned int i = 0; i < n_params; ++i){
			double param = 0;
			memcpy((char*)&param, buff, sizeof(double));
			buff += sizeof(double);
			params.push_back(param);
		}
		
		return (buff - buff_original);
	}
	
	
	
	
};

#endif
