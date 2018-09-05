#ifndef _PARSING_UTILS_H
#define _PARSING_UTILS_H

#include <map>
#include <vector>
#include <random>
#include <assert.h>

#include <nlohmann/json.hpp>

#include "NanoTimer.h"

using json = nlohmann::json;
using namespace std;

class ParsingUtils{

private:
	mt19937 generator;

public:

	ParsingUtils();
	virtual ~ParsingUtils();
	
	double generate(json &json_dist);
	double parseValue(json &json_val, bool force_limits, double forced_min, double forced_max);
	
};

#endif
