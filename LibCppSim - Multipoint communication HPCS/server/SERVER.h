#ifndef SERVER_H
#define SERVER_H

#include "../glob.h"
#include "Message.h"
#include "../client/CLIENT.h"
#include "../network/NETWORK.h"
#include "../Observer.h"

class NETWORK;



class SERVER: public process
{

private:
  
   list<Message*> queue_in;

   bool busy;
   	vector<handle<CLIENT> *> clientes_Peers;
	handle<NETWORK> *network;
	handle<Observer>* obs;
	int nodes;
	int ID_numb;
	int totalQueries;
	
public:
	int param_server;
	
	~SERVER()
	{
		
	}

	SERVER ( const string& name,int _nodes, handle<Observer>* _obs , int _ID_numb, int _totalQueries): process ( name )
	{
		busy=false;
		nodes = _nodes;
		obs = _obs;
		ID_numb = _ID_numb;
		totalQueries=_totalQueries;
	}

	void inner_body ( void );

	void add_request ( Message* bn )
	{
		queue_in.push_back(bn);
	}
	void set_handle_network ( handle<NETWORK>* _network )
	{
		network = _network;
	}

	void add_peer(handle<CLIENT> * p)
	{
		clientes_Peers.push_back(p);
	}

	bool get_busy()
	{
		return busy;
	}

	void print()
	{
		cout << "SERVER" << endl;
	}

	long int getTTL(BIGNUM*);
	int getVersion(string, BIGNUM*);
	long int randomTTL();
	
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
