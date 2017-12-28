#include "SimulatorFactory.h"

SimulatorFactory::SimulatorFactory(const char *json_file, mt19937 *_generator){

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
	
	cout << "SimulatorFactory - Settings: " << settings << "\n";
	
	// Parsear el settings y guardarlo como objeto interno?
	// La otra opcion es guardar directamente el json settings y leerlo cada vez que se cree una nueva instancia
	// En ese caso igual se necesitaria un metodo que conozca el detalle de la estructura del json
	
	cout << "SimulatorFactory - Individual: " << settings["individual"] << "\n";
//	cout << "SimulatorFactory - N of Scenarios: " << settings["scenarios"].size() << " ("<< settings["scenarios"] <<")\n";
	parseEventsOld(settings["scenarios"]);
	
	
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
		double a = stod(json_dist["params.a"].get<string>());
		double b = stod(json_dist["params.b"].get<string>());
		uniform_real_distribution<> uniform(a, b);
		value = uniform(*generator);
	}
	else if( type.compare("normal") == 0 ){
//		cout<<"generate - NORMAL\n";
		double mean = stod(json_dist["params.mean"].get<string>());
		double stddev = stod(json_dist["params.stddev"].get<string>());
		normal_distribution<> normal(mean, stddev);
		value = normal(*generator);
	}
	else if( type.compare("gamma") == 0 ){
//		cout<<"generate - GAMMA\n";
		double alpha = stod(json_dist["params.alpha"].get<string>());
		double beta = stod(json_dist["params.beta"].get<string>());
		gamma_distribution<double> gamma(alpha, beta);
		value = gamma(*generator);
	}
	else{
		cerr<<"generate - Error, unknown distribution "<<type<<"\n";
	}
	return value;
}

double SimulatorFactory::parseValue(json &json_val, bool force_limits, double forced_min, double forced_max){
	cout<<"parseValue - Inicio\n";
	string type = json_val["type"];
	double value = 0.0;
	if( type.compare("random") == 0 ){
		value = generate(json_val["distribution"]);
	}
	else{
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
	cout<<"parseValue - Fin (value: "<<value<<")\n";
	return value;
}

// Este es uno de los metodos que DEBE conocer la estructura del json
// Los metodos old* usan la estructura heredada de settings, previo a optimizaciones
EventList *SimulatorFactory::parseEventsOld(json &scenarios){
	cout << "SimulatorFactory::parseEventsOld - Inicio (num of scenarios: " << scenarios.size() << ")\n";
//	cout << "parseEventsOld - N of Scenarios: " << scenarios.size() << " ("<< scenarios <<")\n";
	EventList *events = new EventList();
	unsigned int pos = (*generator)() % scenarios.size();
	json scen = scenarios[pos];
	cout << "SimulatorFactory::parseEventsOld - scen: "<< pos <<" (" << scen << ")\n";
//	events->setId( scen["id"].get<unsigned int>() );
	events->setId( stoi(scen["id"].get<string>()) );
	
	// Tomar lista de eventos del json
//	json scen_events = scen[]
	unsigned int count = 0;
	unsigned int last_gen = 0;
	// TODO: Este limite de seguridad al tamaño de la poblacion es arbitrario
	unsigned int max_value = 100000000;
	events->resize( scen["events"].size() );
	for( json json_ev : scen["events"] ){
		cout << "SimulatorFactory::parseEventsOld - json_ev[" << count << "]: "<< json_ev <<"\n";
		Event event = events->getEvent(count);
		
		// Tiempo del evento (generacion)
		unsigned int gen = parseValue(json_ev["timestamp"], true, 0, max_value);
		if( gen < last_gen ){
			gen = last_gen + 1;
		}
		last_gen = gen;
		event.setGeneration(gen);
		
		// Tipo del evento
		string type = json_ev["type"];
		cout<<"Type: " << type << "\n";
		// Seteo el tipo en el bloque siguiente
		
		// Parametros especificos
		json json_params = json_ev["params"];
		if( type.compare("create") == 0 ){
			event.setType(CREATE);
			string pop_name = json_params["population"]["name"];
			unsigned int size = parseValue(json_ev["population"]["size"], true, 0, max_value);
			event.addTextParam( pop_name );
			event.addNumParam( size );
		}
		else if( type.compare("split") == 0 ){
			event.setType(SPLIT);
			// TODO: NO esta el percentage del split!
			string src = json_params["source"]["population"]["name"];
			string dst1;
			string dst2;
			unsigned int partitions = json_params["partitions"];
			if( partitions != 2 ){
				cerr<<"SimulatorFactory::parseEventsOld - SPLIT Warning, partitions != 2 (" << partitions << ").\n";
				dst1 = "dst_1_" + to_string(gen);
				dst2 = "dst_2_" + to_string(gen);
			}
			else{
				dst1 = json_params["destination"][0]["population"]["name"];
				dst2 = json_params["destination"][1]["population"]["name"];
			}
			event.addTextParam( src );
			event.addTextParam( dst1 );
			event.addTextParam( dst2 );
			// LO que sigue deberia ser desde el json
			event.addNumParam( 0.5 );
		}
		
		++count;
	}
	
	cout << "SimulatorFactory::parseEventsOld - Fin\n";
	return events;
}

Simulator *SimulatorFactory::getInstance(){
	
	
	Simulator *res = new Simulator();
	// Detectar el model de settings
//	res->setModel( new ModelFW() );
	res->setEvents( parseEventsOld( settings["scenarios"] ) );
	
	
	
	
	
	
	
	
	
	
	return res;
}

char *SimulatorFactory::getInstanceSerialized(){
	return NULL;
}

