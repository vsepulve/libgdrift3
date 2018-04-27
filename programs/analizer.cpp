
#include <iostream>
#include <random>

#include <thread>
#include <mutex>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <vector>
#include <list>

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"
#include "Statistics.h"

#include "CommunicationUtils.h"
#include "ServerThreads.h"
#include "ClientReception.h"

using namespace std;

int main(int argc,char** argv){

	if(argc != 3){
		cout<<"\nUsage: ./analyzer json_base port\n";
		cout<<"\n";
		return 0;
	}
	
	string json_base = argv[1];
	unsigned int port = atoi(argv[2]);
	
	
	
	// Inicio de enlace del puerto
	cout << "Server::main - Enlazando puerto " << port << "\n";
	cout << "Server::main - Creando Socket\n";
	int sock_servidor = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_servidor < 0){
		cout << "Server::main - Error al crear socket.\n";
		return 0;
	}
	// Datos para conexion de server
	struct sockaddr_in serv_addr;
	memset((char*)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	cout << "Server::main - Enlazando.\n";
	if( bind(sock_servidor, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ){
		cout << "Server::main - Error en bind.\n";
		return 0;
	}
	
	cout << "Server::main - Iniciando listen.\n";
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
		
		cout << "Server::main - Esperando clientes.\n";
		// La idea es convertir el accept en la construccion de un objeto adecuado para la comunicacion
		// Este objeto, sin embargo, NO puede ser pasado al thread y al mismo tiempo encargarse del cerrado del socket
		// Esto es porque al iniciarse el thread, se realiza MAS de una copia.
		// Las dos opciones son usar un puntero o pasar los valores para que el thread construya un nuevo objeto de conexion propio
		// En esta version uso esa opcion, le quito el socket al objeto local y creare otro en el thread.
//		ClientReception conexion(sock_servidor, (struct sockaddr *)&cli_addr, &clilen);
		ClientReception conexion(sock_servidor, NULL, NULL);
		cout << "Server::main - Cliente aceptado.\n";
		
		if( ! conexion.receiveRequest() ){
			cout << "Server::main - Error al recibir Request.\n";
			continue;
		}
		
		// Recepcion de un mensaje de prueba: string en el formato length (4 bytes) + chars (length bytes)
		// Notar que esto se puede recibir con readString, lo leo por separado para mayor control
//		unsigned int len = 0;
//		conexion.readUInt(len);
//		cout << "Server::main - len: " << len << "\n";
//		char buff[len + 1];
//		conexion.readData(buff, len);
//		buff[len] = 0;
//		cout << "Server::main - buff: \"" << buff << "\"\n";
//		
//		// Envio de un mensaje de prueba: un entero (4 bytes) con el valor "1"
//		cout << "Server::main - Enviando respuesta (1)\n";
//		conexion.writeUInt(1);
		
	
		cout << "Server::main - request "<<(unsigned int)conexion.getType()<<"\n";
		switch( conexion.getType() ){
			case 0:
				cout << "Server::main - Request vacio, ignorando.\n";
				break;
			case 1:
				cout << "Server::main - Creando thread_init\n";
				sock_cliente = conexion.getSocket();
				conexion.setSocket(-1);
//				thread( thread_analyzer_init, sock_cliente, &config ).detach();
				thread( thread_analyzer_init, sock_cliente, json_base ).detach();
				break;
				
				
				
				
//			case KILL_SERVER:
//				cout << "Server::main - Cerrando Server.\n";
//				close( sock_servidor );
//				procesar = false;
//				break;
			default:
				cout << "Server::main - Handler NO definido, ignorando.\n";
				break;
		}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	}
	
	
}













