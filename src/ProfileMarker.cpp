#include "ProfileMarker.h"


ProfileMarker::ProfileMarker(){
	type = MARKER_ERROR;
	len = 0;
	initial_alleles = 0;
	mut_type = MUTATION_ERROR;
}

ProfileMarker::ProfileMarker(MarkerType _type, unsigned int _len, unsigned int _initial_alleles, MutationType _mut_type, vector<double> &_params)
	: params(_params) {
	type = _type;
	len = _len;
	initial_alleles = _initial_alleles;
	mut_type = _mut_type;
//	params.insert(params.begin(), _params.begin(), _params.end());
}

ProfileMarker::~ProfileMarker(){
	
}

MarkerType ProfileMarker::getType() const{
	return type;
}

unsigned int ProfileMarker::getLength() const{
	return len;
}

unsigned int ProfileMarker::getInitialAlleles() const{
	return initial_alleles;
}

MutationType ProfileMarker::getMutationType() const{
	return mut_type;
}

//const vector<double> &ProfileMarker::getParams() const{
//	return params;
//}

unsigned int ProfileMarker::getNumParam() const{
	return params.size();
}

double ProfileMarker::getParam(unsigned int pos) const{
	if(pos < params.size()){
		return params[pos];
	}
	return 0.0;
}
