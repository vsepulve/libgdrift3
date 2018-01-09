#include "Model.h"

Model::Model(){
}

Model::Model(const Model &original){
}

Model& Model::operator=(const Model& original){
	if (this != &original){
	
	}
	return *this;
}

Model *Model::clone(){
	return new Model(*this);
}

Model::~Model(){
	
}

void Model::run(Population *population, Profile *profile, mt19937 &generator){
	cout << "Model::run - Not Implemented.\n";
}
