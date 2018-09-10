#include "SimulatorFactory.h"

SimulatorFactory::SimulatorFactory(json &_project_json, json &_simulation_json){
	project_json = _project_json;
	simulation_json = _simulation_json;
	// Aqui se podrian determinar los parametros globales que seran usados inter training
	loadScenario();
}

SimulatorFactory::SimulatorFactory(const SimulatorFactory &original){
}

SimulatorFactory& SimulatorFactory::operator=(const SimulatorFactory& original){
	if (this != &original){
	
	}
	return *this;
}

SimulatorFactory *SimulatorFactory::clone(){
	return new SimulatorFactory(*this);
}

SimulatorFactory::~SimulatorFactory(){
}

// Este es uno de los metodos que DEBE conocer la estructura del json
// Los metodos old* usan la estructura heredada de settings, previo a optimizaciones
EventList *SimulatorFactory::parseEventsOld(json &scen){
//	cout << "SimulatorFactory::parseEventsOld - Inicio\n";
	EventList *events = new EventList();
	events->setId( scen["Id"] );
	
	// Tomar lista de eventos del json
	unsigned int count = 0;
	unsigned int last_gen = 0;
	// TODO: Este limite de seguridad al tamaño de la poblacion es arbitrario
	unsigned int max_value = 100000000;
//	cout << "SimulatorFactory::parseEventsOld - Iniciando Parsing\n";
//	cout << "-----\n";
	for( json json_ev : scen["Events"] ){
//		cout << "SimulatorFactory::parseEventsOld - json_ev[" << count << "]: "<< json_ev <<"\n";
		Event *event = new Event();
		events->addEvent(event);
		
		// Tiempo del evento (generacion)
		unsigned int gen = utils.parseValue(json_ev["timestamp"], true, 0, max_value);
		if( gen < last_gen ){
			gen = last_gen + 1;
		}
		last_gen = gen;
		event->setGeneration(gen);
		
		// Tipo del evento
		string type = json_ev["type"];
//		cout<<"Type: " << type << "\n";
		
		// Parametros especificos
		json json_params = json_ev["params"];
		if( type.compare("create") == 0 ){
			event->setType(CREATE);
			string pop_name = json_params["population"]["name"];
			unsigned int size = utils.parseValue(json_params["population"]["size"], true, 0, max_value);
			event->addTextParam( pop_name );
			event->addNumParam( size );
		}
		else if( type.compare("split") == 0 ){
			event->setType(SPLIT);
			// TODO: NO esta el percentage del split!
			string src = json_params["source"]["population"]["name"];
			string dst1;
			string dst2;
			unsigned int partitions = stoi(json_params["partitions"].get<string>());
			if( partitions != 2 ){
				cerr << "SimulatorFactory::parseEventsOld - SPLIT Warning, partitions != 2 (" << partitions << ").\n";
				dst1 = "dst_1_" + to_string(gen);
				dst2 = "dst_2_" + to_string(gen);
			}
			else{
				dst1 = json_params["destination"][0]["population"]["name"];
				dst2 = json_params["destination"][1]["population"]["name"];
			}
			event->addTextParam( src );
			event->addTextParam( dst1 );
			event->addTextParam( dst2 );
			// Lo que sigue deberia ser desde el json (pero no es claro si de source o por destination)
			double percentage = 0.5;
//			double percentage = utils.parseValue(json_params["source"]["population"]["percentage"], true, 0, 1.0);
			event->addNumParam( percentage );
		}
		else if( type.compare("migration") == 0 ){
			event->setType(MIGRATE);
			string src = json_params["source"]["population"]["name"];
			string dst = json_params["destination"]["population"]["name"];
			double percentage = utils.parseValue(json_params["source"]["population"]["percentage"], true, 0, 1.0);
			event->addTextParam( src );
			event->addTextParam( dst );
			event->addNumParam( percentage );
		}
		else if( type.compare("merge") == 0 ){
			event->setType(MERGE);
			unsigned int n_sources = json_params["source"].size();
			if( n_sources != 2 ){
				cerr << "SimulatorFactory::parseEventsOld - MERGE Warning, sources != 2 (" << n_sources << ").\n";
			}
			string src1 = json_params["source"][0]["population"]["name"];
			string src2 = json_params["source"][1]["population"]["name"];
			string dst = json_params["destination"]["population"]["name"];
			event->addTextParam( src1 );
			event->addTextParam( src2 );
			event->addTextParam( dst );
		}
		else if( type.compare("increment") == 0 ){
			event->setType(INCREASE);
			string src = json_params["source"]["population"]["name"];
			double percentage = utils.parseValue(json_params["source"]["population"]["percentage"], true, 0, max_value);
			event->addTextParam( src );
			event->addNumParam( percentage );
		}
		else if( type.compare("decrement") == 0 ){
			event->setType(DECREASE);
			string src = json_params["source"]["population"]["name"];
			double percentage = utils.parseValue(json_params["source"]["population"]["percentage"], true, 0, 1.0);
			event->addTextParam( src );
			event->addNumParam( percentage );
		}
		else if( type.compare("extinction") == 0 ){
			event->setType(EXTINCT);
			string src = json_params["source"]["population"]["name"];
			event->addTextParam( src );
		}
		else if( type.compare("endsim") == 0 ){
			event->setType(ENDSIM);
			// Este evento NO tiene parametros, solo la generacion y el tipo
		}
//		cout << "SimulatorFactory::parseEventsOld - Event:\n";
//		event->print();
//		cout << "-----\n";
		++count;
	}
//	cout << "SimulatorFactory::parseEventsOld - Fin\n";
	return events;
}

bool SimulatorFactory::replaceDistribution(json &param, pair<double, double> &values){
//	cout << "SimulatorFactory::replaceDistribution - Inicio (" << values.first << ", " << values.second << ")\n";
	if(values.second == 0 ){
		param["type"] = "fixed";
		param["value"] = std::to_string(values.first);
	}
	else{
		param["type"] = "random";
		json this_distribution;
		this_distribution["type"] = "normal";
//		this_distribution["params"]["mean"] = std::to_string(values.first);
//		this_distribution["params"]["stddev"] = std::to_string(values.second);
		// NOTE: La conversion directa de double a string FALLA con numeros pequeños
		// Por ahora, dejo esta implementacion propia, pero no es para nada limpia
		std::ostringstream stream1;
        stream1 << std::setprecision(std::numeric_limits<double>::digits10) << values.first ;
		this_distribution["params"]["mean"] = stream1.str();
		std::ostringstream stream2;
        stream2 << std::setprecision(std::numeric_limits<double>::digits10) << values.second ;
		this_distribution["params"]["stddev"] = stream2.str();
//		cout << "SimulatorFactory::replaceDistribution - this_distribution: " << this_distribution << "\n";
		param["distribution"] = this_distribution;
	}
//	cout << "SimulatorFactory::replaceDistribution - param salida: " << param << "\n";
//	cout << "SimulatorFactory::replaceDistribution - Fin\n";
	return true;
}

void SimulatorFactory::loadScenario(){
	cout <<  "SimulatorFactory::loadScenario - Inicio\n";
	
	// Parametros a preparar
	n_populations = 0;
	n_stats = 0;
	n_params = 0;
	string param_name;
	proj_id = project_json["Id"];
	sim_id = simulation_json["Id"];
	
	cout <<  "SimulatorFactory::loadScenario - proj_id: " << proj_id << ", sim_id: " << sim_id << "\n";
	// Primero Individual (de project)
	for( json &marker : project_json["Individual"]["Markers"] ){
		unsigned int marker_type =  marker["Type"];
		unsigned int mutation_model = marker["Mutation_model"];
		if( marker_type == 1 ){
			// MARKER_SEQUENCE
			if( mutation_model == 1 ){
				// MUTATION_BASIC
				param_name = "mutation.rate." + to_string(param_names.size());
				cout <<  "SimulatorFactory::loadScenario - Agregando param \"" << param_name << "\"\n";
				param_names.push_back(param_name);
			}
			else{
				cerr << "SimulatorFactory::loadScenario - Unknown Mutation Model (" << mutation_model << ")\n";
			}
		}
		else if( marker_type == 2 ){
			// MICROSATELLITES
			if( mutation_model == 1 ){
				// MUTATION_BASIC
				param_name = "mutation.rate." + to_string(param_names.size());
				// Notar que podría haber parametros adicionales aca, como la geometrica
				cout <<  "SimulatorFactory::loadScenario - Agregando param \"" << param_name << "\"\n";
				param_names.push_back(param_name);
			}
			else{
				cerr << "SimulatorFactory::loadScenario - Unknown Mutation Model (" << mutation_model << ")\n";
			}
		}
		else{
			cerr << "SimulatorFactory::loadScenario - Unknown Marker Type (" << marker_type << ")\n";
		}
	}
	
	// Ahora el escenario de Simulation
	map<string, unsigned int> counted_events;
	
	cout <<  "SimulatorFactory::loadScenario - Ajustando Eventos\n";
	for( json &json_ev : simulation_json["Events"] ){
		
		// Notar que SIEMPRE se carga prmero la generacion, luego los parametros en orden de lectura
		
		// Tipo del evento
		string type = json_ev["type"];
		counted_events[type]++;
		
		param_name = type + ".";
		if( counted_events[type] > 1 ){
			param_name += to_string(counted_events[type]) + ".";
		}
		param_name += "generation";
		cout <<  "SimulatorFactory::loadScenario - Agregando param \"" << param_name << "\"\n";
		param_names.push_back(param_name);
		
		// Parametros especificos
		if( type.compare("create") == 0 ){
			// create -> size
			param_name = "create.";
			if( counted_events[type] > 1 ){
				param_name += to_string(counted_events[type]) + ".";
			}
			param_name += "size";
			cout <<  "SimulatorFactory::loadScenario - Agregando param \"" << param_name << "\"\n";
			param_names.push_back(param_name);
			// Se crea una poblacion
			++n_populations;
		}
		else if( type.compare("split") == 0 ){
			// split -> percentage
			param_name = "split.";
			if( counted_events[type] > 1 ){
				param_name += to_string(counted_events[type]) + ".";
			}
			param_name += "percentage";
			cout <<  "SimulatorFactory::loadScenario - Agregando param \"" << param_name << "\"\n";
			param_names.push_back(param_name);
			// Se elimina una poblacion, y se agregan 2 (por ahora fijo en old)
			++n_populations;
		}
		else if( type.compare("migration") == 0 ){
			// migration -> percentage
			param_name = "migration.";
			if( counted_events[type] > 1 ){
				param_name += to_string(counted_events[type]) + ".";
			}
			param_name += "percentage";
			cout <<  "SimulatorFactory::loadScenario - Agregando param \"" << param_name << "\"\n";
			param_names.push_back(param_name);
			// Crea una nueva poblacion
			++n_populations;
		}
		else if( type.compare("merge") == 0 ){
			// merge -> SIN parametros (solo los nombres)
			// Elimina 2 poblaciones, crea solo 1
			--n_populations;
		}
		else if( type.compare("increment") == 0 ){
			// increment -> percentage
			param_name = "increment.";
			if( counted_events[type] > 1 ){
				param_name += to_string(counted_events[type]) + ".";
			}
			param_name += "percentage";
			cout <<  "SimulatorFactory::loadScenario - Agregando param \"" << param_name << "\"\n";
			param_names.push_back(param_name);
		}
		else if( type.compare("decrement") == 0 ){
			// decrement -> percentage
			param_name = "decrement.";
			if( counted_events[type] > 1 ){
				param_name += to_string(counted_events[type]) + ".";
			}
			param_name += "percentage";
			cout <<  "SimulatorFactory::loadScenario - Agregando param \"" << param_name << "\"\n";
			param_names.push_back(param_name);
		}
		else if( type.compare("extinction") == 0 ){
			// merge -> SIN parametros (solo el nombre)
			// Elimina 1 poblacion
			--n_populations;
		}
		else if( type.compare("endsim") == 0 ){
			// Este evento NO tiene parametros, solo la generacion y el tipo
		}
	}
	
	// Notar que este 5 depende de los estadisticos
	// Agrego la poblacion de summay
	++n_populations;
	Profile profile(project_json["Individual"]);
	
	n_stats = 0;
	for( unsigned int i = 0; i < profile.getNumMarkers(); ++i ){
		if( profile.getMarker(i).getType() == MARKER_SEQUENCE ){
			n_stats += 5 * n_populations;
		}
		else if( profile.getMarker(i).getType() == MARKER_MS ){
			n_stats += 3 * n_populations;
		}
	}
	
	n_params = param_names.size();
	
	cout <<  "SimulatorFactory::loadScenario - n_populations: " << n_populations << ", n_stats: " << n_stats << ", n_params: " << n_params << "\n";
	
//	cout <<  "SimulatorFactory::loadScenario - Fin\n";
}

// Recibe un vector de pares <mean, stddev> en el orden de los parametros
void SimulatorFactory::reloadParameters(vector<pair<double, double>> &values){
	cout <<  "SimulatorFactory::reloadParameters - Inicio\n";
	unsigned int next_param = 0;
	
	// Primero Individual (de project)
	cout <<  "SimulatorFactory::reloadParameters - Ajustando Individual\n";
	for( json &marker : project_json["Individual"]["Markers"] ){
		unsigned int marker_type =  marker["Type"];
		unsigned int mutation_model = marker["Mutation_model"];
		if( marker_type == 1 ){
			// MARKER_SEQUENCE
			if( mutation_model == 1 ){
				// MUTATION_BASIC
				json &this_param = marker["rate"];
				cout <<  "SimulatorFactory::reloadParameters - Reemplazando " << param_names[next_param] << " -> (" << values[next_param].first << ", " << values[next_param].second << ")\n";
				if( replaceDistribution(this_param, values[next_param]) ){
					++next_param;
				}
			}
			else{
				cerr << "SimulatorFactory::reloadParameters - Unknown Mutation Model (" << mutation_model << ")\n";
			}
		}
		else if( marker_type == 2 ){
			// MICROSATELLITES
			if( mutation_model == 1 ){
				// MUTATION_BASIC
				json &this_param = marker["rate"];
				cout <<  "SimulatorFactory::reloadParameters - Reemplazando " << param_names[next_param] << " -> (" << values[next_param].first << ", " << values[next_param].second << ")\n";
				if( replaceDistribution(this_param, values[next_param]) ){
					++next_param;
				}
				// Notar que podría haber parametros adicionales aca, como la geometrica
			}
			else{
				cerr << "SimulatorFactory::reloadParameters - Unknown Mutation Model (" << mutation_model << ")\n";
			}
		}
		else{
			cerr << "SimulatorFactory::reloadParameters - Unknown Marker Type (" << marker_type << ")\n";
		}
	}
	
	
	// Luego proceso los eventos en el mismo orden de generacion
	// Por ahora asumo escenario 0
	// Una opcion es recibir el escenario como parametro
	// Si se recibe el id, hay que iterar por los escenario hasta elegir el correcto
	
	cout <<  "SimulatorFactory::reloadParameters - Ajustando Eventos\n";
	for( json &json_ev : simulation_json["Events"] ){
		
		// Notar que SIEMPRE se carga prmero la generacion, luego los parametros en orden de lectura
		
		json &this_param = json_ev["timestamp"];
		cout <<  "SimulatorFactory::reloadParameters - Reemplazando " << param_names[next_param] << " -> (" << values[next_param].first << ", " << values[next_param].second << ")\n";
		if( replaceDistribution(this_param, values[next_param]) ){
			++next_param;
		}
		
		// Tipo del evento
		string type = json_ev["type"];
		
		// Parametros especificos
		json &json_params = json_ev["params"];
		if( type.compare("create") == 0 ){
			// create -> size
			json &this_param = json_params["population"]["size"];
			cout <<  "SimulatorFactory::reloadParameters - Reemplazando " << param_names[next_param] << " -> (" << values[next_param].first << ", " << values[next_param].second << ")\n";
			if( replaceDistribution(this_param, values[next_param]) ){
				++next_param;
			}
		}
		else if( type.compare("split") == 0 ){
			// split -> SIN parametros (el porcentaje lo fijamos en 0.5 en old)
			// FIX: eñ porcentaje igual esta pasando en la fase de training, asi que lo omito explícitamente por ahora
//			if( values[next_param].first == 0.5 && values[next_param].second == 0.0 ){
//				cout << "SimulatorFactory::reloadParameters - Omitiendo split (dist " << values[next_param].first << ", " << values[next_param].second << ")\n";
//				++next_param;
//			}
			json &this_param = json_params["source"]["population"]["percentage"];
			cout <<  "SimulatorFactory::reloadParameters - Reemplazando " << param_names[next_param] << " -> (" << values[next_param].first << ", " << values[next_param].second << ")\n";
			if( replaceDistribution(this_param, values[next_param]) ){
				++next_param;
			}
		}
		else if( type.compare("migration") == 0 ){
			// migration -> percentage
			json &this_param = json_params["source"]["population"]["percentage"];
			cout <<  "SimulatorFactory::reloadParameters - Reemplazando " << param_names[next_param] << " -> (" << values[next_param].first << ", " << values[next_param].second << ")\n";
			if( replaceDistribution(this_param, values[next_param]) ){
				++next_param;
			}
		}
		else if( type.compare("merge") == 0 ){
			// merge -> SIN parametros (solo los nombres)
		}
		else if( type.compare("increment") == 0 ){
			// increment -> percentage
			json &this_param = json_params["source"]["population"]["percentage"];
			cout <<  "SimulatorFactory::reloadParameters - Reemplazando " << param_names[next_param] << " -> (" << values[next_param].first << ", " << values[next_param].second << ")\n";
			if( replaceDistribution(this_param, values[next_param]) ){
				++next_param;
			}
		}
		else if( type.compare("decrement") == 0 ){
			// decrement -> percentage
			json &this_param = json_params["source"]["population"]["percentage"];
			cout <<  "SimulatorFactory::reloadParameters - Reemplazando " << param_names[next_param] << " -> (" << values[next_param].first << ", " << values[next_param].second << ")\n";
			if( replaceDistribution(this_param, values[next_param]) ){
				++next_param;
			}
		}
		else if( type.compare("extinction") == 0 ){
			// merge -> SIN parametros (solo el nombre)
		}
		else if( type.compare("endsim") == 0 ){
			// Este evento NO tiene parametros, solo la generacion y el tipo
		}
	}
	
	cout <<  "SimulatorFactory::reloadParameters - Fin\n";
	
}

Simulator *SimulatorFactory::getInstance(){
//	cout << "SimulatorFactory::getInstance - Inicio\n";
	
//	cout << "SimulatorFactory::getInstance - new Simulator...\n";
	Simulator *res = new Simulator();
	res->setId(sim_id);
	// Detectar el model de Simulation (por ahora asumo WF)
//	cout << "SimulatorFactory::getInstance - new ModelWF...\n";
	res->setModel( new ModelWF() );
	
//	cout << "SimulatorFactory::getInstance - parseEventsOld...\n";
	res->setEvents( parseEventsOld( simulation_json ) );
	
//	cout << "SimulatorFactory::getInstance - new Profile...\n";
	res->setProfile( new Profile(project_json["Individual"]) );
	
//	cout << "SimulatorFactory::getInstance - Fin\n";
	return res;
}

char *SimulatorFactory::getInstanceSerialized(){
	// return NULL;
	// En una version posterior, la idea es evitar crea un nuevo simulator
	// La idea es generar directamente cada parte serializadamente
	Simulator *sim = getInstance();
	sim->setId(sim_id);
	char *serialized = sim->serialize();
	delete sim;
	return serialized;
}





