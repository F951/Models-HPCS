#include "redtubo.h"

void RedTubo::inner_body( ){

	//int cont_flag =0;
	double DELAY=0;
	int debug_red=0;
	while(1) {
		
		if ( flag_terminar ==1 ){
			passivate();
		}
		if ( queue_in.empty() ){
			passivate();
		}
		ASSERT( !queue_in.empty() );
		MessageT* m = queue_in.front();
		if (debug_red) {cout << "red tubo - queue_in.front()->getDataType(): " << (queue_in.front())->getDataType() << endl;}
		if (debug_red) {cout << "red tubo - m->getDataType(): " << m->getDataType() << endl;}
		queue_in.pop_front();

		//// Simulates sending a message to other peer
		//busy = true;
		//hold(DELAY_BASE_P2P);	//HOLD TENIENDO EN CUENTA LAS ESTADISTICAS DE LATENCIA DE RED QUE ME PASÓ VERO.
		//busy = false; 
		//(*observer)->t_red_p2p+=DELAY_BASE_P2P;
		
		
		switch (m->getType())
		{
			/**** case T_BDC:
				// if((m->getDest())==0 &&( m->getSrc())==0)
			{
				for(unsigned int i=0; i<nodes.size(); i++)
				{
					(*nodes[i])->deliver(m->clone());
					if ((*nodes[i])->idle() )//&& !(*nodes[i])->get_busy())
					{  
						(*nodes[i])->activateAfter(current());
					}		   
				}
				delete m;
				break;
			} ****/

			case T_P2P:
			{
				// Simulates sending a message to other peer 	//--------------------------------------------
				busy = true;
				hold(DELAY_BASE_P2P);	//HOLD TENIENDO EN CUENTA LAS ESTADISTICAS DE LATENCIA DE RED QUE ME PASÓ VERO.
				busy = false; 
				(*observer)->t_red_p2p+=DELAY_BASE_P2P;			//--------------------------------------------
				
				if (debug_red) {cout << "red tubo - m->getDataType(): " << m->getDataType() << endl;}
				switch (m->getDataType())
				{
					case SOLICITUD_TAREA:
					{
						if (debug_red) {cout << "red tubo: LATENCIA_SOLICITUD_TAREA_P2P" << endl;}
						DELAY=LATENCIA_SOLICITUD_TAREA_P2P;
						break;
					}
					case SOLICITUD_OBJETO:
					{
						if (debug_red) {cout << "red tubo: LATENCIA_SOLICITUD_OBJETO_P2P" << endl;}
						DELAY=LATENCIA_SOLICITUD_OBJETO_P2P;
						break;
					}
					case OBJETO_OPCIONES:
					{
						if (debug_red) {cout << "red tubo: LATENCIA_OBJETO_OPCIONES_P2P" << endl;}
						DELAY=LATENCIA_OBJETO_OPCIONES_P2P;
						break;
					}
					case TRABAJO_TERMINADO:
					{
						if (debug_red) {cout << "red tubo: LATENCIA_TRABAJO_TERMINADO_P2P" << endl;}
						DELAY=LATENCIA_TRABAJO_TERMINADO_P2P;
						break;
					}
					case TRABAJO_AGREGADO:
					{
						if (debug_red) {cout << "red tubo: LATENCIA_TRABAJO_AGREGADO_P2P" << endl;}
						DELAY=LATENCIA_TRABAJO_AGREGADO_P2P;
						break;
					}
					default:
					{
						if (debug_red) {cout << "red tubo: ERROR" << endl;}
						break;
					}
				}
				busy = true;
				hold(DELAY);	//HOLD TENIENDO EN CUENTA LAS ESTADISTICAS DE LATENCIA DE RED QUE ME PASÓ VERO.
				busy = false; 
				(*observer)->t_red_p2p+=DELAY;

				/* Destination comes within the MessageT */
				int dest = m->getDest(); 
				(*nodes[dest])->deliver(m);

				if( (*nodes[dest])->idle() && !(*nodes[dest])->get_busy() )
					(*nodes[dest])->activateAfter( current() );
				break;
			}
		}
	}
}


