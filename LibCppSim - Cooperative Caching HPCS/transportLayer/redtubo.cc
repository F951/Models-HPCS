#include "redtubo.h"

void RedTubo::inner_body( ){

   while(1) {

      if ( queue_in.empty() )
         passivate();

//cout<<"transportLayer/redtubo.cc" <<endl; fflush(stdout);
      ASSERT( !queue_in.empty() );

       
      MessageT* m = queue_in.front();
      queue_in.pop_front();

      // Simulates sending a message to other peer
       


		//~ cout << "LibCppSim - transport: " <<  m->getSrc() << " -> " << m->getDest() << endl;
      
      switch (m->getType())
      {

         case T_BDC:
     // if((m->getDest())==0 &&( m->getSrc())==0)
         {
 //            cout << "transport: Broadcasting "<<  nodes.size() << endl;
             for(unsigned int i=0; i<nodes.size(); i++)
             {

//                 cout << "1 transport: Broadcasting " <<i  << endl;
                 (*nodes[i])->deliver(m->clone());
	       
                 if ((*nodes[i])->idle() )//&& !(*nodes[i])->get_busy())
	         {  
//	             cout<<"2 transport: Bdc Despierta "<<i<<endl;     
                     (*nodes[i])->activateAfter(current());
                 }		   
              }
	      delete m;
          
	      break;
	  }
     // } else {

          case T_P2P:
          {
				busy = true;
				hold(0.00001);
				busy = false; 
            /* Destination comes within the MessageT */
              int dest = m->getDest(); 
 
      //cout << "Redtubo: outTubo " << dest << endl;
	
              (*nodes[dest])->deliver(m);

              if( (*nodes[dest])->idle() && !(*nodes[dest])->get_busy() )
                 (*nodes[dest])->activateAfter( current() );
         break;
	 }
      }
   }
}


