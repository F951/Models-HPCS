#ifndef _CLIENTWSE_H
#define _CLIENTWSE_H

#include "../glob.h"

// has to be implemented by the peer to contact the WSE 

class TlcProtocol;

class MessageWSE;

class ClientWSE : public process

{

public:

   bool busy;
   list<MessageWSE*> queue_in_wse;
	int id_peer_client;
	//Handle agregado para la simulación de desastres naturales. Para que el Peer conozca las capas inferiores.
	handle<TlcProtocol> *handle_nodo_tlc;
	
	
   ClientWSE ( const string& name ): process (name)
   {
//      cout << "ClientWSE: INIT" << endl;
      busy=false;
   }

// puts a message in the client queue
   void receiveWSE(MessageWSE *m)
   {
		queue_in_wse.push_back(m);
   }


   virtual bool get_busy(void)
   { 
   	return busy;
   }

	void set_tlc_of_client(handle<TlcProtocol> *t)
	{
		handle_nodo_tlc=(handle<TlcProtocol>*)t;
	}
	
	handle<TlcProtocol> * get_tlc_of_client()
	{
		return handle_nodo_tlc;
	}

protected:
   void inner_body(void)
   {
   }

	
	
};

#endif
