#include "ClientReception.h"

atomic<unsigned int> ClientReception::next_id(1);

ClientReception::ClientReception(){
	sock_fd = -1;
	request_type = 0;
	instance_id = next_id++;
}

ClientReception::ClientReception(int listen_socket, struct sockaddr *addr, socklen_t *addrlen){
	sock_fd = accept(listen_socket, addr, addrlen);
	instance_id = next_id++;
}

ClientReception::ClientReception(const ClientReception &original){
	sock_fd = original.sock_fd;
	request_type = original.request_type;
	instance_id = next_id++;
	cout << "ClientReception - Copia (fd "<<original.sock_fd<<", id "<<instance_id<<")\n";
}

ClientReception::~ClientReception(){
	cout << "ClientReception::~ClientReception (id "<<instance_id<<")\n";
	//Cerrar socket
	if( sock_fd != -1 ){
		cout << "ClientReception::~ClientReception - cerrando fd "<<sock_fd<<", id "<<instance_id<<"\n";
		close(sock_fd);
	}
}

bool ClientReception::receiveRequest(){
	cout << "ClientReception::receiveRequest (id "<<instance_id<<")\n";
	if( ! readByte(request_type) ){
		cerr << "ClientReception::receiveRequest - Error receiving Request\n";
		request_type = 0;
		return false;
	}
	return true;
}




