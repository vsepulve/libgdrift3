#ifndef _SIMULATOR_FACTORY_H_
#define _SIMULATOR_FACTORY_H_

#include <iostream>
#include <fstream>
#include <random>

#include <nlohmann/json.hpp>

#include <map>

#include "NanoTimer.h"
#include "Simulator.h"
#include "Population.h"
#include "Profile.h"
#include "Model.h"
#include "ModelWF.h"

using json = nlohmann::json;

using namespace std;

class SimulatorFactory{

private:

	mt19937 *generator;
	// Opcion directa, guardar el json parseado
	json settings;

public:
	
	// Constructor principal, recive el json de settings (con las distribuciones)
	SimulatorFactory(const char *json_file, mt19937 *_generator = NULL);
	SimulatorFactory(const SimulatorFactory &original);
	SimulatorFactory& operator=(const SimulatorFactory& original);
	virtual SimulatorFactory *clone();
	virtual ~SimulatorFactory();
	
	// Notar que getInstance normalmente es un metodo estatico
	// Sin embargo, en este caso usaremos instancias diferentes de factory
	// Asi es mas simple almacenar la configuracion y distribuciones de cada grupo de simulaciones
	Simulator *getInstance();
	
	// Este metodo retorna la version serializada simple de un Simulator
	// Esto es para evitar la construccion de una instancia cuando solo se quieren los parametros
	// Tambien serivira para evitar instancias entre comunicacion de threads
	char *getInstanceSerialized();
	
	EventList *parseEventsOld(json &scenarios);
	
	
	double generate(json &json_dist);
	double parseValue(json &json_val, bool force_limits, double forced_min, double forced_max);
	
};















#endif
