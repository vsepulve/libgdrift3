#ifndef _PROFILE_H_
#define _PROFILE_H_

#include <iostream>
#include <fstream>

#include <vector>

#include "ProfileMarker.h"

using namespace std;

class Profile{

private:
	unsigned char ploidy;
	vector<ProfileMarker> markers;
	
public:

	Profile();
	Profile(const Profile &original);
	Profile& operator=(const Profile& original);
	virtual Profile *clone();
	virtual ~Profile();
	
	unsigned int getPloidy() const;
	
	const vector<ProfileMarker> &getMarkers() const;
	
	unsigned int getNumMarkers() const;
	
	const ProfileMarker &getMarker(unsigned int pos) const;
	
	void setPloidy(unsigned int _ploidy);
	
	void addMarker(ProfileMarker &marker);
	
	
	
};

#endif
