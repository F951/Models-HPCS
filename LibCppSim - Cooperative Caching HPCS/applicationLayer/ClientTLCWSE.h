#ifndef _CLIENTTLCWSE_H
#define _CLIENTTLCWSE_H

#include "../glob.h"
#include "../wse/ClientWSE.h"
#include "../wse/WSE.h"
#include "../wse/MessageWSE.h"
#include "Query.h"
#include "Entry.h"
#include "TlcMessage.h"

class TlcProtocol;

class ClientTlcWse: public ClientWSE
{
public:

//   list<MessageWSE *> queue_in;
   handle<WSE> *wse;
   handle<ClientWSE> *I_obj;
   handle<TlcProtocol> *tlc;


   ~ClientTlcWse ( )
   {
      //free (wse);
      //free(tlc);
   }

   ClientTlcWse ( const string& name, handle<WSE> *_wse )
	            : ClientWSE ( name )
   {
      wse=_wse;
	
   }

   void inner_body(void);

   void set_tlc(handle<TlcProtocol> *t);

   /*Implemented to make the connection with WSE */
   void set_ClientWSE_obj(handle<ClientTlcWse> *t)
   {
      I_obj = (handle<ClientWSE>*)t;
   }

   handle<ClientWSE>* get_ClientWSE_obj()
   {
      return I_obj;
   }

   void sendWSEQuery(Query *q);

   long int getTTL(BIGNUM *);
   long int getTimeIncremental(long int);

};

#endif
