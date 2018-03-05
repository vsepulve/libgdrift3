#include "Model.h"

Model::Model(){
	type = 1;
}

Model::Model(const Model &original){
	type = original.type;
}

Model& Model::operator=(const Model& original){
	if (this != &original){
		type = original.type;
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
