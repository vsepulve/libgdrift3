#ifndef _MODEL_H_
#define _MODEL_H_

#include <map>
#include <vector>
#include <random>
#include <assert.h>

#include "NanoTimer.h"
#include "Population.h"
#include "Profile.h"
#include "Individual.h"

using namespace std;

class Model{

protected:
	// Por ahora no estoy usando el type, pero sera necesario para multiples modelos
	// Notar que PODRIA REQUERIR UN FACTORY por polimorfismo
	unsigned int type;

private:

public:

	Model();
	Model(const Model &original);
	Model& operator=(const Model& original);
	virtual Model *clone();
	virtual ~Model();
	
	virtual void run(Population *population, Profile *profile, mt19937 &generator);
	
	// Metodo de debug
	virtual void print(){
		cout << "Model::print - type: " << type << "\n";
	}
	
	// Notar que por ahora solo estoy guardando un entero type en la serializacion
	// Esto es solo una formalidad, no lo estoy usando de momento
	
	virtual unsigned int serializedSize(){
		return sizeof(int);
	}
	
	virtual void serialize(char *buff){
//		cout<<"Model::serialize - Guardando type " << type << "\n";
		memcpy(buff, (char*)&type, sizeof(int));
	}
	
	virtual unsigned int loadSerialized(char *buff){
//		cout<<"Model::loadSerialized - Inicio\n";
		
		// Guardo el original para calcular desplazamiento
		char *buff_original = buff;
		
		memcpy((char*)&type, buff, sizeof(int));
		buff += sizeof(int);
		
//		cout<<"Model::loadSerialized - type: " << type << "\n";
		
//		cout<<"Model::loadSerialized - Fin (bytes usados: " << (buff - buff_original) << ")\n";
		return (buff - buff_original);
	}
	
	
	
	
	
	
	
	
};

#endif
