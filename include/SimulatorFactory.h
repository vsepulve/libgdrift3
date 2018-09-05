#ifndef _SIMULATOR_FACTORY_H_
#define _SIMULATOR_FACTORY_H_

#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>
#include <limits>

#include <nlohmann/json.hpp>

#include <map>

#include "NanoTimer.h"
#include "Simulator.h"
#include "Population.h"
#include "Profile.h"
#include "Model.h"
#include "ModelWF.h"
#include "ParsingUtils.h"

using json = nlohmann::json;
using namespace std;

class SimulatorFactory{

private:
	
	ParsingUtils utils;
	json project_json;
	json simulation_json;
	
	EventList *parseEventsOld(json &scenarios);
	
	// Retorna verdadero en caso de reemplazo
	// Originalmente retornaba true si hacia el cambio
	// Pero ahora que aplica fixed en varianza 0, es innecesario
	bool replaceDistribution(json &param, pair<double, double> &values);
	
	// Caracteristicas de esta simulacion
	// Para determinarlos, escaneo el json en el constructor
	unsigned int n_populations;
	unsigned int n_stats;
	unsigned int n_params;
	vector<string> param_names;
	unsigned int proj_id;
	unsigned int sim_id;
	
	void loadScenario();

public:
	
	SimulatorFactory(json &_project_json, json &_simulation_json);
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
	
	void reloadParameters(vector<pair<double, double>> &values);
	
	// Revisa el json de settings completo para determinar el numero de estadisticos
	// NOTE: de momento esto es el numero de estatisticos usados (5) por el numero de poblaciones final + 1 (summary)
	// Si se especifica esto en el json de otro modo, habria que agregarlo
	unsigned int getNumStatistics(){
		return n_stats;
	}
	
	// Revisa el json de settings completo para determinar el numero de parametros
	// Para eso evalua los individuos y eventos en el mismo orden que reloadParameters
	unsigned int getNumParams(){
		return n_params;
	}
	
	// Revisa el json de settings completo para determinar el numero de parametros
	// Para eso evalua los individuos y eventos en el mismo orden que reloadParameters
	vector<string> &getParams(){
		return param_names;
	}
	
	unsigned int getProjectId(){
		return proj_id;
	}
	
};















#endif
