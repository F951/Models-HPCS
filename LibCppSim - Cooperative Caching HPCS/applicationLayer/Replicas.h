#ifndef _REPLICAS_H
#define _REPLICAS_H
#include "../glob.h"

// TODO

class Replicas
{
public:
   string value;
   BIGNUM* hashValue;

   BIGNUM* responsable;
   list<BIGNUM*> replicas;

  // Replicas(string q, BIGNUM* b, BIGNUM* r)
   Replicas(BIGNUM* b, BIGNUM* r)
   {
      //value =q;
      responsable = r;
      replicas.push_back(b);
   }

   ~Replicas()
   {
     BN_free(responsable);
   }
   
   void putReplica(BIGNUM* id)
   {
      if (!isContained(id))
         replicas.push_back(id);
   }


   bool isContained(BIGNUM* id)
   {
      list<BIGNUM*>::iterator it;
      it = replicas.begin();

      while(it != replicas.end())
      {
         if(BN_cmp(*it, id) ==0 )
	    return true;

	 ++it;
      }

      return false;
   }

   BIGNUM* getResponsable()
   {
      return responsable;
   }

  void print()
  {
    cout << "-------------"<< BN_bn2hex(responsable) << "----------" << endl;
    list<BIGNUM*>::iterator it;
    it = replicas.begin();
    
    while(it != replicas.end())
    { 
     cout << BN_bn2hex(*it) <<endl;
     ++it;
    } 
  }
};


#endif
