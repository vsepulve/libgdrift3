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
	assert(pos < markers.size());
	return markers[pos];
}
	
void Profile::setPloidy(unsigned int _ploidy){
	ploidy = (unsigned char)_ploidy;
}

void Profile::addMarker(ProfileMarker &marker){
	markers.push_back(marker);
}









