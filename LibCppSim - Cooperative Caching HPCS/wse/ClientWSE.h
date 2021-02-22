#ifndef _CLIENTWSE_H
#define _CLIENTWSE_H

#include "../glob.h"

// has to be implemented by the peer to contact the WSE 

class MessageWSE;

class ClientWSE : public process


{

public:

   bool busy;
   list<MessageWSE*> queue_in_wse;

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

protected:
   void inner_body(void)
   {
   }


};

#endif
