#ifndef _CLIENT_H
#define _CLIENT_H

#include "../glob.h"
#include "../server/ClientWSE.h"
#include "../server/SERVER.h"
#include "../server/Message.h"
#include "../network/NETWORK.h"

class SERVER;
class NETWORK;

class CLIENT: public ClientWSE
{
public:

	handle<SERVER> *server;
	handle<ClientWSE> *I_obj;
	handle<NETWORK> * network;
	int param_peer;
	int ID_numb;
	int nodes;
	int totalQueries;

   ~CLIENT ( )
   {

   }

   CLIENT ( const string& name, int _ID_numb, handle<NETWORK> *_network , int _nodes, int _totalQueries)
	            : ClientWSE ( name )
   {
      network=_network;
      ID_numb=_ID_numb; 
      nodes=_nodes;
      totalQueries = _totalQueries;
	
   }

   void inner_body(void);

	void set_ClientWSE_obj(handle<CLIENT> *t)   {
		I_obj = (handle<ClientWSE>*)t;
	}

   handle<ClientWSE>* get_ClientWSE_obj(){
      return I_obj;
   }


   long int getTTL(BIGNUM *);
   long int getTimeIncremental(long int);
   
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
