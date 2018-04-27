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

#include "NanoTimer.h"
#include "SimulatorFactory.h"
#include "Simulator.h"
#include "Statistics.h"

using namespace std;



//void thread_stat(int sock_cliente, Configuration *config);
void thread_analyzer_init(int sock_cliente, string json_file_base);

void simultion_thread(unsigned int pid, string output_base, WorkManager *manager);



#endif //_SERVER_THREADS_H
