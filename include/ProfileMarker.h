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
	unsigned int initial_alleles;
	MutationType mut_type;
	vector<double> params;
	
public:
	
	
	ProfileMarker();
	ProfileMarker(MarkerType _type, unsigned int _len, unsigned int _initial_alleles, MutationType _mut_type, vector<double> &_params);
	virtual ~ProfileMarker();
	
	MarkerType getType() const;
	
	unsigned int getLength() const;
	
	unsigned int getInitialAlleles() const;
	
	MutationType getMutationType() const;
	
//	const vector<double> &getParams() const;
	
	unsigned int getNumParam() const;
	
	double getParam(unsigned int pos) const;
	
	void print() const{
		cout << "MarkerType: " << type;
		cout << ", Length: " << len;
		cout << ", InitialAlleles: " << initial_alleles;
		cout << ", MutationType: " << mut_type;
		cout << ", Params: ";
		for( double p : params )
			cout << p << " | ";
		cout << "\n";
	}
	
	
	
	
};

#endif
