#include "SimulatorFactory.h"

SimulatorFactory::SimulatorFactory(string json_file, mt19937 *_generator){

	if(_generator != NULL){
		generator = _generator;
	}
	else{
		random_device seed;
		generator = new mt19937(seed());
	}
	
	ifstream reader (json_file, ifstream::in);
//	json settings;
	reader >> settings;
	
//	cout << "SimulatorFactory - Settings: " << settings << "\n";
	
	// Parsear el settings y guardarlo como objeto interno?
	// La otra opcion es guardar directamente el json settings y leerlo cada vez que se cree una nueva instancia
	// En ese caso igual se necesitaria un metodo que conozca el detalle de la estructura del json
	
//	cout << "SimulatorFactory - Individual: " << settings["individual"] << "\n";
//	cout << "SimulatorFactory - N of Scenarios: " << settings["scenarios"].size() << " ("<< settings["scenarios"] <<")\n";
//	parseEventsOld(settings["scenarios"]);
	
	
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

double SimulatorFactory::generate(json &json_dist){
//	cout<<"generate - Inicio\n";
	string type = json_dist["type"];
	double value = 0.0;
	if( type.compare("uniform") == 0 ){
//		cout<<"generate - UNIFORM\n";
		double a = stod(json_dist["params"]["a"].get<string>());
		double b = stod(json_dist["params"]["b"].get<string>());
		uniform_real_distribution<> uniform(a, b);
		value = uniform(*generator);
	}
	else if( type.compare("normal") == 0 ){
//		cout<<"generate - NORMAL\n";
		double mean = stod(json_dist["params"]["mean"].get<string>());
		double stddev = stod(json_dist["params"]["stddev"].get<string>());
		normal_distribution<> normal(mean, stddev);
		value = normal(*generator);
	}
	else if( type.compare("gamma") == 0 ){
//		cout<<"generate - GAMMA\n";
		double alpha = stod(json_dist["params"]["alpha"].get<string>());
		double beta = stod(json_dist["params"]["beta"].get<string>());
		gamma_distribution<double> gamma(alpha, beta);
		value = gamma(*generator);
	}
	else{
		cerr<<"generate - Error, unknown distribution "<<type<<"\n";
	}
//	cout<<"generate - Fin (" << value << ")\n";
	return value;
}

double SimulatorFactory::parseValue(json &json_val, bool force_limits, double forced_min, double forced_max){
//	cout<<"parseValue - Inicio ("<< json_val <<")\n";
	// Si es directamente un string, solo hay que convertirlo
	if( json_val.type() == json::value_t::string ){
//		cout<<"parseValue - Valor directo, parseando\n";
		string text = json_val.get<string>();
		return stod(text);
	}
	string type = json_val["type"];
//	cout<<"parseValue - " << type << "\n";
	double value = 0.0;
	if( type.compare("random") == 0 ){
//		cout<<"parseValue - Dist " << json_val["distribution"] << "\n";
		value = generate(json_val["distribution"]);
	}
	else{
//		cout<<"parseValue - Value " << json_val["value"] << "\n";
		value = stod(json_val["value"].get<string>());
	}
	if(force_limits){
		if(value < forced_min){
			value = forced_min;
		}
		else if(value > forced_max){
			value = forced_max;
		}
	}
//	cout<<"parseValue - Fin (value: "<<value<<")\n";
	return value;
}

// Este es uno de los metodos que DEBE conocer la estructura del json
// Los metodos old* usan la estructura heredada de settings, previo a optimizaciones
EventList *SimulatorFactory::parseEventsOld(json &scen){
//	cout << "SimulatorFactory::parseEventsOld - Inicio\n";
//	cout << "SimulatorFactory::parseEventsOld - N of Scenarios: " << scenarios.size() << " ("<< scenarios <<")\n";
	EventList *events = new EventList();
//	unsigned int pos = (*generator)() % scenarios.size();
//	json scen = scenarios[pos];
//	cout << "SimulatorFactory::parseEventsOld - scen: "<< pos <<" (" << scen << ")\n";
//	events->setId( scen["id"].get<unsigned int>() );
	events->setId( stoi(scen["id"].get<string>()) );
	
	// Tomar lista de eventos del json
//	json scen_events = scen[]
	unsigned int count = 0;
	unsigned int last_gen = 0;
	// TODO: Este limite de seguridad al tamaño de la poblacion es arbitrario
	unsigned int max_value = 100000000;
//	events->resize( scen["events"].size() );
//	cout << "-----\n";
//	cout << "SimulatorFactory::parseEventsOld - Iniciando Parsing\n";
//	cout << "-----\n";
	for( json json_ev : scen["events"] ){
//		cout << "SimulatorFactory::parseEventsOld - json_ev[" << count << "]: "<< json_ev <<"\n";
//		cout << "SimulatorFactory::parseEventsOld - Parsing event " << count << "\n";
//		Event event = events->getEvent(count);
		Event *event = new Event();
		events->addEvent(event);
		
		// Tiempo del evento (generacion)
		unsigned int gen = parseValue(json_ev["timestamp"], true, 0, max_value);
		if( gen < last_gen ){
			gen = last_gen + 1;
		}
		last_gen = gen;
		event->setGeneration(gen);
		
		// Tipo del evento
		string type = json_ev["type"];
//		cout<<"Type: " << type << "\n";
		// Seteo el tipo en el bloque siguiente
		
		// Parametros especificos
		json json_params = json_ev["params"];
		if( type.compare("create") == 0 ){
			event->setType(CREATE);
			string pop_name = json_params["population"]["name"];
			unsigned int size = parseValue(json_params["population"]["size"], true, 0, max_value);
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
				cerr<<"SimulatorFactory::parseEventsOld - SPLIT Warning, partitions != 2 (" << partitions << ").\n";
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
			// LO que sigue deberia ser desde el json
			event->addNumParam( 0.5 );
		}
		else if( type.compare("migration") == 0 ){
			event->setType(MIGRATE);
			// source.population.name
			// source.population.percentage
			// destination.population.name
			string src = json_params["source"]["population"]["name"];
			string dst = json_params["destination"]["population"]["name"];
			double percentage = parseValue(json_params["source"]["population"]["percentage"], true, 0, 1.0);
			event->addTextParam( src );
			event->addTextParam( dst );
			event->addNumParam( percentage );
		}
		else if( type.compare("merge") == 0 ){
			event->setType(MERGE);
			unsigned int n_sources = json_params["source"].size();
			if( n_sources != 2 ){
				cerr<<"SimulatorFactory::parseEventsOld - MERGE Warning, sources != 2 (" << n_sources << ").\n";
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
			double percentage = parseValue(json_params["source"]["population"]["percentage"], true, 0, max_value);
			event->addTextParam( src );
			event->addNumParam( percentage );
		}
		else if( type.compare("decrement") == 0 ){
			event->setType(DECREASE);
			string src = json_params["source"]["population"]["name"];
			double percentage = parseValue(json_params["source"]["population"]["percentage"], true, 0, 1.0);
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

Profile *SimulatorFactory::parseProfileOld(json &individual){
//	cout << "SimulatorFactory::parseProfileOld - Inicio\n";
	
	Profile *profile = new Profile();
	
	profile->setPloidy( stoi(individual["ploidy"].get<string>()) );
	
	json chromosomes = individual["chromosomes"];
	for(unsigned int chr = 0; chr < chromosomes.size(); ++chr){
		json genes = chromosomes[chr]["genes"];
		for(unsigned int gen = 0; gen < genes.size(); ++gen){
			json this_gene = genes[gen];
			unsigned int type = stoi(this_gene["type"].get<string>());
			if( type == 0 ){
				// MARKER_SEQUENCE
				unsigned int length = stoi(this_gene["nucleotides"].get<string>());
				unsigned int initial_alleles = stoi(this_gene["number-of-alleles"].get<string>());
				unsigned int mutation_type = stoi(this_gene["mutation"]["model"].get<string>());
				if( mutation_type == 0 ){
					// MUTATION_BASIC
					double rate = parseValue(this_gene["mutation"]["rate"], true, 0, 1.0);
//					cout << "SimulatorFactory::parseProfileOld - Agregando marcador (length: " << length 
//						<< ", initial_alleles: " << initial_alleles 
//						<< ", rate: " << rate << ")\n";
					vector<double> params;
					params.push_back(rate);
					ProfileMarker marker(MARKER_SEQUENCE, length, initial_alleles, MUTATION_BASIC, params);
					profile->addMarker(marker);
				}
				else{
//					cerr << "SimulatorFactory::parseProfileOld - Unknown Mutation Type (" << mutation_type << ")\n";
				}
			}
			else{
//				cerr << "SimulatorFactory::parseProfileOld - Unknown Marker Type (" << type << ")\n";
			}
		}
	}
	
//	cout << "SimulatorFactory::parseProfileOld - Fin\n";
	return profile;
}

bool SimulatorFactory::replaceDistribution(json &param, pair<double, double> &values){
//	cout << "SimulatorFactory::replaceDistribution - Inicio\n";
//	cout << "SimulatorFactory::replaceDistribution - param: " << param << "\n";

	if(values.second == 0 ){
		param["type"] = "fixed";
		param["value"] = std::to_string(values.first);
	}
	else{
		param["type"] = "random";
		json this_distribution;
		this_distribution["type"] = "normal";
		this_distribution["params"]["mean"] = std::to_string(values.first);
		this_distribution["params"]["stddev"] = std::to_string(values.second);
		param["distribution"] = this_distribution;
	}

//	string type = param["type"];
//	if( type.compare("random") == 0 && values.second != 0 ){
//		// Elimino distribucion antigua (tecnicamente innecesario)
//		param.erase("distribution");
//		// Preparo nueva distribucion
//		json this_distribution;
//		this_distribution["type"] = "normal";
//		this_distribution["params"]["mean"] = std::to_string(values.first);
//		this_distribution["params"]["stddev"] = std::to_string(values.second);
//		// Reintegro la nueva distribucion
//		param["distribution"] = this_distribution;
//		return true;
//	}
	
//	cout << "SimulatorFactory::replaceDistribution - Fin (Omitido)\n";
	return true;
}
// Recibe un vector de pares <mean, stddev> en el orden de los parametros
void SimulatorFactory::loadScenario(){
//	cout <<  "SimulatorFactory::loadScenario - Inicio\n";
	
	// Parametros a preparar
	n_populations = 0;
	n_stats = 0;
	n_params = 0;
//	vector<string> param_names;
	string param_name;
	
	// Primero el profile
	json &individual = settings["individual"];
	json &chromosomes = individual["chromosomes"];
	for(unsigned int chr = 0; chr < chromosomes.size(); ++chr){
		json &genes = chromosomes[chr]["genes"];
		for(unsigned int gen = 0; gen < genes.size(); ++gen){
			json &this_gene = genes[gen];
			unsigned int type = stoi(this_gene["type"].get<string>());
			if( type == 0 ){
				// MARKER_SEQUENCE
				unsigned int mutation_type = stoi(this_gene["mutation"]["model"].get<string>());
				if( mutation_type == 0 ){
					// MUTATION_BASIC
					param_name = "mutation.rate." + param_names.size();
					param_names.push_back(param_name);
				}
				else{
//					cerr << "SimulatorFactory::parseProfileOld - Unknown Mutation Type (" << mutation_type << ")\n";
				}
			}
			else{
//				cerr << "SimulatorFactory::parseProfileOld - Unknown Marker Type (" << type << ")\n";
			}
		}
	}
	
	// De aqui en adelante asumo que solo hay un escenario por simulacion (nuevo modelo)
//	cout <<  "SimulatorFactory::loadScenario - Tomando Escenario 0\n";
	json &scen = settings["scenarios"][0];
	map<string, unsigned int> counted_events;
	
//	cout <<  "SimulatorFactory::loadScenario - Ajustando Eventos\n";
	for( json &json_ev : scen["events"] ){
		
		// Notar que SIEMPRE se carga prmero la generacion, luego los parametros en orden de lectura
		
		// Tipo del evento
		string type = json_ev["type"];
		counted_events[type]++;
		
		param_name = type + ".";
		if( counted_events[type] > 1 ){
			param_name += to_string(counted_events[type]) + ".";
		}
		param_name += "generation";
		param_names.push_back(param_name);
		
		// Parametros especificos
		if( type.compare("create") == 0 ){
			// create -> size
			param_name = "create.";
			if( counted_events[type] > 1 ){
				param_name += to_string(counted_events[type]) + ".";
			}
			param_name += "size";
			param_names.push_back(param_name);
			// Se crea una poblacion
			++n_populations;
		}
		else if( type.compare("split") == 0 ){
			// split -> SIN parametros (el porcentaje lo fijamos en 0.5 en old)
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
			param_names.push_back(param_name);
		}
		else if( type.compare("decrement") == 0 ){
			// decrement -> percentage
			param_name = "decrement.";
			if( counted_events[type] > 1 ){
				param_name += to_string(counted_events[type]) + ".";
			}
			param_name += "percentage";
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
	n_stats = 5 * n_populations;
	n_params = param_names.size();
	
	cout <<  "SimulatorFactory::loadScenario - n_populations: " << n_populations << ", n_stats: " << n_stats << ", n_params: " << n_params << "\n";

//	cout <<  "SimulatorFactory::loadScenario - Fin\n";
	
}

// Recibe un vector de pares <mean, stddev> en el orden de los parametros
void SimulatorFactory::reloadParameters(vector<pair<double, double>> &values){
//	cout <<  "SimulatorFactory::reloadParameters - Inicio\n";
	unsigned int next_param = 0;
	
//	cout <<  "SimulatorFactory::reloadParameters - Ajustando Perfil\n";
	// Primero el profile
	json &individual = settings["individual"];
	json &chromosomes = individual["chromosomes"];
	for(unsigned int chr = 0; chr < chromosomes.size(); ++chr){
		json &genes = chromosomes[chr]["genes"];
		for(unsigned int gen = 0; gen < genes.size(); ++gen){
			json &this_gene = genes[gen];
			unsigned int type = stoi(this_gene["type"].get<string>());
			if( type == 0 ){
				// MARKER_SEQUENCE
//				cout <<  "SimulatorFactory::reloadParameters - Tomando mutation_type\n";
				unsigned int mutation_type = stoi(this_gene["mutation"]["model"].get<string>());
//				cout <<  "SimulatorFactory::reloadParameters - mutation_type: " << mutation_type << "\n";
				if( mutation_type == 0 ){
					// MUTATION_BASIC
//					cout <<  "SimulatorFactory::reloadParameters - [mutation][rate]\n";
					json &this_param = this_gene["mutation"]["rate"];
					if( replaceDistribution(this_param, values[next_param]) ){
						++next_param;
					}
				}
				else{
//					cerr << "SimulatorFactory::parseProfileOld - Unknown Mutation Type (" << mutation_type << ")\n";
				}
			}
			else{
//				cerr << "SimulatorFactory::parseProfileOld - Unknown Marker Type (" << type << ")\n";
			}
		}
	}
	
	// Luego proceso los eventos en el mismo orden de generacion
	// Por ahora asumo escenario 0
	// Una opcion es recibir el escenario como parametro
	// Si se recibe el id, hay que iterar por los escenario hasta elegir el correcto
	
//	cout <<  "SimulatorFactory::reloadParameters - Tomando Escenario 0\n";
	json &scen = settings["scenarios"][0];
	
//	cout <<  "SimulatorFactory::reloadParameters - Ajustando Eventos\n";
	for( json &json_ev : scen["events"] ){
		
		// Notar que SIEMPRE se carga prmero la generacion, luego los parametros en orden de lectura
		
		json &this_param = json_ev["timestamp"];
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
			if( replaceDistribution(this_param, values[next_param]) ){
				++next_param;
			}
		}
		else if( type.compare("split") == 0 ){
			// split -> SIN parametros (el porcentaje lo fijamos en 0.5 en old)
		}
		else if( type.compare("migration") == 0 ){
			// migration -> percentage
			json &this_param = json_params["source"]["population"]["percentage"];
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
			if( replaceDistribution(this_param, values[next_param]) ){
				++next_param;
			}
		}
		else if( type.compare("decrement") == 0 ){
			// decrement -> percentage
			json &this_param = json_params["source"]["population"]["percentage"];
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
	
//	cout <<  "SimulatorFactory::reloadParameters - Fin\n";
	
}

Simulator *SimulatorFactory::getInstance(){
//	cout << "SimulatorFactory::getInstance - Inicio\n";
	
//	cout << "SimulatorFactory::getInstance - new Simulator...\n";
	Simulator *res = new Simulator();
	// Detectar el model de settings
//	cout << "SimulatorFactory::getInstance - new ModelWF...\n";
	res->setModel( new ModelWF() );
//	cout << "SimulatorFactory::getInstance - parseEventsOld...\n";
	
	// Verificar si settings tiene solo 1 scenario, o varios.
	if( settings.find("scenarios") != settings.end() 
		&& settings["scenarios"].size() > 0 ){
//		cout << "SimulatorFactory::getInstance - Multiple Scenarios\n";
		unsigned int pos = (*generator)() % settings["scenarios"].size();
//		cout << "SimulatorFactory::getInstance - Parsing scenario " << pos << " / " << settings["scenarios"].size() << "\n";
		res->setEvents( parseEventsOld( settings["scenarios"][pos] ) );
	}
	else if( settings.find("scenario") != settings.end() ){
//		cout << "SimulatorFactory::getInstance - Single Scenario\n";
		res->setEvents( parseEventsOld( settings["scenario"] ) );
	}
	else{
		cerr << "SimulatorFactory::getInstance - No Scenario found\n";
	}
	
//	cout << "SimulatorFactory::getInstance - parseProfileOld...\n";
	res->setProfile( parseProfileOld( settings["individual"] ) );
	
//	cout << "SimulatorFactory::getInstance - Fin\n";
	return res;
}

char *SimulatorFactory::getInstanceSerialized(){
	// return NULL;
	// En una version posterior, la idea es evitar crea un nuevo simulator
	// La idea es generar directamente cada parte serializadamente
	Simulator *sim = getInstance();
	char *serialized = sim->serialize();
	delete sim;
	return serialized;
}





