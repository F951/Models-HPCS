#ifndef _Query_h
#define _Query_h

#include "../glob.h"
#include "../p2pLayer/NodeEntry.h"
class Query
{
public:

   char* term;
   BIGNUM* hashValue;
   int id;
   int rte;
   double t_start;
   int hops;
   NodeEntry* src;

   long int timeOut;
   bool fc;

   long int getTimeOut()
   {
      return timeOut;
   }

   void renewTimeOut(long int thistime)
   {
      timeOut = thistime + TIMEOUT_Q;
   }

   void forceRouting()
   {
      fc=true;
   }
   bool getForceRouting()
   {
      return fc;
   }

   Query(int _id, char *ptr, BIGNUM* _hashValue,  double _t_start)
   {
      term=(char*)malloc(strlen(ptr)+1);
      strcpy(term,ptr);
      term[strlen(ptr)]='\0';
     // term=ptr;
      id=_id;
      t_start = _t_start;
      hops=0;
      hashValue=_hashValue;
    
      timeOut = _t_start + TIMEOUT_Q;
      fc=false;
//		cout << "Query "<< id << endl;
   }
   
   void setQuerySrc(NodeEntry* _ne)
   {
      src=_ne;
   }
}; 
#endif
