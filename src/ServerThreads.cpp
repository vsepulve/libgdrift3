#include "ServerThreads.h"

// Metodo solo para comodidad del processing_thread
// Este metodo podria ser parte de un paquete utils o un objeto estadistico
vector<double> get_statistics(Simulator *sim, float sampling){
	vector<double> res;
	Statistics stats(sim, sampling);
//	cout << "get_statistics - Preparing Statistics\n";
	map<string, vector< map<string, double> > > statistics = stats.getStatistics();
	// Agrupo los estadisticos en el mismo orden, population_name primero, n marcadores, y stat name al final
	for( auto &par : statistics ){
//		cout << "get_statistics - Stats Population \"" << par.first << "\"\n";
//		unsigned int marker_pos = 0;
		for( map<string, double> &marker_stats : par.second ){
//			cout << "get_statistics - [ Marker " << marker_pos++ << " ]\n";
			for( auto &stat : marker_stats ){
//				cout<<"Test - " << stat.first << ": "<< stat.second <<"\n";
				res.push_back(stat.second);
			}
//			cout << "-----\n";
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
void processing_thread(unsigned int pid, string output_base, WorkManager *manager, Analyzer *analyzer){
	
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
		
		unsigned int sim_id = sim.getId();
		unsigned int feedback = manager->getFeedback(sim_id);
		// NOTE: Lo que sigue asume que el id ES UNICO
		string file_name = output_base;
		file_name += std::to_string(sim_id);
		file_name += "_f";
		file_name += std::to_string(feedback);
		file_name += "_";
		file_name += std::to_string(pid);
		file_name += ".txt";
		
		cout << "processing_thread[" << pid << "] - Ejecutando Sim " << sim_id << ", feedback " << feedback << ", finished " << manager->getFinished(sim_id) << " / " << manager->getTotal(sim_id) << "\n";
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
		
		// Verificar si es necesario ejecutar el analyzer
		manager->addFinished(sim_id);
		unsigned int finished = manager->getFinished(sim_id);
		unsigned int total = manager->getTotal(sim_id);
		// Cambio la condicion para que SOLO lo ejecute en el caso de igualdad
		if( finished == total ){
			cout << "processing_thread[" << pid << "] - Ejecutando Analyzer por fin de batch (" << finished << " / " << total << ")\n";
			analyzer->execute(sim_id);
		}
		
	}// while... trabajo en la cola
	
//	global_mutex.lock();
	cout << "processing_thread[" << pid << "] - Fin (Total trabajos: " << procesados << ", Total ms: " << timer.getMilisec() << ")\n";
//	global_mutex.unlock();
	
}

// INIT: Crear el target y preparar datos
// Este proceso tambien recibe y guarda el json del proyecto (ademas del target)
void thread_init_project(int sock_cliente, string json_project_base, string target_base, WorkManager *manager){
	// Por ahora, asumo que el archivo de datos de secuencias esta en disco
	// Despues puedo parchar eso creando ese archivo desde el servicio go que inicia este thread
	// Tambien asumo que ya hay un project_id, despues veo si es necesario cambiar eso
	
	ClientReception conexion;
	conexion.setSocket(sock_cliente);
	
	cout << "Server::thread_init_project - Start\n";
	bool error = false;
	unsigned int size = 0;
	unsigned int project_id = 0;
	unsigned int n_markers = 0;
	unsigned int n_pops = 0;
	string json_file = "";
	string target_file = "";
	
	// Empiezo recibiendo project_id
	if( ! error && ! conexion.readUInt(project_id) ){
		cerr << "Server::thread_init_project - Error receiving project_id\n";
		project_id = 0;
		error = true;
	}
	cout << "Server::thread_init_project - project_id: " << project_id << "\n";
	
	// En lugar de eso, recibo el json del proyecto completo y lo almaceno asociado al id
	if( ! error && ! conexion.readUInt(size) ){
		cerr << "Server::thread_init_project - Error receiving json size\n";
		size = 0;
		error = true;
	}
	if(size > MAX_READ){
		cerr << "Server::thread_init_project - Warning, size > MAX_READ (size: " << size << ")\n";
		size = MAX_READ;
	}
	cout << "Server::thread_init_project - size: " << size << "\n";
	char buff[size + 1];
	if( ! error && ! conexion.readData(buff, size) ){
		cerr << "Server::thread_init_project - Error receiving json\n";
		size = 0;
		error = true;
	}
	buff[size] = 0;
	
	// Si todo esta ok, guardo el texto del json
	if(! error ){
		json_file = json_project_base;
		json_file += to_string(project_id);
		json_file += ".json";
		
		fstream writer(json_file, fstream::out | fstream::trunc);
		if( writer.good() ){
			writer.write(buff, size);
			writer.close();
		}
		else{
			cerr << "Server::thread_init_project - Error opening file \"" << json_file << "\"\n";
			error = true;
		}
	}
	
	// Si todo esta ok, parsear el json y generar el target
	if(! error ){
		
		cout << "Server::thread_init_project - Parseando json\n";
		string json_text(buff);
		stringstream json_stream(json_text);
		json json_project;
		json_stream >> json_project;
		
		// Leer N_populations del json
		n_pops = json_project["N_populations"];
		cout << "Server::thread_init_project - n_pops: " << n_pops << "\n";
		
		// Iterar por Individual -> Markers
		Profile profile(json_project["Individual"]);
		n_markers = profile.getNumMarkers();
		cout << "Server::thread_init_project - n_markers: " << n_markers << "\n";
		
		// Cada uno tiene N_populations rutas en el arreglo Sample_path
		// Proceso esto en el orden de estadisticos: por cada poblacion (ordenada por nombre) => los datos de cada marcador
		// El mapa siguinte es pop_name -> vector por marcador
		map<string, vector<string> > sample_paths;
		for( unsigned int i = 0; i < n_pops; ++i ){
			// Considerando el nuevo statistics, quiza un solo archivo deberia tener todos los marcadores para la poblacion
			string pop_name = json_project["Populations"][i]["Name"];
			for( unsigned int j = 0; j < n_markers; ++j ){
				string sample_path = json_project["Populations"][i]["Sample_path"][j];
				cout<<"Server::thread_init_project - sample[" << pop_name << "][" << j << "]: " << sample_path << "\n";
				sample_paths[pop_name].push_back(sample_path);
			}
		}
		
		// Cada uno de esos archivos debe ser cargado en una poblacion
		// Tomar los estadisticos de esas poblaciones
		
		// Estructura temporal para almacenar summary populations
		vector<vector<string>> summary_alleles;
		Statistics stats;
		for( auto samples : sample_paths ){
			// Los metodos siguientes necesitan, al menos, el profile para saber que datos leer y estadisticos generar
			stats.processStatistics(samples.second[0], samples.first, &profile, &summary_alleles);
		}
		stats.processStatistics("summary", &profile, &summary_alleles);
		
		// Crear el target con los estadisticos generados
		target_file = target_base;
		target_file += to_string(project_id);
		target_file += ".txt";
		
		// Preparar escritor y crear el archivo 
		fstream writer(target_file, fstream::out | fstream::trunc);
		if( writer.good() ){
			char stat_buff[1024];
			memset(stat_buff, 0, 1024);
//			map<string, vector< map<string, double> > > statistics = stats.getStatistics();
			for( auto it_stats_pop : stats.getStatistics() ){
				string pop_name = it_stats_pop.first;
				unsigned int marker_pos = 0;
				for( map<string, double> stats_marker : it_stats_pop.second ){
					for( auto it_stat : stats_marker ){
						string stat_name = it_stat.first;
						double value = it_stat.second;
						cout << "Server::thread_init_project - pop " << pop_name << " - marker " << marker_pos << " - stat " << stat_name <<" -> " << value << "\n";
						sprintf(stat_buff + strlen(stat_buff), "%f\t", value);
					}
					++marker_pos;
				}
			}
			sprintf(stat_buff + strlen(stat_buff), "\n");
			writer.write(stat_buff, strlen(stat_buff));
			writer.close();
		}
		else{
			cerr << "Server::thread_init_project - Error opening file \"" << target_file << "\"\n";
			error = true;
		}
		
	}
	
	// Envio codigo de exito al cliente
	if( error ){
		cout << "Server::thread_init_project - Sending error code to client\n";
		conexion.writeUInt(1);
	}
	else{
		cout << "Server::thread_init_project - Sending ok code to client\n";
		conexion.writeUInt(0);
	}
	
	cout << "Server::thread_init_project - End\n";
}


// START: Agregar trabajo a work_queue e iniciar simulaciones
void thread_start_sim(int sock_cliente, string json_sim_base, string json_project_base, WorkManager *manager){
	
	ClientReception conexion;
	conexion.setSocket(sock_cliente);
	
	cout << "Server::thread_start_sim - Start\n";
	
	bool error = false;
	unsigned int size = 0;
	unsigned int project_id = 0;
	unsigned int sim_id = 0;
	unsigned int n_sims = 10000;
	
	// Empiezo recibiendo project_id
	if( ! error && ! conexion.readUInt(project_id) ){
		cerr << "Server::thread_start_sim - Error receiving project_id\n";
		project_id = 0;
		error = true;
	}
	cout << "Server::thread_start_sim - project_id: " << project_id << "\n";
	
	// luego sim_id
	if( ! error && ! conexion.readUInt(sim_id) ){
		cerr << "Server::thread_start_sim - Error receiving sim_id\n";
		sim_id = 0;
		error = true;
	}
	cout << "Server::thread_start_sim - sim_id: " << sim_id << "\n";
	
	// Recibo el json de la simulacion (size y bytes)
	if( ! error && ! conexion.readUInt(size) ){
		cerr << "Server::thread_start_sim - Error receiving json size\n";
		size = 0;
		error = true;
	}
	if(size > MAX_READ){
		cerr << "Server::thread_start_sim - Warning, size > MAX_READ (size: " << size << ")\n";
		size = MAX_READ;
	}
	cout << "Server::thread_start_sim - size: " << size << "\n";
	char buff[size + 1];
	if( ! error && ! conexion.readData(buff, size) ){
		cerr << "Server::thread_start_sim - Error receiving json\n";
		size = 0;
		error = true;
	}
	buff[size] = 0;
	
	// Si todo esta ok, guardo el texto del json
	if(! error ){
		string json_sim_file = json_sim_base;
		json_sim_file += to_string(sim_id);
		json_sim_file += ".json";
		
		fstream writer(json_sim_file, fstream::out | fstream::trunc);
		if( writer.good() ){
			writer.write(buff, size);
			writer.close();
		}
		else{
			cerr << "Server::thread_start_sim - Error opening file \"" << json_sim_file << "\"\n";
			error = true;
		}
	}
	
	// Si todo esta ok, parsear el json y agrego el factory al manager
	if(! error ){
		string json_text(buff);
		stringstream json_stream(json_text);
		json json_sim;
		json_stream >> json_sim;
		
		cout << "Server::thread_start_sim - Eventos: " << json_sim["Events"] << "\n";
		
		string json_file = json_project_base;
		json_file += to_string(project_id);
		json_file += ".json";
		
		cout << "Server::thread_start_sim - Parseando json del Proyecto\n";
		ifstream reader(json_file, ifstream::in);
		json json_project;
		reader >> json_project;
		
		cout << "Server::thread_start_sim - Iniciando manager->addWork...\n";
		manager->addWork(sim_id, json_project, json_sim, n_sims);
		cout << "Server::thread_start_sim - addWork terminado\n";
		
	}
	
	// Envio codigo de exito al cliente
	if( error ){
		cout << "Server::thread_start_sim - Sending error code to client\n";
		conexion.writeUInt(1);
	}
	else{
		cout << "Server::thread_start_sim - Sending ok code to client\n";
		conexion.writeUInt(0);
	}
	
}

void thread_query_sim(int sock_cliente, string json_sim_base, string json_project_base, WorkManager *manager, Analyzer *analyzer){
	
	ClientReception conexion;
	conexion.setSocket(sock_cliente);
	
	cout << "Server::thread_query_sim - Start\n";
	
	bool error = false;
//	unsigned int size = 0;
	unsigned int sim_id = 0;
//	unsigned int n_sims = 10;
	
	// luego sim_id
	if( ! error && ! conexion.readUInt(sim_id) ){
		cerr << "Server::thread_query_sim - Error receiving sim_id\n";
		sim_id = 0;
		error = true;
	}
	cout << "Server::thread_query_sim - sim_id: " << sim_id << "\n";
	
	analyzer->partialResults(sim_id);
	
	
	
	
	
	// Por ahora envio una respuesta de prueba directamente
	
	// Primero un codigo de que hay datos (en int)
	conexion.writeUInt(1);
	
	string resp = "Test Response";
	// Notar que envio solo los bytes de la respuesta en forma cruda
	// Eso es porque go usara TODOS los bytes como string
	conexion.writeData(resp.c_str(), resp.length());
//	conexion.writeByte(0);
	
	
	
	
	
	
	
}


