#include "Profile.h"

Profile::Profile(){
	ploidy = 0;
}

Profile::Profile(json &individual){
//	cout << "Profile - Inicio\n";
	ParsingUtils utils;
	setPloidy( individual["Plody"] );
//	cout << "Profile - Cargando Marcadores\n";
	for( json &marker : individual["Markers"] ){
//		cout << "Profile - marker: " << marker << "\n";
		unsigned int marker_type =  marker["Type"];
		unsigned int mutation_model = marker["Mutation_model"];
		if( marker_type == 1 ){
			// MARKER_SEQUENCE
			if( mutation_model == 1 ){
				// MUTATION_BASIC
				unsigned int length = marker["Size"];
				unsigned int pool_size = marker["Pool_size"];
				double rate = utils.parseValue(marker["Rate"], true, 0, 1.0);
				vector<double> params;
				params.push_back(rate);
				ProfileMarker marker(MARKER_SEQUENCE, length, pool_size, MUTATION_BASIC, params);
				addMarker(marker);
			}
			else{
				cerr << "Profile - Unknown Mutation Model (" << mutation_model << ")\n";
			}
		}
		else if( marker_type == 2 ){
			// MICROSATELLITES
			if( mutation_model == 1 ){
				// MUTATION_BASIC
				unsigned int pool_size = marker["Pool_size"];
				double rate = utils.parseValue(marker["Rate"], true, 0, 1.0);
				// Quizas despues habria que agregar parámetros de la geometrica
				// Por otra parte, quizas sea mejor eso para mutation_model = 2
				vector<double> params;
				params.push_back(rate);
				ProfileMarker marker(MARKER_MS, 0, pool_size, MUTATION_BASIC, params);
				addMarker(marker);
			}
			else{
				cerr << "Profile - Unknown Mutation Model (" << mutation_model << ")\n";
			}
		}
		else{
			cerr << "Profile - Unknown Marker Type (" << marker_type << ")\n";
		}
	}
//	cout << "Profile - Fin\n";
}

Profile::Profile(const Profile &original){
	ploidy = original.getPloidy();
}

Profile& Profile::operator=(const Profile& original){
	if (this != &original){
		ploidy = original.getPloidy();
	}
	return *this;
}

Profile *Profile::clone(){
	return new Profile(*this);
}

Profile::~Profile(){
//	cout << "~Profile - Start\n";
	markers.clear();
//	cout << "~Profile - End\n";
}

unsigned int Profile::getPloidy() const{
	return ploidy;
}
	
const vector<ProfileMarker> &Profile::getMarkers() const{
	return markers;
}
	
unsigned int Profile::getNumMarkers() const{
	return markers.size();
}

const ProfileMarker &Profile::getMarker(unsigned int pos) const{
	assert(pos < markers.size());
	return markers[pos];
}
	
void Profile::setPloidy(unsigned int _ploidy){
	ploidy = (unsigned char)_ploidy;
}

void Profile::addMarker(ProfileMarker &marker){
	markers.push_back(marker);
}









