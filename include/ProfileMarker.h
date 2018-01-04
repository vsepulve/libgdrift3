#ifndef _PROFILE_MARKER_H_
#define _PROFILE_MARKER_H_

#include <iostream>
#include <fstream>

#include <vector>

using namespace std;

enum MarkerType { MARKER_ERROR=0, MARKER_SEQUENCE=1 };
enum MutationType { MUTATION_ERROR=0, MUTATION_BASIC=1 };

class ProfileMarker{

private:

	MarkerType type;
	unsigned int len;
	MutationType mut_type;
	vector<double> params;
	
public:
	
	
	ProfileMarker();
	ProfileMarker(MarkerType _type, unsigned int _len, MutationType _mut_type, vector<double> &_params);
	virtual ~ProfileMarker();
	
	MarkerType getType();
	
	unsigned int getLength();
	
	MutationType getMutationType();
	
	vector<double> &getParams();
	
	unsigned int getNumParam();
	
	double getParam(unsigned int pos);
	
	void print() const{
		cout << "MarkerType: " << type;
		cout << ", Length: " << len;
		cout << ", MutationType: " << mut_type;
		cout << ", Params: ";
		for( double p : params )
			cout << p << " | ";
		cout << "\n";
	}
	
	
	
	
};

#endif
