#include "ParsingUtils.h"

ParsingUtils::ParsingUtils(){
	random_device seed;
	generator = mt19937(seed());
}

ParsingUtils::~ParsingUtils(){
	
}

double ParsingUtils::generate(json &json_dist){
//	cout << "ParsingUtils::generate - Inicio\n";
	string type = json_dist["type"];
	double value = 0.0;
	if( type.compare("uniform") == 0 ){
//		cout << "ParsingUtils::generate - UNIFORM\n";
		double a = stod(json_dist["params"]["a"].get<string>());
		double b = stod(json_dist["params"]["b"].get<string>());
		uniform_real_distribution<> uniform(a, b);
		value = uniform(generator);
	}
	else if( type.compare("normal") == 0 ){
//		cout << "ParsingUtils::generate - NORMAL\n";
		double mean = stod(json_dist["params"]["mean"].get<string>());
		double stddev = stod(json_dist["params"]["stddev"].get<string>());
		normal_distribution<> normal(mean, stddev);
		value = normal(generator);
	}
	else if( type.compare("gamma") == 0 ){
//		cout << "ParsingUtils::generate - GAMMA\n";
		double alpha = stod(json_dist["params"]["alpha"].get<string>());
		double beta = stod(json_dist["params"]["beta"].get<string>());
		gamma_distribution<double> gamma(alpha, beta);
		value = gamma(generator);
	}
	else{
		cerr << "ParsingUtils::generate - Error, unknown distribution "<<type<<"\n";
	}
//	cout << "ParsingUtils::generate - Fin (" << value << ")\n";
	return value;
}

double ParsingUtils::parseValue(json &json_val, bool force_limits, double forced_min, double forced_max){
//	cout << "ParsingUtils::parseValue - Inicio ("<< json_val <<")\n";
	// Si es directamente un string, solo hay que convertirlo
	if( json_val.type() == json::value_t::string ){
		string text = json_val.get<string>();
		return stod(text);
	}
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
//	cout << "ParsingUtils::parseValue - Fin (value: "<<value<<")\n";
	return value;
}
