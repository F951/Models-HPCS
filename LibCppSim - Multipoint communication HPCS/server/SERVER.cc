#include "SERVER.h"


void SERVER::inner_body(void)
{
	int debug=0;
	srand (1);
	
   while(1)
   {
		
		if(queue_in.empty() )
			passivate();

		ASSERT(!queue_in.empty());

		Message* m = queue_in.front();

		queue_in.pop_front();
		
		double DELAY = ((ID_numb/nodes) + 0.1*(m->id_tarea%(totalQueries/2)/(double)totalQueries) )*10 ;
		
		
		busy=true;
		hold(DELAY);
		busy=false;
		
		switch ( m->source )
		{
			case PEER:		//NETWORK.
			{
				/***********Enviar estadísticas al observador.**************/
				if(debug){cout<<"WSE "<< ID_numb<< "- recibo dato de peer"<< (*(m->src))->id_peer_client << ". StartTime: "<< m->start_time <<". EndTime: "<< this->time() << endl;}
				(*obs)->terminadas++;
				(*obs)->addLatencia(this->time() - m->start_time);
				/***********Enviar estadísticas al observador.**************/
					
				break;
			}
			
			case USER:		//Generador.
			{
				/*********** Elegir peer de destino aleatorio y ponerlo en el mensaje **************/
				int chosen = (m->id_tarea + ID_numb) % nodes;
				if(debug){cout<<"WSE "<< ID_numb<< "- enviar dato a peer"<< chosen << ". Time: " << this->time() << endl;}
				handle<CLIENT> *puntero_cliente;	
				handle<ClientWSE>* handle_cliente_WSE;
				puntero_cliente = clientes_Peers[chosen];
				handle_cliente_WSE=(*puntero_cliente)->get_ClientWSE_obj();	
				m->src=handle_cliente_WSE;
				m->start_time=this->time();
				/*********** Elegir peer de destino aleatorio y ponerlo en el mensaje **************/
				(*network)->add_message(m);
				if( (*network)->idle() && !(*network)->get_busy() ){
					(*network)->activateAfter( current() );
				}	
				break;
			}
				

			default: cout<<"ERROR: WSE Type o msg"<<endl;
		}	    
	}
}
