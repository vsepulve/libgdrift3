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

private:
	// Por ahora no estoy usando el type, pero sera necesario para multiples modelos
	// Notar que PODRIA REQUERIR UN FACTORY por polimorfismo
	unsigned int type;

public:

	Model();
	Model(const Model &original);
	Model& operator=(const Model& original);
	virtual Model *clone();
	virtual ~Model();
	
	virtual void run(Population *population, Profile *profile, mt19937 &generator);
	
	// Notar que por ahora solo estoy guardando un entero type en la serializacion
	// Esto es solo una formalidad, no lo estoy usando de momento
	
	virtual unsigned int serializedSize(){
		return sizeof(int);
	}
	
	virtual void serialize(char *buff){
		memcpy(buff, (char*)&type, sizeof(int));
	}
	
	virtual unsigned int loadSerialized(char *buff){
		
		// Guardo el original para calcular desplazamiento
		char *buff_original = buff;
		
		memcpy((char*)&type, buff, sizeof(int));
		buff += sizeof(int);
		
		
		return (buff - buff_original);
	}
	
	
	
	
	
	
	
	
};

#endif
