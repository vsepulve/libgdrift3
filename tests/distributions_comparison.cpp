
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <sstream>
#include <fstream>
#include <string.h>

using namespace std;

#define STAT_SQR2PI 2.506628274631000

int main(int argc,char** argv) {

	if(argc != 6){
		cout<<"\nUsage: ./test mean_1 stddev_1 mean_2 stddev_2 output\n";
		cout<<"\n";
		return 0;
	}
	double mean_1 = atof(argv[1]);
	double stddev_1 = atof(argv[2]);
	double mean_2 = atof(argv[3]);
	double stddev_2 = atof(argv[4]);
	string output = argv[5];
	
	unsigned int n_points = 30;
	
	cout<<"Test - Inicio\n";
	
	// Los graficare en un rango fijo de +- 3 sigma
	
	double min_1 = mean_1 - 4 * stddev_1;
	double min_2 = mean_2 - 4 * stddev_2;
	double max_1 = mean_1 + 4 * stddev_1;
	double max_2 = mean_2 + 4 * stddev_2;
	
	double min = (min_1 < min_2)?min_1:min_2;
	double max = (max_1 > max_2)?max_1:max_2;
	
	double step = (max - min) / n_points;
	double next = min;
	
	fstream writer(output, fstream::out | fstream::trunc);
	for(unsigned int i = 0; i < n_points; ++i){
		double x = next;
		next += step;
		
		double y_1 = exp( - ( pow((x-mean_1), 2.0) ) / ( 2.0*stddev_1*stddev_1 ) ) / (STAT_SQR2PI * stddev_1);
		double y_2 = exp( - ( pow((x-mean_2), 2.0) ) / ( 2.0*stddev_2*stddev_2 ) ) / (STAT_SQR2PI * stddev_2);
		
		cout << x << "\t" << y_1 << "\t" << y_2 << "\n";
		writer << x << "\t" << y_1 << "\t" << y_2 << "\n";
		
	}
	
	writer.close();
	
	
	
	
	
	
	
	
	
	
	
	
	cout<<"Test - Fin\n";

	return 0;
	
	
}















