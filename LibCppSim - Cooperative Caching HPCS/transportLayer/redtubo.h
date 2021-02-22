#ifndef REDTUBO_H
#define REDTUBO_H

#include "../glob.h"
#include "MessageT.h"
#include "NodeT.h"

/** Main class of the transport Layer **/

class RedTubo: public process
{  


protected:

   list<MessageT *> queue_in; 
   
   /* nodes in the upperLayer */
   vector <handle<NodeT>* > nodes;   
   bool busy;
  
   /* process function */
   void inner_body( void );  

public:

   ~RedTubo()
   {
      nodes.clear();
   }

   RedTubo ( const string& name ): process( name )
   {
      busy = false;    
   }
   
   /* adds a Peer in the upperLayer */
   void add_Node( handle<NodeT> *n )
   {
      nodes.push_back(n);
   }

   /* returns the busy state of the redTubo */
   bool get_busy()
   {
      return busy;
   }

   /* adds a MessageT in the queue */
   void add_message( MessageT *m )
   {
      //cout << "Redtubo: inTubo "<< m->getDest() << endl;
      queue_in.push_back( m );
   }
   
   void print()
   {
      cout << "REDTUBO" << endl;
   }

};

#endif
