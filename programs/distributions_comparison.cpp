
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <sstream>
#include <fstream>
#include <string.h>

#include <vector>

using namespace std;

#define STAT_SQR2PI 2.506628274631000

int main(int argc,char** argv) {

	if(argc != 5){
		cout<<"\nUsage: ./test n_params distributions_1 distributions_2 output_base\n";
		cout<<"\n";
		return 0;
	}
	unsigned int n_params = atoi(argv[1]);
	string input_1 = argv[2];
	string input_2 = argv[3];
	string output_base = argv[4];
	
	unsigned int n_points = 30;
	
	cout<<"Test - Inicio\n";
	
	vector<double> mean_1;
	vector<double> stddev_1;
	
	vector<double> mean_2;
	vector<double> stddev_2;
	
	unsigned int buff_size = 1020;
	char buff[buff_size];
	fstream lector;
	
	cout<<"Test - Cargando \"" << input_1 << "\"\n";
	lector.open(input_1, fstream::in);
	for(unsigned int i = 0; i < n_params; ++i){
		lector.getline(buff, buff_size);
		if( !lector.good() || strlen(buff) < 1 ){
			break;
		}
		
		string line(buff);
		stringstream toks(line);
		
		string tok;
		
		toks >> tok;
		cout << "Parametro \"" << tok << "\" - ";
		
		double mean = 0.0;
		toks >> mean;
		
		double stddev = 0.0;
		toks >> stddev;
		cout << mean << " | " << stddev << "\n";
		
		mean_1.push_back(mean);
		stddev_1.push_back(stddev);
	}
	lector.close();
	
	cout<<"Test - Cargando \"" << input_2 << "\"\n";
	lector.open(input_2, fstream::in);
	for(unsigned int i = 0; i < n_params; ++i){
		lector.getline(buff, buff_size);
		if( !lector.good() || strlen(buff) < 1 ){
			break;
		}
		
		string line(buff);
		stringstream toks(line);
		
		string tok;
		
		toks >> tok;
		cout << "Parametro \"" << tok << "\" - ";
		
		double mean = 0.0;
		toks >> mean;
		
		double stddev = 0.0;
		toks >> stddev;
		cout << mean << " | " << stddev << "\n";
		
		mean_2.push_back(mean);
		stddev_2.push_back(stddev);
	}
	lector.close();
	
	
	cout<<"Test - Procesando distribuciones\n";
	
	vector<double> errors;
	
	for(unsigned int i = 0; i < n_params; ++i){
		
		cout<<"Test - Parametro " << i << "\n";
		
		double min_1 = mean_1[i] - 4 * stddev_1[i];
		double min_2 = mean_2[i] - 4 * stddev_2[i];
		double max_1 = mean_1[i] + 4 * stddev_1[i];
		double max_2 = mean_2[i] + 4 * stddev_2[i];
	
		double min = (min_1 < min_2)?min_1:min_2;
		double max = (max_1 > max_2)?max_1:max_2;
	
		double step = (max - min) / n_points;
		double next = min;
		
		string output = output_base;
		output += "_" + to_string(i) + ".txt";
		fstream writer(output, fstream::out | fstream::trunc);
		double error = 0.0;
		double t1 = 0.0;
		double t2 = 0.0;
		for(unsigned int k = 0; k < n_points; ++k){
			double x = next;
			next += step;
			
			double y_1 = exp( - ( pow((x - mean_1[i]), 2.0) ) / ( 2.0 * stddev_1[i] * stddev_1[i] ) ) / (STAT_SQR2PI * stddev_1[i]);
			double y_2 = exp( - ( pow((x - mean_2[i]), 2.0) ) / ( 2.0 * stddev_2[i] * stddev_2[i] ) ) / (STAT_SQR2PI * stddev_2[i]);
		
			error += step * ((y_1>y_2)?(y_1-y_2):(y_2-y_1));
			t1 += step * y_1;
			t2 += step * y_2;
		
//			cout << x << "\t" << y_1 << "\t" << y_2 << "\n";
			writer << x << "\t" << y_1 << "\t" << y_2 << "\n";
		
		}
		cout<<"Test - Error: " << error << " (t1: " << t1 << ", t2: " << t2 << ")\n";
		cout<<"-----  -----\n";
		errors.push_back(error);
		writer.close();
	
	}
	
	double error_mean = 0;
	double error_var = 0;
	double error_stddev = 0;
	
	for(double error : errors){
		error_mean += error;
	}
	error_mean /= errors.size();
	
	
	for(double error : errors){
		error_var += pow(error - error_mean, 2.0);
	}
	error_var /= errors.size();
	error_stddev = pow(error_var, 0.5);
	
	cout<<"Test - error_mean: " << error_mean << "\n";
	cout<<"Test - error_stddev: " << error_stddev << "\n";
	cout<<"-----  -----\n";
	
	/*
	
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
	double error = 0.0;
	double t1 = 0.0;
	double t2 = 0.0;
	for(unsigned int i = 0; i < n_points; ++i){
		double x = next;
		next += step;
		
		double y_1 = exp( - ( pow((x-mean_1), 2.0) ) / ( 2.0*stddev_1*stddev_1 ) ) / (STAT_SQR2PI * stddev_1);
		double y_2 = exp( - ( pow((x-mean_2), 2.0) ) / ( 2.0*stddev_2*stddev_2 ) ) / (STAT_SQR2PI * stddev_2);
		
		error += step * ((y_1>y_2)?(y_1-y_2):(y_2-y_1));
		t1 += step * y_1;
		t2 += step * y_2;
		
		cout << x << "\t" << y_1 << "\t" << y_2 << "\n";
		writer << x << "\t" << y_1 << "\t" << y_2 << "\n";
		
	}
	cout<<"Test - Error: " << error << " (t1: " << t1 << ", t2: " << t2 << ")\n";
	writer.close();
	
	
	
	
	
	
	
	
	
	*/
	
	
	cout<<"Test - Fin\n";

	return 0;
	
	
}















