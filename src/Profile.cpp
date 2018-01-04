#include "Profile.h"

Profile::Profile(){
	ploidy = 0;
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
	if(pos >= markers.size()){
		cerr<<"Profile::getMarker - Error, invalid position ("<<pos<<" >= "<<markers.size()<<")\n";
		exit(EXIT_FAILURE);
	}
	return markers[pos];
}
