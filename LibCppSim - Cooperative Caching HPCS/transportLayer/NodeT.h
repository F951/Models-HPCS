#ifndef _NODET_h
#define _NODET_h
#include "../glob.h"

/* NodeT: class to be implemented by the upperNodeLayer 
 * */

class NodeT : public process 
{

protected: 

   int nid; // index of the peer
   bool busy;
   list<void*> queue_in_lower;
 
public:

   ~NodeT()
   {

   }

   NodeT(const string& name,int _nid):process(name)
   {
      //cout << "NodeT: INIT Node :" << _nid << endl;
      nid=_nid;
      busy=false;
   }

   // returns the index of the peer
   int getIP()
   {
      return nid;
   }

   /* deliver: puts a message in the queue of the upperLayer */
   void deliver(void *m)
   {
      //cout<<"transport: Entrega mje NodeT "<<endl; 
      queue_in_lower.push_back(m);
   }

   /* add_message add a message in the queue */
   virtual void add_message(MessageT*) 
   {
   };

   /* returns the busy state of the node */
   virtual bool get_busy(void)
   {
      return busy;
   }

protected:
   /* process function */
   void inner_body(void)
   {
   }

};

#endif
