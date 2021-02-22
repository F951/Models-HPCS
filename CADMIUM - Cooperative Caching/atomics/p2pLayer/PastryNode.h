#ifndef _PASTRYNODE_h
#define _PASTRYNODE_h
#include "../glob.h"


// Class to be implemented by the nodes of the upperLayer
class PastryNode : public process {

public:

   BIGNUM* nid;
   bool busy;
   list<void*> queue_in_lower;

  ~PastryNode()
  {
  }
 
   PastryNode( const string& name, BIGNUM *_nid ): process( name )
   {
      nid=_nid;
      busy=false;
   }

   // Deliver sends the message m to the upperLayer through 
   // the list queue_in_lower 
   void deliver( void *m )
   {
      //cout<<"PastryNode: Deliver llega "<<endl;
      queue_in_lower.push_back(m);
   }

   virtual void print(void){}

   // To be implemented by the upperLayer node 
   // receives a message 
   virtual void add_message(PastryMessage*){}

   // To be implemented by the upperLayer node
   // returns the busy state of the node
   virtual bool get_busy(void)
   {
      return busy;
   }

protected:
   // Process function
   void inner_body(void){}

};

#endif
