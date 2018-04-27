#include "ServerThreads.h"


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


