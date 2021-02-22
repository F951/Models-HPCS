#include "ClientTLCWSE.h"
#include "TlcProtocol.h"

#include <iostream>
#include <fstream>
#include <ctime>

void ClientTlcWse::inner_body(void)
{
	int debug_cliente=0;
	//char *ptr;
	string ptr;
	int sentQueries=0;
	srand (static_cast <unsigned> (this->time()));
	// FILE * archivo_hash1;
	
	int indice;
	//~ const char * buffer;
	
	ofstream salida_WSE_y_CLIENTE;
	salida_WSE_y_CLIENTE.open ("Resultados_r1d/salida_WSE_y_CLIENTE.txt",ios_base::app);
	double T_trabajo;
	Query *q;
	vector <int> tareas_repetidas;	
	H=(*observer)->Peers_envio;

   while(1)
   {	
		//while (end_cliente){
		//	passivate();
		//}
		while ( flag_terminar==1 ){
			passivate();
		}
		
		if(ClientWSE::queue_in_wse.empty())
		{
			// salida_WSE_y_CLIENTE.close();
			passivate();
		}
		
		ASSERT(!ClientWSE::queue_in_wse.empty());

		MessageWSE *temp;
		temp = ClientWSE::queue_in_wse.front();
		ClientWSE::queue_in_wse.pop_front();
		
		if (debug_cliente) {escritura_a_archivo::GetStream() <<"=================== ClientTlcWSE ================="<<endl;}
		if (debug_cliente) {escritura_a_archivo::GetStream() <<"#"<<temp->rta->id_tarea<<". clienteWSE nro: "<< (*I_obj)->id_peer_client<<endl; fflush(stdout);}
		if (AGREGACION_PEERS==0){
			if (debug_cliente) {escritura_a_archivo::GetStream() <<"clienteWSE: agregacion en servidor"<< endl; fflush(stdout);}
			switch (temp->tag) {
				case 0:
				{
					(*observer)->N_tag0_client++;
					(*observer)->Time_tag0_client+=this->time();
					q = new Query(sentQueries, temp->getKey(), temp->getQuery(),  this->time(),temp->TTL_tarea,temp->rta);
					if ((*wse)->cacheSize!=0){
						//Generación de una consulta desde el Peer a la red. Sirve para el caso en que el Peer necesita pedir una imagen para etiquetar
						if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente) Case 0" << endl;}
						if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente) Case 0 =============================================== Tarea - " << temp->rta->id_tarea <<". Consulta generada por el PEER:"<<(*(*tlc)->pastry)->idTransporte<<" para la red (tag 0)"<< endl; fflush(stdout);}
						sentQueries++;
						(*tlc)->add_query( q );
						if ((*tlc)->idle() && !(*tlc)->get_busy())         
							(*tlc)->activateAfter(current());
					}else{
						if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente) Case 0 sin caché. Pedir el objeto al WSE" << endl;}
						if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente) Case 0 =============================================== Tarea - " << temp->rta->id_tarea <<". Consulta generada por el PEER:"<<(*(*tlc)->pastry)->idTransporte<<" para la red (tag 0)"<< endl; fflush(stdout);}
						//~ busy=true;	hold(LATENCIA_PEER_WSE);busy=false;
						sendWSEQuery(q);					
					}
					break;
				}
				
				case 1:
				{
					(*observer)->N_tag1_client++;
					(*observer)->Time_tag1_client+=this->time();
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente- Case 1) "<< endl;}
					if ((*wse)->cacheSize!=0){if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente- Case 1) - PEER:"<<(*(*tlc)->pastry)->idTransporte<<", tarea: "<< temp->rta->id_tarea<< endl; fflush(stdout);}}
					else{if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente- Case 1) - PEER:"<<(*this->I_obj)->id_peer_client<<", tarea: "<< temp->rta->id_tarea<< endl; fflush(stdout);}}
					//CREO UNA NUEVA ENTRY CON LOS DATOS QUE RECIBI
					Entry *newEntry = new Entry(this->time(), temp->getKey(), temp->getQuery(),  temp->getVersion(), ENTRY_SIZE, getTTL(temp->getQuery()),temp->TTL_tarea,temp->rta->opciones,temp->prioridad_tarea,temp->id_imagen,temp->rta);
					if ((*wse)->cacheSize!=0){if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente- Case 1) - PEER:"<<(*(*tlc)->pastry)->idTransporte<<", tarea: "<< temp->rta->id_tarea<<". Envío mensaje TLC para responder con el objeto a los peers" <<endl; fflush(stdout);}}
					else{if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente- Case 1) - PEER:"<<(*this->I_obj)->id_peer_client<<", tarea: "<< temp->rta->id_tarea<<". Envío mensaje TLC para responder con el objeto a los peers" <<endl; fflush(stdout);}}
					TlcMessage *tm = new TlcMessage(TLC_WSE, (*tlc)->msgID, newEntry);
					(*tlc)->msgID=(*tlc)->msgID+1;
					(*tlc)->add_message(tm);
					if ((*tlc)->idle() && !(*tlc)->get_busy())         
						(*tlc)->activateAfter(current());
					break;
				}
				
				case 2:
				{
					(*observer)->N_tag2_client++;
					(*observer)->Time_tag2_client+=this->time();
					//~ if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER:"<<(*(*tlc)->pastry)->idTransporte<<", tarea: "<< temp->rta->id_tarea<< endl; fflush(stdout);}
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?.  tarea: "<< temp->rta->id_tarea<< endl; fflush(stdout);}
					//~ escritura_a_archivo::GetStream() << "TAG 2. Clave Ptr - Key: "<< temp->getKey() <<". ID de la imagen: " << temp->id_imagen<< endl; fflush(stdout);
					//~ if (0) {escritura_a_archivo::GetStream() << "Mensaje recibido en el PEER:"<<(*(*tlc)->pastry)->idTransporte<<" con TAG 2, proviniente desde el PEER "<< (*(*tlc)->observer)->codigos[temp->responsable_agreg] << " de codigo "<< BN_bn2hex(temp->responsable_agreg) << endl; fflush(stdout);}
					busy=true;
					//HOLD PARA ATENCION
					double lower_bound = 0;
					double upper_bound = 300;
					rng<double> *generador_double = new rngUniform( "Uniform", lower_bound, upper_bound );
					double tiempo_atencion = generador_double->value();
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?. tarea: "<< temp->rta->id_tarea<< ". Hacer Hold de atencion"<< endl;}
					hold(tiempo_atencion);
					busy=true;
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?. tarea: "<< temp->rta->id_tarea<< ". Hold de atencion hecho"<< endl;}
					busy=false;
					
					//HOLD PARA TRABAJO
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?. tarea: "<< temp->rta->id_tarea<< ". Hacer Hold de trabajo"<< endl;}					
					//~ T_trabajo= work_time->value();
					T_trabajo = 0.4 + work_time1->value( ) + work_time2->value( );
					busy=true;
					hold(T_trabajo);
					busy=false;
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?. tarea: "<< temp->rta->id_tarea<< ". Hold de trabajo hecho"<< endl;}					
					//Enviar tiempo de trabajo al WSE para acumular tiempo de utilización del peer (carga).
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?. tarea: "<< temp->rta->id_tarea<< ". Holds hechos, acumular tiempo de trabajo"<< endl; fflush(stdout);}

					//Para enviar el tiempo al observador desde este cliente, se podría usar como intermediario el nodo tlc, que sí tiene el handle.
					(*observer)->utilizacion[ (*(this->I_obj))->id_peer_client ]+=T_trabajo;
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?. tarea: "<< temp->rta->id_tarea<< ". Elegir opciones"<< endl; fflush(stdout);}
					//AÑADIR SENTENCIAS PARA MODIFICAR OPCIONES (TRABAJO) SEGÚN FLAG DE HABILIDAD.
					list<int>::iterator it;
					int jj=0;
					for (it=temp->rta->opciones.begin();it!=temp->rta->opciones.end();it++)	{
						if (habilidad==1){
							float ar = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/1.0);
							*it=(ar<0.7);
							jj++;			
						} 
						else{
							float ar = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/1.0);
							*it=(ar<0.5);
							jj++;
						}
					}
					//Pongo en 0 el consenso, es para rellenar ese campo que uso en el caso 2
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?.  tarea: "<< temp->rta->id_tarea<< ". Crear mensaje para el WSE"<< endl; fflush(stdout);}
					MessageWSE* wseQuery = new MessageWSE(ClientTlcWse::get_ClientWSE_obj(),temp->getQuery(),temp->getKey(),3, temp->TTL_tarea, 0,temp->id_imagen,temp->rta);
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?.  tarea: "<< temp->rta->id_tarea<< ". Hacer el HOLD"<< endl; fflush(stdout);}
					//~ busy=true;	hold(LATENCIA_PEER_WSE);busy=false;
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?.  tarea: "<< temp->rta->id_tarea<< ". Añadir requerimmiento al WSE"<< endl; fflush(stdout);}
					(*wse)->consultas_enviadas_desde_peers++;
					//(*wse)->add_request(wseQuery);
					//if((*wse)->idle() && !(*wse)->get_busy()){
					//	(*wse)->activateAfter(current());
					//}
					wseQuery->set_type(PEER_to_WSE);
					(*isp)->add_message(wseQuery);
					if( (*isp)->idle() && !(*isp)->get_busy() ){
						(*isp)->activateAfter( current() );
					}	
					if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente - Case 2) - PEER: ?.  tarea: "<< temp->rta->id_tarea<< ". Salgo de Case2"<< endl; fflush(stdout);}
					break;
				}
				
				default:
				{
					escritura_a_archivo::GetStream() << "========="<< endl; fflush(stdout);
					escritura_a_archivo::GetStream() << "Llega mensaje al ClienteWSE con tag distinto de 0 o 1"<< endl; fflush(stdout);
				}
			}
				
		}else{	
			if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". clienteWSE: agregacion en peers"<< endl; fflush(stdout);}
			if (debug_cliente) {cout << "#"<<temp->rta->id_tarea<<". clienteWSE: agregacion en peers"<< endl; fflush(stdout);}
			switch (temp->tag) {
				case 0:
				{
					(*observer)->N_tag0_client++;
					(*observer)->Time_tag0_client+=this->time();
					//Generación de una consulta desde el Peer a la red. Sirve para el caso en que el Peer necesita pedir una imagen para etiquetar
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 0" << endl;}
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 0 =============================================== Tarea - " << temp->rta->id_tarea <<". Consulta generada por el PEER:"<<(*(*tlc)->pastry)->idTransporte<<" para la red (tag 0)"<< endl; fflush(stdout);}
					q = new Query(sentQueries, temp->getKey(), temp->getQuery(),  this->time(),temp->TTL_tarea,temp->rta);
					sentQueries++;
					(*tlc)->add_query( q );
					if ((*tlc)->idle() && !(*tlc)->get_busy())         
						(*tlc)->activateAfter(current());
					break;
				}
				
				case 1:
				{
					(*observer)->N_tag1_client++;
					(*observer)->Time_tag1_client+=this->time();
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 1 ========= -------------------------------------------------------- "<< endl; fflush(stdout);}
					//temp->rta->print_rta();
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 1  Mensaje recibido en el PEER:"<<(*(*tlc)->pastry)->idTransporte<<". con TAG 1 ========================================================="<< endl; fflush(stdout);}
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 1  Clave Ptr - Key: "<< temp->getKey() <<". ID de la imagen: " << temp->id_imagen<< endl; fflush(stdout);}
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Responsable de la tarea según modificación: " << temp->rta->Responsable_agreg->getIP() << endl;}
					Entry *newEntry = new Entry(this->time(), temp->getKey(), temp->getQuery(),  temp->getVersion(), ENTRY_SIZE, getTTL(temp->getQuery()),temp->TTL_tarea,temp->rta->opciones,temp->prioridad_tarea,temp->id_imagen,temp->rta);
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) PEER: "<< (*(*tlc)->pastry)->idTransporte << ". En_proceso_local.size(): "<< en_proceso_local.size() << endl;}
					//~ if (0) {escritura_a_archivo::GetStream() << "(Cliente) PEER: "<< (*(*tlc)->pastry)->idTransporte << ". En_proceso_local->peers_consultados.size(): "<< en_proceso_local[temp->rta->id_tarea]->peers_consultados.size() << endl;}
					//Envío la Entry al nodo TLC y Pastry de este mismo Peer, para que pueda grabarse en Cache (inserción global y local)
					TlcMessage *tm = new TlcMessage(TLC_WSE, (*tlc)->msgID, newEntry, (*(*tlc)->pastry)->ne);
					(*tlc)->msgID=(*tlc)->msgID+1;
					(*tlc)->add_message(tm);
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) PEER: "<< (*(*tlc)->pastry)->idTransporte << ". Envío el objeto de la tarea: "<< temp->rta->id_tarea << " hacia la capa inferior TLC para mandarlo a los solicitantes."<< endl;}
					if ((*tlc)->idle() && !(*tlc)->get_busy())         
						(*tlc)->activateAfter(current());
					break;
				}
				
				case 2:
				{
					(*observer)->N_tag2_client++;
					(*observer)->Time_tag2_client+=this->time();
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 2 - ========="<< endl; fflush(stdout);}
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 2 - =========================================== Mensaje recibido en el PEER:"<<(*(*tlc)->pastry)->idTransporte<<" con TAG 2. ";}
					indice = temp->rta->id_tarea;
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 2 - Clave Ptr - Key: "<< temp->getKey() <<". ID de la imagen: " << temp->rta->id_img << ". Tarea: " << indice << ". Responsable de la task: " << temp->rta->Responsable_agreg->getIP() << endl; fflush(stdout);}
					if (temp->rta->reenviada==1){
						if (debug_cliente) {printLista(temp->rta->peers_consultados_2,"temp->rta->peers_consultados_2",temp->rta->id_tarea);}
					}
					busy=true;
					//HOLD PARA ATENCION
					double lower_bound = 0;
					double upper_bound = 300;
					rng<double> *generador_double = new rngUniform( "Uniform", lower_bound, upper_bound );
					double tiempo_atencion = generador_double->value();
					hold(tiempo_atencion);
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Salida hold tiempo_atencion - Tarea:" <<   indice <<endl;}
					busy=false;
					//HOLD PARA TRABAJO
					//~ T_trabajo= work_time->value();
					T_trabajo = 0.4 + work_time1->value( ) + work_time2->value( );
					busy=true;
					hold(T_trabajo);
					busy=false;
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Salida hold T_trabajo - Tarea:" << indice <<endl;}
					//Enviar tiempo de trabajo al WSE para acumular tiempo de utilización del peer (carga).
					if ((*wse)->cacheSize!=0){
						(*(*tlc)->observer)->utilizacion[ ((*(*tlc)->pastry)->idTransporte) ]+=T_trabajo;
					}else{
						(*observer)->utilizacion[ (*(this->I_obj))->id_peer_client ]+=T_trabajo;
					}
					//AÑADIR SENTENCIAS PARA MODIFICAR OPCIONES (TRABAJO) SEGÚN FLAG DE HABILIDAD.
					list<int>::iterator it;
					int jj=0;
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 2 - temp->id_imagen: " << temp->id_imagen << endl;}
					if (temp->id_imagen != 0){
						if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 2 - tamaño de la lista de opciones: " << temp->opciones.size() << endl; }
						for (it=temp->rta->opciones.begin();it!=temp->rta->opciones.end();it++)	{
							if (habilidad==1){
								float ar = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/1.0);
								*it=(ar<0.7);
								jj++;			
							} 
							else{
								float ar = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/1.0);
								*it=(ar<0.5);
								jj++;
							}
						}
						if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 2 - Opciones elegidas: " << endl; }
						//printLista(temp->rta->opciones,"temp->rta->opciones",temp->rta->id_tarea);
				
					} else {
						if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Case 2 - La imagen es 0"<< endl;}
					}
					Entry *newEntry = new Entry(this->time(), temp->getKey(), temp->getQuery(),  temp->getVersion(), ENTRY_SIZE, getTTL(temp->getQuery()),temp->rta->TTL_tarea,temp->rta->opciones,temp->rta->prioridad_tarea,temp->rta->id_img,temp->rta);
					TlcMessage *tm = new TlcMessage(TLC_SEND_RESP, (*tlc)->msgID, newEntry);
					(*tlc)->msgID=(*tlc)->msgID+1;
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 2 - POR MANDAR RESPUESTA AL RESPONSABLE. PEER: " << (*(*tlc)->pastry)->idTransporte <<". Tarea: " << indice<< ". (temp->responsable_agreg)->getIP(): " << (temp->responsable_agreg)->getIP() << ". temp->src->getIP(): " << (*(*(*temp->src)->get_tlc_of_client())->pastry)->idTransporte << "(temp->rta->Responsable_agreg)->getIP()" << (temp->rta->Responsable_agreg)->getIP() << endl;}
					//printLista(temp->rta->opciones,"temp->rta->opciones",temp->rta->id_tarea);
					tm->src=(*(*tlc)->pastry)->ne;
					//~ tm->destNode=(temp->responsable_agreg);
					tm->destNode=(temp->rta->Responsable_agreg);
					(*tlc)->add_message(tm);
					if ((*tlc)->idle() && !(*tlc)->get_busy())         
						(*tlc)->activateAfter(current());
					break;
				}
				
				case 3:
				{
					(*observer)->N_tag3_client++;
					(*observer)->Time_tag3_client+=this->time();
					(*wse)->consultas_totales++;
					
					(*observer)->contador_recibidas=consultas_totales;
					int id_peer;	
					id_peer=temp->responsable_agreg->getIP();		//Este campo del mensaje tiene otro nombre, pero en este caso lo uso para identificar de dónde viene el mensaje.
					(*wse)->SolvedTasks[id_peer]++;
					//id_peer=temp->src->id_peer_client;
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) =============================================================================" << endl;}
					int indice;			const char * buffer;		buffer=(temp->key).c_str();			indice = atoi (buffer);
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - RESPUESTA RECIBIDA en PEER " << (*(*tlc)->pastry)->idTransporte << " DESDE " << id_peer << ". Indice: " << indice << ", temp->rta->id_tarea: "<< temp->rta->id_tarea << endl;}
					//~ if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente) Case 3 (responsable) - RESPUESTA RECIBIDA en PEER " << (*(*tlc)->pastry)->idTransporte << " DESDE " << id_peer << ". Tarea: " << indice << endl;}
					//if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente) Case 3 (responsable) - Responsable de la tarea según modificación: " << temp->rta->Responsable->getIP() << endl;}
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - Id de tarea: "<<indice <<". Está en 'EN_PROCESO_LOCAL'? "<< (en_proceso_local.count(indice)!=0) << endl;}
					//~ if (en_proceso_local.count(indice)==0 || ( en_proceso_local.count(indice)==1 && en_proceso_local[indice]->reenviada==0 && temp->rta->reenviada==1 )){	
					
					if (en_proceso_local.count(indice)==0){
						//Esto pasa sólo al principio, cuando el responsable no conoce la tarea. O sea que no hay forma de que la tarea ya esté reenviada. Por eso pongo los flags en 0.
						Rtas_Peers* rta = new Rtas_Peers();
						rta->copy(temp->rta);
						en_proceso_local[temp->rta->id_tarea] = rta;
						en_proceso_local[temp->rta->id_tarea]->Responsable_agreg = temp->rta->Responsable_agreg;
						en_proceso_local[temp->rta->id_tarea]->enviada_al_wse_1 = 0;
						en_proceso_local[temp->rta->id_tarea]->enviada_al_wse_2 = 0;
						en_proceso_local[temp->rta->id_tarea]->reenviada = 0;
						if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - Tarea : " << temp->rta->id_tarea << " insertada en 'en_proceso_local de Peer: " << (*(*tlc)->pastry)->idTransporte << endl;}
					}
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<"\n";}
					//~ en_proceso_local[indice]->print_rta();
					//~ temp->rta->print_rta();
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - Tarea : " << temp->rta->id_tarea <<  ". en_proceso_local[indice]->reenviada: "<< en_proceso_local[indice]->reenviada << ". temp->rta->reenviada: " << temp->rta->reenviada<< endl;}
					if ( en_proceso_local[indice]->reenviada == 0 && temp->rta->reenviada==1){
						//Actualizar respuesta (lista de peers consultados)
						if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - Tarea : " << temp->rta->id_tarea << " ACTUALIZADA en 'en_proceso_local de Peer: " << (*(*tlc)->pastry)->idTransporte << endl;}
						Rtas_Peers* rta = new Rtas_Peers();
						rta->copy(temp->rta);
						//~ int aux1 = en_proceso_local[temp->rta->id_tarea]->reenviada;
						int aux1 = en_proceso_local[temp->rta->id_tarea]->enviada_al_wse_1;
						int aux2 = en_proceso_local[temp->rta->id_tarea]->enviada_al_wse_2;
						en_proceso_local[temp->rta->id_tarea] = rta;
						en_proceso_local[temp->rta->id_tarea]->enviada_al_wse_1 = aux1;
						en_proceso_local[temp->rta->id_tarea]->enviada_al_wse_2 = aux2;
						en_proceso_local[temp->rta->id_tarea]->Responsable_agreg = temp->rta->Responsable_agreg;
						//~ en_proceso_local[temp->rta->id_tarea]->reenviada = temp->rta->reenviada;
						en_proceso_local[temp->rta->id_tarea]->enviada_al_wse_1 = temp->rta->enviada_al_wse_1;
						if (debug_cliente) {printLista(en_proceso_local[temp->rta->id_tarea]->peers_consultados_1,"peers_consultados_1",temp->rta->id_tarea);}
						if (debug_cliente) {printLista(en_proceso_local[temp->rta->id_tarea]->peers_consultados_2,"peers_consultados_2",temp->rta->id_tarea);}
						if (debug_cliente) {printLista(temp->rta->peers_consultados_2,"temp->rta->peers_consultados_2",temp->rta->id_tarea);}
					}
					//~ if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente) Case 3 (responsable) - Recibidas: " << en_proceso_local[indice]->rtas_recibidas << ". Esperadas: "<< en_proceso_local[indice]->rtas_esperadas << endl;}
					if ( en_proceso_local[indice]->reenviada == temp->rta->reenviada){ //Esta es la vía usual
						if ( en_proceso_local[indice]->reenviada==0){
							//~ if (debug_cliente) {escritura_a_archivo::GetStream() << "(Cliente) Case 3 (responsable) - Recibidas: " << en_proceso_local[indice]->rtas_recibidas << ". Esperadas: "<< en_proceso_local[indice]->rtas_esperadas << endl;}
							en_proceso_local[indice]->peers_que_respondieron_1.push_back(id_peer);
							//-----------------------------------------Depuracion de peers que se espera que respondan---------------------------------------------------------
							if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Cantidad de peers consultados 1: " << en_proceso_local[indice]->peers_consultados_1.size()<< endl;}
							if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Cantidad de peers_que_respondieron 1: " << en_proceso_local[indice]->peers_que_respondieron_1.size()<< endl;}
							if (debug_cliente) {printLista(en_proceso_local[indice]->peers_consultados_1,"en_proceso_local[indice]->peers_consultados_1",temp->rta->id_tarea);}
							if (debug_cliente) {printLista(en_proceso_local[indice]->peers_que_respondieron_1,"en_proceso_local[indice]->peers_que_respondieron_1",temp->rta->id_tarea);}
							if ( (this->time() - (en_proceso_local[indice]->tiempo_inicio) ) > temp->TTL_tarea)	{ //El TTL de tarea es INVÁLIDO
								if (en_proceso_local[indice]->enviada_al_wse_1 == 0) {
									//Si no se mandó la agregación al WSE, hacerlo.
									if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - Early stopping 1. Esperadas: "<< en_proceso_local[indice]->peers_consultados_2.size() << ", recibidas: "<< en_proceso_local[indice]->peers_que_respondieron_1.size()<<". Calcular votación por TTL vencido" << endl;}
									(*observer)->contador_recepcion2++;
									//~ int	consenso_calculado=
									calcular_consenso(	indice,		en_proceso_local[indice]->rtas_esperadas,
														en_proceso_local,	 0.6,  en_proceso_local[indice]->rtas_esperadas,	temp);
									//~ en_proceso_local[indice]->reenviada=0;
									en_proceso_local[indice]->rtas_recibidas=0;
									en_proceso_local[indice]->enviada_al_wse_1=1;
								}
								else
								{
									//Si se mandó, no hacer nada y esperar el reenvío para hacer la nueva agregación
									if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". SALIDA POR RESPUESTA OBSOLETA: " << endl;}
								}
							}
							else
							{		//TTL VALIDO
								acumular_rtas(temp->rta, id_peer,indice);
								(*observer)->contador_recepcion1++;
								//Pregunto si ya recibí las H que espero.
								if (en_proceso_local[indice]->peers_consultados_1.size()==en_proceso_local[indice]->peers_que_respondieron_1.size())	//Si llegaron todas las que esperaba...
								{
									if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - Se recibieron las "<< en_proceso_local[indice]->peers_consultados_1.size() << " respuestas esperadas en el responsable para la tarea: "<< indice<<". Calcular votación" << endl;}
									//~ int consenso_calculado=
									calcular_consenso(	indice,		en_proceso_local[indice]->rtas_esperadas,
														en_proceso_local,	 0.6,  en_proceso_local[indice]->rtas_esperadas,	temp);
									//~ en_proceso_local[indice]->reenviada=1;
									en_proceso_local[indice]->rtas_recibidas=0;
									en_proceso_local[indice]->enviada_al_wse_1=1;
								}	
							}
						}else{
							if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - Recibidas: " << en_proceso_local[indice]->rtas_recibidas << ". Esperadas: "<< en_proceso_local[indice]->rtas_esperadas << endl;}
							en_proceso_local[indice]->peers_que_respondieron_2.push_back(id_peer);
							//-----------------------------------------Depuracion de peers que se espera que respondan---------------------------------------------------------
							if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Cantidad de peers consultados 2: " << en_proceso_local[indice]->peers_consultados_2.size()<< endl;}
							if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Cantidad de peers_que_respondieron 2: " << en_proceso_local[indice]->peers_que_respondieron_2.size()<< endl;}
							if (debug_cliente) {printLista(en_proceso_local[indice]->peers_consultados_2,"en_proceso_local[indice]->peers_consultados_2",temp->rta->id_tarea);}
							if (debug_cliente) {printLista(en_proceso_local[indice]->peers_que_respondieron_2,"en_proceso_local[indice]->peers_que_respondieron_2",temp->rta->id_tarea);}
							if (debug_cliente) {diferencias_en_listas(en_proceso_local[indice]->peers_consultados_2,en_proceso_local[indice]->peers_que_respondieron_2,temp->rta->id_tarea);}
							if ( (this->time() - (en_proceso_local[indice]->tiempo_inicio) ) > temp->TTL_tarea)	{ //El TTL de tarea es INVÁLIDO
								if (en_proceso_local[indice]->enviada_al_wse_2 == 0) {
									//Si no se mandó la agregación al WSE, hacerlo.
									if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - Early stopping 2. Esperadas: "<< en_proceso_local[indice]->peers_consultados_2.size() << ", recibidas: "<< en_proceso_local[indice]->peers_que_respondieron_2.size()<<". Calcular votación por TTL vencido" << endl;}
									(*observer)->contador_recepcion2++;
									//~ int consenso_calculado=
									calcular_consenso(	indice,		en_proceso_local[indice]->rtas_esperadas,
														en_proceso_local,	 0.6,  en_proceso_local[indice]->rtas_esperadas,	temp);
									//~ en_proceso_local[indice]->reenviada=0;
									en_proceso_local[indice]->rtas_recibidas=0;
									en_proceso_local[indice]->enviada_al_wse_2=1;
								}
								else
								{
									//Si se mandó, no hacer nada y esperar el reenvío para hacer la nueva agregación
									if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - TTL INVALIDO, pero ya fue avisado el servidor" << endl;}
								}
							}
							else
							{	//TTL VALIDO
								if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - TTL VALIDO. verifico si contestaron todos para Calcular votación" << endl;}
								acumular_rtas(temp->rta, id_peer,indice);
								(*observer)->contador_recepcion1++;
								//Pregunto si ya recibí las H que espero.
								if (en_proceso_local[indice]->peers_consultados_2.size()==en_proceso_local[indice]->peers_que_respondieron_2.size())	//Si llegaron todas las que esperaba...
								{
									if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - Se recibieron las "<< en_proceso_local[indice]->peers_consultados_2.size() << " respuestas esperadas en el responsable para la tarea: "<< indice<<". Calcular votación" << endl;}
									//~ int consenso_calculado=
									calcular_consenso(	indice,		en_proceso_local[indice]->rtas_esperadas,
														en_proceso_local,	 0.6,  en_proceso_local[indice]->rtas_esperadas,	temp);
									//~ en_proceso_local[indice]->reenviada=1;
									en_proceso_local[indice]->rtas_recibidas=0;
									en_proceso_local[indice]->enviada_al_wse_2=1;
								}
								else
								{
									if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) Case 3 (responsable) - TTL VALIDO. Faltan respuestas" << endl;}
								}	
							}
						}
					}
					else{	//Caso de respuesta vieja (llega rta del primer envío y la tarea ya fue reenviada).
						//Ignorar mensaje.
						//Escape por respuesta vieja. Corresponde al primer envío de la tarea, y ahora el segundo envío es el que se está procesando.
						//Envío este mensaje porque puede ser que el wse se haya pasivado, y se cuelgue el simulador.
						
						//(*observer)->contador_recepcion2++;
						//if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". (Cliente) CASE 3 (responsable) - (la respuesta era de un envío inicial)"  << endl;}
						//busy=true;	hold(LATENCIA_PEER_WSE);busy=false;
						//MessageWSE* wseQuery = new MessageWSE(ClientTlcWse::get_ClientWSE_obj(),temp->getQuery(),temp->getKey(),4, temp->TTL_tarea, 0,temp->id_imagen,en_proceso_local[indice]);
						//(*wse)->add_request(wseQuery);
						//if((*wse)->idle() && !(*wse)->get_busy())
						//	(*wse)->activateAfter(current());					
					}
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Cliente) Case 3 (responsable) salgo de case 3"<<endl;}
					//~ (*wse)->mostrar_tareas_en_proceso(temp->rta->id_tarea);
					//~ (*wse)->mostrar_tareas_en_proceso_cout(temp->rta->id_tarea);
					//~ if (debug_cliente) {escritura_a_archivo::GetStream() <<"\n";}
					break;
				}
				default:
				{
					//~ escritura_a_archivo::GetStream() << "========="<< endl; fflush(stdout);
					if (debug_cliente) {escritura_a_archivo::GetStream() << "#"<<temp->rta->id_tarea<<". Llega mensaje al ClienteWSE con tag distinto de 0 o 1"<< endl; fflush(stdout);}
				}
			}
		}
   }

}

long int ClientTlcWse::getTTL(BIGNUM* b)
{
   long int query_TTL;
   long int last_TTL;
   Entry* old = (*tlc)->RCACHE->check(b);
   if(old==NULL)
   {
      query_TTL = MIN_TTL;
   }
   else
   {
      last_TTL = old->getTimeOut();
      query_TTL = ClientTlcWse::getTimeIncremental(last_TTL);
      
   }
   return query_TTL;
}

long int ClientTlcWse::getTimeIncremental(long int last_TTL)
{
//   escritura_a_archivo::GetStream() << " incremental" << endl;
// TODO: Search function!!!!
    last_TTL= this->time() + last_TTL + STATIC_TIMEOUT;
    if((last_TTL-this->time())>MAX_TTL)
       last_TTL=this->time() + MAX_TTL;
    return last_TTL;
}

void ClientTlcWse::set_tlc(handle<TlcProtocol> *t)
{
  tlc =t;
  (*I_obj)->set_tlc_of_client(tlc);
}

void ClientTlcWse::sendWSEQuery(Query* q)
//void ClientTlcWse::sendWSEQuery(Query* q, int indice)
{	
	
	//~ list<int> opciones;
	if (0) {escritura_a_archivo::GetStream() << "WSE QUERY - Tarea: " << q->rta->id_tarea << endl;}
	//MessageWSE* wseQuery = new MessageWSE(ClientTlcWse::get_ClientWSE_obj(), q->hashValue, q->term, PEER,    q->TTL_tarea,q->opciones,q->prioridad_tarea,0,0,id_tarea);
	MessageWSE* wseQuery = new MessageWSE(ClientTlcWse::get_ClientWSE_obj(), q->hashValue, q->term, PEER,    q->TTL_tarea,0,0,q->rta);
    //(*wse)->add_request(wseQuery);
    //if((*wse)->idle() && !(*wse)->get_busy())
	//	(*wse)->activateAfter(current());
	wseQuery->set_type(PEER_to_WSE);
	(*isp)->add_message(wseQuery);
	if( (*isp)->idle() && !(*isp)->get_busy() ){
		(*isp)->activateAfter( current() );
	}	
}


int ClientTlcWse::calcular_consenso(int indice, 
							int peers_reenvio, 
							map<int, Rtas_Peers*> &en_proceso_local,
							double umbral_votacion, 
							int H,
							MessageWSE* &temp
							)
	{
		//-------------------------------------------------------------------------------------
		int nro_respuestas=0;
		for (int i=0; i<(int)(en_proceso_local[indice])->votacion.size();i++) {
			nro_respuestas=en_proceso_local[indice]->rtas_recibidas;
		}
		std::clock_t c_start;
		std::clock_t c_end;
		long double time_elapsed_ms;
		int SAVE_CPUtime_STATS=0;
		ofstream archivo_calculo_de_consenso_cliente;
		if (SAVE_CPUtime_STATS) {
			archivo_calculo_de_consenso_cliente.open ("stats_CPUtime/archivo_calculo_de_consenso_cliente.txt",ios_base::app);
		}
		c_start = std::clock();
		//-------------------------------------------------------------------------------------		
		int consenso=0;
		if (0) {escritura_a_archivo::GetStream() << "(CLIENTE) Cantidad de opciones votadas en la tarea " <<indice<<": " << (en_proceso_local[indice])->votacion.size() << endl;}
		//~ en_proceso_local[indice]->print_rta();
		//Para analizar la tarea que se completó, recorrer las listas de votación y ver cuántos peers votaron.
		for (int i=0; i<(int)(en_proceso_local[indice])->votacion.size();i++)  {
			if ( ((en_proceso_local[indice])->votacion[i].size()>umbral_votacion*H ))	{
			//if ( ((en_proceso_local[indice])->votacion[i].size()> 7 ))	{	//Umbral de votación fijo, según análisis de bibliografía para conf de parámetros.
				consenso=1;
				en_proceso_local[indice]->consenso=1;
			}
		}
		busy=true; hold(CTE_CONSENSO_WSE);busy=false;
		(*observer)->t_consenso_peers+=CTE_CONSENSO_WSE;
		c_end = std::clock();//------------------------------------------------------------------------------------
		time_elapsed_ms = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
		if (SAVE_CPUtime_STATS) { archivo_calculo_de_consenso_cliente << time_elapsed_ms << ";" << H <<";" << nro_respuestas<< ";"<<en_proceso_local[indice]->rtas_recibidas<<endl;}//--------------------------------
		//~ busy=true; hold(CTE_CONSENSO_CLIENTE);busy=false;
		if (0) {escritura_a_archivo::GetStream() << "(CLIENTE) Consenso: " << consenso << endl;}
		//Envío a la tarea al WSE
		Rtas_Peers *rta= new Rtas_Peers();
		rta->copy(en_proceso_local[indice]);
		rta->Responsable_agreg=en_proceso_local[indice]->Responsable_agreg;
		//~ rta->reenviada=1;
		if (0) {escritura_a_archivo::GetStream() << "#"<<indice<<". Envío respuesta desde el responsable al servidor. Reenviada?" << temp->rta->reenviada <<endl;}
		//~ busy=true;	hold(LATENCIA_PEER_WSE);busy=false;
		MessageWSE* m = new MessageWSE(ClientTlcWse::get_ClientWSE_obj(),temp->getQuery(),temp->getKey(),3, temp->TTL_tarea, consenso,temp->id_imagen,rta);
		//(*wse)->add_request(wseQuery);
		//if((*wse)->idle() && !(*wse)->get_busy())
		//	(*wse)->activateAfter(current());
		m->set_type(PEER_to_WSE);
		(*isp)->add_message(m);
		if( (*isp)->idle() && !(*isp)->get_busy() ){
			(*isp)->activateAfter( current() );
		}	
		return consenso;
	}






	
void ClientTlcWse::printLista(std::list<int> & lista,const char* nombre, int indice)
{
	std::list<int>::iterator it_lista;
	escritura_a_archivo::GetStream() << "#"<<indice<<". ==============Lista: "<< nombre << "\n";
	escritura_a_archivo::GetStream() << "#"<<indice<<". ";
	for (it_lista=lista.begin(); it_lista!=lista.end(); ++it_lista)
	{
		//~ std::escritura_a_archivo::GetStream() << ' ' << *it_lista;
		escritura_a_archivo::GetStream() << ' ' << *it_lista;
	} 
	escritura_a_archivo::GetStream() << "\n#"<<indice<<". ==============\n";
}

void ClientTlcWse::mostrar_tareas_en_proceso_local()
{
	for(map<int,Rtas_Peers*>::const_iterator it = en_proceso_local.begin(); it != en_proceso_local.end(); ++it){
		escritura_a_archivo::GetStream() << it->second->id_tarea <<",";
	}escritura_a_archivo::GetStream() << "\n";
}

void ClientTlcWse::mostrar_tareas_en_proceso()
{
	escritura_a_archivo::GetStream() << "en_proceso (WSE) .size(): " << (*wse)->en_proceso.size() << endl;
	for(map<int,Rtas_Peers*>::const_iterator it = (*wse)->en_proceso.begin(); it != (*wse)->en_proceso.end(); ++it){
		escritura_a_archivo::GetStream() << it->second->id_tarea <<",";
	}escritura_a_archivo::GetStream() << "\n";
}
void ClientTlcWse::acumular_rtas(Rtas_Peers* rta, int id_peer, int indice){

	list<int>::iterator it1;	//Con it1 recorro la lista de opciones del mensaje que llega.
	list<int>::iterator it2;	//Con it2 recorro la lista "en proceso"
	int jj=0;
	for (it1=rta->opciones.begin();it1!=rta->opciones.end();it1++){	
		if ((*it1)==1){
			(en_proceso_local[indice])->votacion[jj].push_back(id_peer);
		}
		jj++;	//Sirve sólo para mostrar qué opción se está sumando
	}
}

void ClientTlcWse::diferencias_en_listas(list<int> &lista1, list<int>& lista2, int indice){
	vector <int> diferencias;
	list<int>::iterator it_prueba1,it_prueba2;
	for (it_prueba1=lista1.begin(); it_prueba1!=lista1.end(); ++it_prueba1)
	{
		it_prueba2 = find(lista2.begin(), lista2.end(), *it_prueba1);
		if(it_prueba2 == lista2.end()){
			diferencias.push_back(*it_prueba1);
		}
	}
	(*wse)->print_vec(diferencias,"diferencias",indice);	
}

