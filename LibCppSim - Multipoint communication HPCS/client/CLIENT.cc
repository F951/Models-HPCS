#include "CLIENT.h"

void CLIENT::inner_body(void)
{
	
	int debug=0;
	
   while(1)
   {
      if(ClientWSE::queue_in_wse.empty())
         passivate();
       
       ASSERT(!ClientWSE::queue_in_wse.empty());

       Message *temp;
       temp = ClientWSE::queue_in_wse.front();
       ClientWSE::queue_in_wse.pop_front();
		
		temp->source=PEER;
		
		double DELAY = ((ID_numb/nodes) + 0.1*(temp->id_tarea%(totalQueries/2)/(double)totalQueries) )*10 ;
		busy=true;
		hold(DELAY);
		busy=false;
		
		if(debug){cout<<"ClientWSE "<< (*I_obj)->id_peer_client << ". Envío mensaje a NETWORK - Time: " << this->time() << endl;}
		
		(*network)->add_message(temp);
		if( (*network)->idle() && !(*network)->get_busy() ){
			(*network)->activateAfter( current() );
		}
   }
}


long int CLIENT::getTimeIncremental(long int last_TTL)
{
    last_TTL= this->time() + last_TTL + STATIC_TIMEOUT;
    if((last_TTL-this->time())>MAX_TTL)
       last_TTL=this->time() + MAX_TTL;
    return last_TTL;
}


