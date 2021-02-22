#ifndef WSE_H
#define WSE_H

#include "../glob.h"
#include "MessageWSE.h"
#include "LruA.h"

//#include "../auxiliar/Util.h"
//#include "../auxiliar/Hash.h"
//#include "../Observer.h"


//#define WSECACHESIZE 8000

class WSE: public process
{

private:
  
   // ID_PAGE , (TTL, VERSION)
//   map<BIGNUM*, pair<long int,int> > answers;
   
   list<MessageWSE*> queue_in;

   bool busy;
   LRUA *ANSWERS;

	

public:

   ~WSE()
   {
    //  answers.clear();
      delete ANSWERS;
   }

   WSE ( const string& name ): process ( name )
   {
      busy=false;
      int cacheSize = WSECACHESIZE; 
      ANSWERS = new LRUA(&cacheSize); 
   }

   void inner_body ( void );

   void add_request ( MessageWSE* bn )
   {
      queue_in.push_back(bn);
   }

   bool get_busy()
   {
      return busy;
   }

   void print()
   {
      cout << "WSE" << endl;
   }

   long int getTTL(BIGNUM*);
   //int getVersion(BIGNUM*);
   int getVersion(string, BIGNUM*);
   long int randomTTL();
};


#endif 
