#ifndef _REPLICADOR_H
#define _REPLICADOR_H

#include "../glob.h"
#include "Entry.h"
#include "TlcProtocol.h"

class TlcProtocol;

class Replicator: public process {

vector <Entry*> entries;

private:

public:
   bool ends;
   int overload;
   int Rep_Strategy;

   static bool compare(Entry *e1 , Entry *e2)
   {
      if(e1->frequency > e2->frequency)
         return true;
      else
         return false;
   }
				  
   struct sorter
   {
      BIGNUM* key;
      sorter(BIGNUM* k) : key(k){}
      bool operator() (Inlink *i1, Inlink *i2)
      {
         if(i1->getFrequency(key) > i2 ->getFrequency(key))
            return true;
         else
            return false;
      }
   };

   handle <TlcProtocol>* tlc;

   ~Replicator()
   {
      //free(tlc);
      entries.clear();
   }

   Replicator(const string& name, int _RS):process(name)
   {
      ends = false;
      Rep_Strategy=_RS;
   }

   void set_tlc(handle<TlcProtocol> *t);
   void inner_body(void);
   bool isOverloaded(handle<TlcProtocol>*);
   void retrieveEntries(handle<TlcProtocol>* );
   double getQueueSize( handle <TlcProtocol>*);
   void replicate(int);
   void replicateLeafset();
   void replicateBubble();
   void replicateTLC();
   int getAvailableCapacity(handle <TlcProtocol>*); 
   bool isReplicated(BIGNUM*, BIGNUM*);

   void end()
   {
      ends = true;
   }	

};

#endif
