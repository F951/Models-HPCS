#ifndef _LCACHE_H
#define _LCACHE_H

#include "../glob.h"
#include "../p2pLayer/NodeEntry.h"
#include "Amortiguador.h"


class LCache 
{

public:

   string key;
   BIGNUM* hashValue;

   int size;

   NodeEntry* locationID;
   NodeEntry* nextStepID;
   int sequential;
   long priority;
   Amortiguador *amortiguador;


  ~LCache()
  {
      BN_free(hashValue);
  }


   LCache(string _key, BIGNUM* _hashValue, 
          NodeEntry* loc, NodeEntry * _ns , 
	  long int timestamps)
   {
      locationID = loc;
      nextStepID = _ns;
      key = _key;
      hashValue = BN_dup( _hashValue);
      priority = timestamps;
      sequential = MAX_SEQUENTIAL;
      size=LCACHE_SIZE;
   }

   // update the peer that stores locally the object
   void addNewLocation(NodeEntry* _lID, long timestamps)
   {
      locationID = _lID;
      sequential = sequential -1;
      priority = timestamps;
   }


   // gets the last peer that queried the object
   NodeEntry* getLocation()
   {
      return locationID;
   }

   // gets the next peer in the path
   NodeEntry* getNextStep()
   {
      return nextStepID;
   }

   void CreateAmortiguador( double Taho )
   {
      amortiguador = new Amortiguador(Taho);
   }

   NodeEntry* getDonante( NodeEntry *e, double R)
   {  
      return amortiguador->getDonante(e,R);
   }
   // helps when sending this entry to another peer
   LCache * clone()
   {
      LCache *tmp = new LCache(this->key,
                               this->hashValue,
			       this->locationID, 
			       this->nextStepID,
			       this->priority
			       );
      tmp->sequential = this->sequential;
      tmp->amortiguador = this->amortiguador;
      return tmp;

   }
  
   void update(long int timestamps)
   {
      priority = timestamps;
   }
   

};

#endif
