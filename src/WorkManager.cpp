#include "WorkManager.h"

WorkManager::WorkManager(){
	killed = false;
}

WorkManager::~WorkManager(){
}

void WorkManager::setKilled(){
	lock_guard<mutex> lock(inner_mutex);
	killed = true;
}

bool WorkManager::isKilled(){
	lock_guard<mutex> lock(inner_mutex);
	return killed;
}

// Este metodo interno NO esta protegido por el mutex
// Eso es porque puede ser llamado por cualquier otro metodo de la clase
// Asume que el factory de sim_id esta listo
void WorkManager::addBatch(unsigned int sim_id){
	SimulatorFactory *factory = status_table.getFactory(sim_id);
	unsigned int n_sims = status_table.getTotal(sim_id);
	unsigned int old_size = work_queue.size();
	if( old_size == 0 ){
		// Llenado inicial
		cout << "WorkManager::addBatch - Llenado directo, Cola de trabajo vacia\n";
		for( unsigned int i = 0; i < n_sims; ++i ){
			work_queue.push_back(factory->getInstanceSerialized());
		}
	}
	else if( old_size >= n_sims ){
		// Llenado disperso
		unsigned int s = (unsigned int)(old_size / n_sims);
		auto it = work_queue.begin();
//		unsigned int pos = 0;
		cout << "WorkManager::addBatch - Llenado disperso (old_size: " << old_size << ", n_sims: " << n_sims << ", s: " << s << ")\n";
		for( unsigned int i = 0; i < n_sims; ++i ){
//			cout << "addBatch - Insert en pos " << pos << "\n";
			it = work_queue.insert(it, factory->getInstanceSerialized());
			for( unsigned int j = 0; j < s+1; ++j ){
				it++;
//				++pos;
			}
		}
	}
	else{
		// Llenado denso
		unsigned int s = (unsigned int)(n_sims / old_size);
		unsigned int e = n_sims - old_size * s;
		auto it = work_queue.begin();
//		unsigned int pos = 0;
		cout << "WorkManager::addBatch - Llenado denso (old_size: " << old_size << ", n_sims: " << n_sims << ", s: " << s << ", e: " << e << ")\n";
		for( unsigned int i = 0; i < old_size; ++i ){
			unsigned int limit = (i<e)?(s+1):(s);
			for( unsigned int j = 0; j < limit; ++j ){
//				cout << "addBatch - Insert en pos " << pos << "\n";
				it = work_queue.insert(it, factory->getInstanceSerialized());
				it++;
//				++pos;
			}
			it++;
//			++pos;
		}
	}
	cout << "WorkManager::addBatch - Fin (new size: " << work_queue.size() << ")\n";
}
	
// Prepara un nuevo SimulatorFactory con json_file, y lo asocia a sim_id en la tabla
// Agrega n_sims Simulators del factory de sim_id a la cola
void WorkManager::addWork(unsigned int sim_id, string &json_file, unsigned int n_sims){
	cout << "WorkManager::addWork - Inicio (" << n_sims << " sims desde \"" << json_file << "\")\n";
	
	SimulatorFactory *factory = new SimulatorFactory(json_file);
	
	lock_guard<mutex> lock(inner_mutex);
	status_table.addSimulation(sim_id, factory, n_sims);
	addBatch(sim_id);
	status_table.resetFinished(sim_id);

	cout << "WorkManager::addWork - Fin (new size: " << work_queue.size() << ")\n";
}

void WorkManager::addTraining(unsigned int sim_id, vector<pair<double, double>> &dist_post){
	cout << "WorkManager::addTraining - Inicio\n";
	
	lock_guard<mutex> lock(inner_mutex);
	status_table.getFactory(sim_id)->reloadParameters(dist_post);
	addBatch(sim_id);
	status_table.resetFinished(sim_id);

	cout << "WorkManager::addTraining - Fin (new size: " << work_queue.size() << ")\n";
}

// Retorna el proximo trabajo de la cola (y lo saca de la cola)
// Retorna NULL si la cola esta vacia
// NOTAR: NO la marca como entregada o terminada, espera el termino efectivo
char *WorkManager::getWork(){
	char *work = NULL;
	lock_guard<mutex> lock(inner_mutex);
	if( ! work_queue.empty() ){
		work = work_queue.front();
		work_queue.pop_front();
	}
	return work;
}

// Aumenta el contador de simulaciones terminadas de sim_id
void WorkManager::addFinished(unsigned int sim_id){
	lock_guard<mutex> lock(inner_mutex);
	status_table.addFinished(sim_id);
}

// Retorna el total esperado de simulaciones de sim_id (batch size)
unsigned int WorkManager::getTotal(unsigned int sim_id){
	unsigned int res = 0;
	lock_guard<mutex> lock(inner_mutex);
	res = status_table.getTotal(sim_id);
	return res;
}

// Retorna el numero de simulaciones MARCADAS como terminadas
unsigned int WorkManager::getFinished(unsigned int sim_id){
	unsigned int res = 0;
	lock_guard<mutex> lock(inner_mutex);
	res = status_table.getFinished(sim_id);
	return res;
}

// Transfiere el calculo de n_stats directo del factory de sim_id
unsigned int WorkManager::getNumStatistics(unsigned int sim_id){
	unsigned int res = 0;
	lock_guard<mutex> lock(inner_mutex);
	res = status_table.getFactory(sim_id)->getNumStatistics();
	return res;
}

// Transfiere el calculo de n_params directo del factory de sim_id
unsigned int WorkManager::getNumParams(unsigned int sim_id){
	unsigned int res = 0;
	lock_guard<mutex> lock(inner_mutex);
	res = status_table.getFactory(sim_id)->getNumParams();
	return res;
}

// Transfiere la extraccion de nombres de parametros directo del factory de sim_id
vector<string> &WorkManager::getParams(unsigned int sim_id){
	lock_guard<mutex> lock(inner_mutex);
	return status_table.getFactory(sim_id)->getParams();
}

// Feedback actual de la simulacion (controlado por addFeedback)
unsigned int WorkManager::getFeedback(unsigned int sim_id){
	unsigned int res = 0;
	lock_guard<mutex> lock(inner_mutex);
	res = status_table.getFeedback(sim_id);
	return res;
}

// Incrementa el feedback de la simulacion en uno
void WorkManager::addFeedback(unsigned int sim_id){
	lock_guard<mutex> lock(inner_mutex);
	status_table.addFeedback(sim_id);
}
	
	
	
	
	


