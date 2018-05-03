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

// NOTE: De aqui en adelante estoy asumiendo que cada simulacion tiene solo un escenario
// Esto es para identificar el escenario con todas sus caracteristicas por sim_id
// Obviamente eso implica que varios escenarios seran tratados como simulaciones diferentes

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
	
	// Flag de estado general del sistema (se setea true para terminar)
	bool killed;

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
	
	// Mensaje de termino para los threads procesadores
	bool isKilled();
	
	void setKilled();
	
	// Transfiere el calculo de n_stats directo del factory de sim_id
	unsigned int getNumStatistics(unsigned int sim_id);
	
	// Transfiere el calculo de n_params directo del factory de sim_id
	unsigned int getNumParams(unsigned int sim_id);
	
	// Transfiere la extraccion de nombres de parametros directo del factory de sim_id
	vector<string> &getParams(unsigned int sim_id);
	
	// Feedback actual de la simulacion (controlado por addFeedback)
	unsigned int getFeedback(unsigned int sim_id);
	
	// Incrementa el feedback de la simulacion en uno
	void addFeedback(unsigned int sim_id);
	
	
	
};

#endif
