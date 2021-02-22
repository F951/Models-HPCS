#include "NETWORK.h"
#include "../client/CLIENT.h"

void NETWORK::inner_body( ){

	int debug_network=0;
	
	while(1) {
		if ( flag_terminar==1){
			passivate();
		}
		if ( queue_in.empty() ){
			passivate();
		}
		ASSERT( !queue_in.empty() );
		Message* m = queue_in.front();
		queue_in.pop_front();
		
		
		double DELAY = m->size;
		
		if(debug_network){cout<<"network - recibo dato y me preparo para el hold- time: "<< this->time() <<  ". Proximo Hold: " << DELAY <<  endl;}
		busy=true;
		hold(DELAY);
		busy=false;
		(*obs)->addLatencia_network(DELAY);
		
		switch (m->source)
		{
			case USER:
			{
				if(debug_network){cout<<"NETWORK - enviar dato a peer: "<< (*(m->src))->id_peer_client << " - Time: "<< this->time() <<  endl;}
				(*(m->src))->receiveWSE(m);
				if( (*(m->src))->idle() && !(*(m->src))->get_busy() )
					(*(m->src))->activateAfter( current() );
				break;
			}
			
			case PEER:
			{
				if(debug_network){cout<<"network - enviar dato a WSE desde peer"<< (*(m->src))->id_peer_client << "time: "<< this->time() <<  endl;}
				
				(*server[m->server])->add_request(m);
				if( (*server[m->server])->idle() && !(*server[m->server])->get_busy() )
					(*server[m->server])->activateAfter( current() );
				break;
			}
			
		}
	}
}


void NETWORK::printMSG(Message *m){
	//~ cout << "Mensaje en NETWORK de tipo: " << m->getType() << endl;
	//~ cout << "Tarea: " << m->rta->id_tarea << endl;
	//~ cout << "TAG: " << m->tag << endl;
	//~ cout << "SRC: " << (*(m->src))->id_peer_client << endl;
}
