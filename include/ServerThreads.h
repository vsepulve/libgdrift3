#ifndef _SERVER_THREADS_H
#define _SERVER_THREADS_H

#include <thread>
#include <mutex>

#include <iostream>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>

#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime

#include "CommunicationUtils.h"
#include "ClientReception.h"
#include "WorkManager.h"
#include "Analyzer.h"

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"
#include "Statistics.h"

#define MAX_READ 1000000

using namespace std;

// Thread permanente de procesamiento
void processing_thread(unsigned int pid, string output_base, WorkManager *manager, Analyzer *analyzer);

// INIT: Crear el target y preparar datos
// Este proceso tambien recibe y guarda el json del proyecto (ademas del target)
void thread_init_sim(int sock_cliente, string json_file_base, WorkManager *manager);

// START: Agregar trabajo a work_queue e iniciar simulaciones
void thread_start_sim(int sock_cliente, string json_file_base, WorkManager *manager);














#endif //_SERVER_THREADS_H
