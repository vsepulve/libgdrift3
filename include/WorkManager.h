#ifndef _WORK_MANAGER_H_
#define _WORK_MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <thread>
#include <mutex>

#include <vector>
#include <list>
#include <map>

#include "SimulatorFactory.h"
#include "Simulator.h"
#include "StatusTable.h"
#include "WorkQueue.h"

using namespace std;

class WorkManager{

protected:

private:
	StatusTable status_table;
	
//	WorkQueue work_queue;
	// Por ahora implemento la WorkQueue localmente
	// NOTAR: Si resulta muy verboso o complejo, se puede encapsular en WorkQueue
	list<char*> work_queue;
	
	// Para mantener el acceso exclusivo a las estructuras, mantengo un mutex
	// NOTAR: Podria ser conveniente usar 2 independientes (uno para la tabla y otro para la lista)
	mutex inner_mutex;

public:

	WorkManager();
	virtual ~WorkManager();
	
	// Prepara un nuevo SimulatorFactory con json_file, y lo asocia a sim_id en la tabla
	// Agrega n_sims Simulators del factory de sim_id a la cola
	void addWork(unsigned int sim_id, string &json_file, unsigned int n_sims);
	
	// Retorna el proximo trabajo de la cola (y lo saca de la cola)
	// Retorna NULL si la cola esta vacia
	// NOTAR: NO la marca como entregada o terminada, espera el termino efectivo
	char *getWork();
	
	// Aumenta el contador de simulaciones terminadas de sim_id
	void addFinished(unsigned int sim_id);
	
	// Retorna el total esperado de simulaciones de sim_id (batch size)
	unsigned int getTotal(unsigned int sim_id);
	
	// Retorna el numero de simulaciones MARCADAS como terminadas
	unsigned int getFinished(unsigned int sim_id);
	
	
	
	
};

#endif
