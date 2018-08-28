
#include <iostream>
#include <random>

#include <thread>
#include <mutex>

#include <vector>
#include <list>

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"

#include "WorkManager.h"
#include "Analyzer.h"

#include "ServerThreads.h"
#include "CommunicationUtils.h"
#include "ClientReception.h"

using namespace std;

int main(int argc,char** argv){

	// Definiciones inciales
	if(argc != 8){
		cout<<"\nUsage: ./server port n_threads project_json_base sim_json_base output_base target_base results_base\n";
		cout<<"\nExample: ./bin/server 12345 4 ./data/project_json_ ./data/sim_json_ ./data/data_ ./data/target_ ./data/results_\n";
		cout<<"\n";
		return 0;
	}
	unsigned int port = atoi(argv[1]);
	unsigned int n_threads = atoi(argv[2]);
	string project_json_base = argv[3];
	string sim_json_base = argv[4];
	string output_base = argv[5];
	string target_base = argv[6];
	string results_base = argv[7];
	
	cout << "Server - Inicio (port: " << port << ", n_threads: " << n_threads << ", output_base: " << output_base << ", target_base: " << target_base << ", results_base: " << results_base << ")\n";
	
	// Preparacion de elementos compartidos
	cout << "Server - Iniciando WorkManager\n";
	WorkManager *manager = new WorkManager();
	cout << "Server - Iniciando Analyzer\n";
	Analyzer *analyzer = new Analyzer(manager, n_threads, 0.05, output_base, target_base, results_base);
	
	// Preparacion y lanzamiento de Threads procesadores (con sleep interno)
	// En esta version, los threads de procesamiento de lanzan detached
	cout << "Server - Iniciando Threads\n";
	unsigned int pids[n_threads];
	for(unsigned int i = 0; i < n_threads; ++i){
		pids[i] = i;
		thread( processing_thread, pids[i], output_base, manager, analyzer ).detach();
	}
	
	// Preparacion e inicio del sistema de comunicacion
	// Procesamiento de llamadas (de aqui en adelante queda escuchando como daemon)
	
	// Inicio de enlace del puerto
	cout << "Server - Creando Socket\n";
	int sock_servidor = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_servidor < 0){
		cout << "Server - Error al crear socket.\n";
		return 0;
	}
	// Datos para conexion de server
	struct sockaddr_in serv_addr;
	memset((char*)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	cout << "Server - Enlazando.\n";
	if( bind(sock_servidor, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ){
		cout << "Server - Error en bind.\n";
		return 0;
	}
	
	cout << "Server - Iniciando listen.\n";
	listen(sock_servidor, 100);
	// Datos para conexion de cliente
	int sock_cliente = -1;
	// Esto es innecesario en nuestro caso
//	struct sockaddr_in cli_addr;
//	socklen_t clilen = sizeof(cli_addr);
	// Inicio de proceso
	bool procesar = true;
	while( procesar ){
		
		cout << "-----\n";
		
		cout << "Server - Esperando clientes.\n";
		
		// En esta version uso esa opcion, le quito el socket al objeto local y creare otro en el thread.
//		ClientReception conexion(sock_servidor, (struct sockaddr *)&cli_addr, &clilen);
		ClientReception conexion(sock_servidor, NULL, NULL);
		cout << "Server - Cliente aceptado.\n";
		
		if( ! conexion.receiveRequest() ){
			cout << "Server - Error al recibir Request.\n";
			continue;
		}
		
		cout << "Server - request " << (unsigned int)conexion.getType() << "\n";
		switch( conexion.getType() ){
			case 0:
				cout << "Server - Request vacio, ignorando.\n";
				break;
			case 1:
				// INIT: Crear el target y preparar datos
				cout << "Server - Creando thread_init_project\n";
				sock_cliente = conexion.getSocket();
				conexion.setSocket(-1);
				thread( thread_init_project, sock_cliente, project_json_base, target_base, manager ).detach();
				break;
			case 2:
				cout << "Server - DELETE PROJECT\n";
				break;
			case 3:
				cout << "Server - QUERY PROJECT\n";
				break;
			case 4:
				// START: Agregar trabajo a work_queue e iniciar simulaciones
				cout << "Server - Creando thread_start_sim\n";
				sock_cliente = conexion.getSocket();
				conexion.setSocket(-1);
				thread( thread_start_sim, sock_cliente, sim_json_base, project_json_base, manager ).detach();
				break;
			case 5:
				cout << "Server - DELETE SIMULATION\n";
				break;
			case 6:
				cout << "Server - QUERY SIMULATION\n";
				sock_cliente = conexion.getSocket();
				conexion.setSocket(-1);
				thread( thread_query_sim, sock_cliente, sim_json_base, project_json_base, manager, analyzer ).detach();
				break;
				
//			case KILL_SERVER:
//				cout << "Server - Cerrando Server.\n";
//				close( sock_servidor );
//				procesar = false;
//				break;
			default:
				cout << "Server - Handler NO definido, ignorando.\n";
				break;
		}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	}
	
	
	
	
}













