#ifndef NETWORK_H
#define NETWORK_H

#include "../glob.h"
#include "../Observer.h"
#include "../server/Message.h"
#include "../server/SERVER.h"

class CLIENT;
class Observer;
class SERVER;

class NETWORK: public process
{  

protected:

	list<Message *> queue_in; 

	handle<Observer> *observer;

	bool busy;

	void inner_body( void );  
	int flag_terminar;
	handle<Observer>* obs;
    
public:
    int param_network;

public:

	vector<handle<CLIENT> *> clientes_server_Peers;
	int NP;
	vector<handle<SERVER> *> server;
	int nro_servers;
	
	~NETWORK() 	{
		
	}

	NETWORK ( const string& name,  int _NP ,int _nro_servers, handle<Observer>* _obs): process( name ) {
	  busy = false;  
	  NP  = _NP;  
	  flag_terminar=0;
	  nro_servers = _nro_servers;
	  obs=_obs;
	}

	void add_peer(handle<CLIENT> * p) {
		clientes_server_Peers.push_back(p);
	}

	bool get_busy() 	{
		return busy;
	}

	void add_message( Message *m ) {
		queue_in.push_back( m );
	}
	void add_server( handle<SERVER> *_server ) {
		server.push_back( _server );
	}
	


	void print()
	{
	  cout << "NETWORK" << endl;
	}

	void set_handle_obs(handle<Observer> *_obs)
	{
		observer=_obs;
	}
	
	void printMSG(Message *m);
	
	void terminar(  ){
		flag_terminar=1;	
	}
	
	double get_random_time_exp(double par, double multiplicador){
		//------------------------------------------------------------------------------
		int seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator (seed);
		std::exponential_distribution<double> distribution(par);
		double delay = multiplicador*distribution(generator);
		//------------------------------------------------------------------------------
		return delay;
	}
	
};



#endif
