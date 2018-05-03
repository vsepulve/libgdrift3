#include "ServerThreads.h"

// Metodo solo para comodidad del processing_thread
// Este metodo podria ser parte de un paquete utils o un objeto estadistico
vector<double> get_statistics(Simulator *sim, float sampling){
	vector<double> res;
	Statistics stats(sim, sampling);
	map<string, vector< map<string, double> > > statistics = stats.getStatistics();
	// Agrupo los estadisticos en el mismo orden, population_name primero, n marcadores, y stat name al final
	for( auto &par : statistics ){
//		cout<<"get_statistics - Stats Population \""<< par.first <<"\"\n";
//		unsigned int marker_pos = 0;
		for( map<string, double> &marker_stats : par.second ){
//			cout<<"get_statistics - [ Marker "<<marker_pos++<<" ]\n";
			for( auto &stat : marker_stats ){
//				cout<<"Test - " << stat.first << ": "<< stat.second <<"\n";
				res.push_back(stat.second);
			}
//			cout<<"-----\n";
		}
	}
	return res;
}

// Metodo solo para comodidad del processing_thread
// Este metodo podria ser parte de un paquete utils o un objeto estadistico
vector<double> get_params(Simulator *sim){
	vector<double> res;
	
	// No voy a comparar parametros punto a punto con los datos antiguos, solo las distribuciones
	// Por esto, en este caso el orden NO esta atado a los nombres json de las variables
	// Simplemente extraere los parametros del profile, y luego de cada evento
	// Esto asume obviamente que todas las simulaciones comparadas tienen los mismos parametros
	
	Profile *profile = sim->getProfile();
	for(unsigned int i = 0; i < profile->getNumMarkers(); ++i){
		ProfileMarker marker = profile->getMarker(i);
		for(unsigned int j = 0; j < marker.getNumParam(); ++j){
			res.push_back( marker.getParam(j) );
		}
	}
	
	// Obviamente en el caso de los eventos solo considero la generacion y los parametros numericos
	EventList *events = sim->getEvents();
	for(unsigned int i = 0; i < events->size(); ++i){
		Event *event = events->getEvent(i);
		res.push_back( (double)(event->getGeneration()) );
		for( double value : event->getNumParams() ){
			res.push_back(value);
		}
	}
	
	return res;
}

// Thread de procesamiento principal
void processing_thread(unsigned int pid, string output_base, WorkManager *manager, Analizer *analizer){
	
//	global_mutex.lock();
	cout << "processing_thread[" << pid << "] - Inicio\n";
//	global_mutex.unlock();
	
	NanoTimer timer;
	unsigned int procesados = 0;
	unsigned int sleep_time = 5;
	
	while(true){
		
		if( manager->isKilled() ){
			break;
		}
		
		char *serialized = manager->getWork();
		// Verificar si esta dada la señal de termino
		if( serialized == NULL ){
			std::this_thread::sleep_for (std::chrono::seconds(sleep_time));
			continue;
		}
		
		Simulator sim;
		sim.loadSerialized(serialized);
		++procesados;
		
		unsigned int sim_id = 0;
//		unsigned int sim_id = sim.getId();
		// NOTE: Lo que sigue asume que el id ES UNICO
		string file_name = output_base;
		file_name += std::to_string(sim_id);
		file_name += "_";
		file_name += std::to_string(pid);
		file_name += ".txt";
		
		sim.run();
		
		// Parte local del analyzer
		// Esto requiere el target 
		// Falta definir e implementar la normalizacion
		
		vector<double> statistics = get_statistics(&sim, 0.05);
		vector<double> params = get_params(&sim);
		
		fstream writer(file_name, fstream::out | fstream::app);
		for( double value : statistics ){
			writer << value << "\t";
		}
		for( double value : params ){
			writer << value << "\t";
		}
		writer << "\n";
		writer.close();
		
		// Verificar si es necesario ejecutar el analizer
		manager->addFinished(sim_id);
		unsigned int finished = manager->getFinished(sim_id);
		unsigned int total = manager->getTotal(sim_id);
		if( finished >= total ){
			 analizer->execute(sim_id);
		}
		
	}// while... trabajo en la cola
	
//	global_mutex.lock();
	cout << "processing_thread[" << pid << "] - Fin (Total trabajos: " << procesados << ", Total ms: " << timer.getMilisec() << ")\n";
//	global_mutex.unlock();
	
}


// NOTE: que uno de los trabajos del init de servicio es CREAR el target
void thread_analyzer_init(int sock_cliente, string json_file_base){
	
	ClientReception conexion;
	conexion.setSocket(sock_cliente);
	
	cout << "Server::thread_analyzer_init - Start\n";
	
	bool error = false;
	unsigned int size = 0;
	
	// Primero recibo el string con el json
	if( ! conexion.readUInt(size) ){
		cerr << "Server::thread_analyzer_init - Error receiving json size\n";	
		size = 0;
		error = true;
	}
	cout << "Server::main - size: " << size << "\n";
	char buff[size + 1];
	
	if( ! error && ! conexion.readData(buff, size) ){
		cerr << "Server::thread_analyzer_init - Error receiving json data\n";	
		size = 0;
		error = true;
	}
	buff[size] = 0;
	
	// Luego guardo el texto en un archivo (esto puede cambiar a envio directo luego)
	if(! error ){
		string json_file = json_file_base;
		unsigned long long milisec = (unsigned long long)(std::time(0));
		json_file += to_string(milisec);
		json_file += ".json";
	
		fstream writer(json_file, fstream::out | fstream::trunc);
		if( writer.good() ){
			writer.write(buff, size);
			writer.close();
		}
		else{
			cerr << "Server::thread_analyzer_init - Error opening file \"" << json_file << "\"\n";
			error = true;
		}
	}
	
	// Abro comunicacion con el server Scheduler
	
	// Envio el request_type
	
	// Envio la ruta del archivo json
	
	// Espero respuesta del scheduler
	
	// Envio codigo de exito al cliente
	if( error ){
		cout << "Server::thread_analyzer_init - Sending error code to client\n";
		conexion.writeUInt(1);
	}
	else{
		cout << "Server::thread_analyzer_init - Sending ok code to client\n";
		conexion.writeUInt(0);
	}
	
	
//	unsigned int len = 0;
//	conexion.readUInt(len);
//	cout << "Server::main - len: " << len << "\n";
//	char buff[len + 1];
//	conexion.readData(buff, len);
//	buff[len] = 0;
//	cout << "Server::main - buff: \"" << buff << "\"\n";
//	
//	// Envio de un mensaje de prueba: un entero (4 bytes) con el valor "1"
//	cout << "Server::main - Enviando respuesta (1)\n";
//	conexion.writeUInt(1);
	

}


