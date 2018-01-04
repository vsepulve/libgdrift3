#include "ProfileMarker.h"


ProfileMarker::ProfileMarker(){
	type = MARKER_ERROR;
	len = 0;
	mut_type = MUTATION_ERROR;
}

ProfileMarker::ProfileMarker(MarkerType _type, unsigned int _len, MutationType _mut_type, vector<double> &_params)
	: params(_params) {
	type = _type;
	len = _len;
	mut_type = _mut_type;
//	params.insert(params.begin(), _params.begin(), _params.end());
}

ProfileMarker::~ProfileMarker(){
	
}

MarkerType ProfileMarker::getType(){
	return type;
}

unsigned int ProfileMarker::getLength(){
	return len;
}

MutationType ProfileMarker::getMutationType(){
	return mut_type;
}

vector<double> &ProfileMarker::getParams(){
	return params;
}

unsigned int ProfileMarker::getNumParam(){
	return params.size();
}

double ProfileMarker::getParam(unsigned int pos){
	if(pos < params.size()){
		return params[pos];
	}
	return 0.0;
}
