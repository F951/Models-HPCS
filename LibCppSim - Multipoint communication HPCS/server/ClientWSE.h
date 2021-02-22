#ifndef _CLIENTWSE_H
#define _CLIENTWSE_H

#include "../glob.h"

// has to be implemented by the peer to contact the WSE 

class Message;

class ClientWSE : public process


{

public:

int id_peer_client;
   bool busy;
   list<Message*> queue_in_wse;

   ClientWSE ( const string& name ): process (name)
   {
//      cout << "ClientWSE: INIT" << endl;
      busy=false;
      //~ id_peer_client= _id_peer_client;
   }

// puts a message in the client queue
   void receiveWSE(Message *m)
   {
	queue_in_wse.push_back(m);
   }


   virtual bool get_busy(void)
   { 
   	return busy;
   }

protected:
   void inner_body(void)
   {
   }


};

#endif
