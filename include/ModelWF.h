#ifndef _MODEL_WF_H_
#define _MODEL_WF_H_

#include "Model.h" 

using namespace std;

class ModelWF : public Model{

private:

	vector<Individual> dst;
	
	unsigned int processDNAGenes(unsigned int marker_pos, ProfileMarker &marker, Pool *pool, mt19937 &generator);
	unsigned int processMSGenes(unsigned int marker_pos, ProfileMarker &marker, unsigned int ploidy, Pool *pool, mt19937 &generator);

public:

	ModelWF();
	ModelWF(const ModelWF &original);
	ModelWF& operator=(const ModelWF& original);
	virtual Model *clone();
	virtual ~ModelWF();
	
	virtual void run(Population *population, Profile *profile, mt19937 &generator);
	
	// Metodo de debug
	virtual void print(){
		cout << "ModelWF::print - type: " << type << "\n";
	}
	
};

#endif
