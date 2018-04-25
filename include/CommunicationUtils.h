#ifndef _COMMUNICATION_UTILS_H
#define _COMMUNICATION_UTILS_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

// Funciones basicas de comunicacion de bajo nivel

// Escribe chars de buff hasta escribir size o fallar
// Retoran el numero de bytes escritos
unsigned int writeBytes(int sock_fd, const char *buff, unsigned int size);

// Lee chars hasta completar size (o fallar), escribiendolos en buff
// Asume que buff tiene al menos size bytes
// Notar que este metodo NO agrega el '\0' final para un c-string valido
// Retorna el numero de bytes leidos
unsigned int readBytes(int sock_fd, char *buff, unsigned int size);

// Envia un c-string
// Primero envia el largo (strlen) en 4 bytes, luego los chars
// Retorna true en exito
bool writeString(int sock_fd, const char *buff);

// Lee un c-string
// Primero lee el largo en 4 bytes, luego los chars (y los escribe en buff)
// ASUME que el buff tiene espacio suficiente (notar que esta es una llamada insegura)
// SOLO DEBE SER USADA SI SE CONOCE A PRIORI EL LARGO MAXIMO para entregarle un buffer adecuado
// Retorna true en exito
bool readStringSimple(int sock_fd, char *buff, unsigned int buff_size);

// Lee un c-string
// Primero lee el largo en 4 bytes, luego los chars
// Escribe los chars leidos en buff, hasta un maximo de (buff_size - 1)
// Finalmente agrega el '\0' final
// Notar que esta llamada lee pero omite chars por sobre (buff_size - 1), trunca el string
// Retorna true en exito
bool readStringSimple(int sock_fd, char *buff, unsigned int buff_size, char *inner_buff, unsigned int inner_buff_size);








#endif //_COMMUNICATION_UTILS_H
