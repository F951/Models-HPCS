#include "WSE.h"
#include "../BD/BD.h"
#include "../p2pLayer/NodeEntry.h"
#include "../applicationLayer/TlcProtocol.h"
#include "../applicationLayer/ClientTLCWSE.h"
#include "../isp/ISP.h"
#include "../applicationLayer/Query.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//#include <random>
#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#include <algorithm>
#include <random>
#include <chrono>
void WSE::inner_body(void)
{		
		std::stringstream str;
		handle<TlcProtocol> *gq;
		handle<ClientTlcWse> *puntero_cliente_wse;	
		string ptr;
		int chosen=0;
		Mje_Gen_WSE *mG;
		//Mje_WSE_BD *mBD;
		MessageWSE* m;
		MessageWSE *mP;
		handle<ClientWSE>* handle_cliente_WSE;
		Rtas_Peers *rta;
		BIGNUM* hashValue ;
		en_proceso.clear();
		ofstream salida_WSE_y_CLIENTE;
		salida_WSE_y_CLIENTE.open ("mensajes_sim/salida_WSE_y_CLIENTE.txt",ios_base::app);
		
		
		//Registros de tamaño de colas "en_proceso" y de entrada de mensajes de los peers.
		ofstream estadisticas_wse_en_wse;
		int escribir_estadisticas=0;
		if (escribir_estadisticas==1){estadisticas_wse_en_wse.open ("registros/estadisticas_wse_en_wse.txt",ios_base::app);}
		vector<int> longitud_en_proceso;
		vector<int> longitud_queue_in;
		vector<double> tiempos;
		//~ double marca_inicio_virtual_t_wse=0;
		//~ double marca_final_virtual_time_wse=0;
		int indice;
		const char * buffer;
		//double newRate = (1/rate_reenvio);
		double newRate = (1/250.0);
		rng<double>* Tiempo_reenvio= new rngExp("Tiempo Renvío", newRate);
		Tiempo_reenvio->reset();
		for (int n=0;n<NP;n++){
			mapa_prioridad.insert (std::make_pair(0,n));
		}
		int contador_miss_en_proceso=0;
		vector <int> frecuencias_voluntarios;
		vector <int> opciones_totales_voluntarios;
		
		vector <int> TotalTasks;
		vector <double> ranking_voluntarios;
		for (int i=0; i<NP;i++)	{SolvedTasks.push_back(0);}
		for (int i=0; i<NP;i++)	{TotalTasks.push_back(0);}
		for (int i=0; i<NP;i++)	{frecuencias_voluntarios.push_back(0);}
		for (int i=0; i<NP;i++)	{opciones_totales_voluntarios.push_back(0);}
		for (int i=0; i<NP;i++)	{ranking_voluntarios.push_back(0);}
		vector <double> porc_utilizacion_voluntarios;
		for (int i=0; i<NP;i++)	{porc_utilizacion_voluntarios.push_back(0);}		
		list<int>::iterator it_peers_consultados;	
		(*observer)->T_inicio=this->time();
		int responsable_agreg_asignado=0;
		int contador_tareas_enviadas=0;
   while(1)
   {	
	   if (end_WSE == 1 ){
			passivate();
	   }
	   
	   
		if (DEBUG_WSE) {escritura_a_archivo::GetStream() <<"(WSE) ends[NP]="<< ends[NP] << ". Testigo: " << consultas_enviadas_desde_peers << ". Testigo TLC: " << consultas_enviadas_desde_tlc <<". size 'en_proceso': " << en_proceso.size() << ". gen stop: " << gen_stop <<endl;}
		if (DEBUG_WSE) {cout <<"(WSE) ends[NP]="<< ends[NP] << ". Testigo: " << consultas_enviadas_desde_peers << ". Testigo TLC: " << consultas_enviadas_desde_tlc <<". size 'en_proceso': " << en_proceso.size() << ". gen stop: " << gen_stop <<endl;}
		if ( ends[NP] == 1 && (en_proceso.size()==0)) {
			if (cacheSize!=0) {
				handle<TlcProtocol> *gq;
				gq = Peers[0];
				(*gq)->sendEndBroadcast();
			}
			(*observer)->ends_obs[NP]=1;
			(*observer)->T_finalizacion=this->time();
			(*observer)->end();
			passivate();
		}
		if(queue_in.empty() && queue_in_Gen.empty()){
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() <<"contador_miss_en_proceso: " << contador_miss_en_proceso << endl;}
			if ( (consultas_totales >= enviadas )  &&(en_proceso.size()==0 && gen_stop==1)){	//Enviadas se aumenta cuando hay msjes del GEN, y en reenvío.
				if (cacheSize!=0) {
					handle<TlcProtocol> *gq;
					gq = Peers[0];
					(*gq)->sendEndBroadcast();
				}
				ends[NP] = 1;
				(*observer)->ends_obs[NP]=1;
				(*observer)->T_finalizacion=this->time();
				(*observer)->end();
				passivate();
			}
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() <<"(WSE): No finalizo por consultas recibidas " << endl;}
			if (end_WSE && (en_proceso.size()==0) ){		//Apagar réplicas
				if (cacheSize!=0) {
					for( int i=0 ;i<(int)Peers.size();i++)	{ 
						gq = Peers[i];
						(*gq)->EndReplicator();
					}		
				}
			}
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() <<"(WSE): Pasivar " << endl;}
			passivate();
		}
				
		if (!queue_in_Gen.empty())   //Procesa mjes que vienen del generador
		{			
			ASSERT(!queue_in_Gen.empty());	//Control
			mG =  queue_in_Gen.front();		//Leo el mensaje
			queue_in_Gen.pop_front();		//Saco el mensaje de la cola
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". =================== WSE Gen================="<<endl;}
			double lower_bound = parametro_TTL*0.95; 		//TTL - Para crear el random flotante (double), uso un generador de libcppsim.
			double upper_bound = parametro_TTL*1.05;
			rng<double> *generador_double = new rngUniform( "Uniform", lower_bound, upper_bound );
			double TTL = generador_double->value();
			
			rta = new Rtas_Peers(mG->id_tarea,mG->id_img, TTL, H, mG->prioridad,mG->opciones,this->time(), mG->tiempo_generacion); // Cambio en el TTL
			rta->enviada_al_wse_1=0;
			rta->enviada_al_wse_2=0;
			en_proceso[mG->id_tarea] = rta;
			str.str(std::string());;
			str << mG->id_tarea;
			str.str();
			hashValue = h->GenerateKey((char*)(str.str()).c_str());
			ptr=(char*)(str.str()).c_str();
			
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". (WSE) Por hacer el primer hold" << endl;}
			busy=true; hold(TIME_GEN1);busy=false;
			(*observer)->Time_tagGen_wse+=TIME_GEN1;
			/****
			if (cacheSize!=0){	
				gq = Peers[0];
				en_proceso[mG->id_tarea]->Responsable=new NodeEntry((*observer)->codigos2[responsable_asignado],responsable_asignado);
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". Responsable: " << responsable_asignado << endl;}
				responsable_asignado++;
				if (responsable_asignado==NP){responsable_asignado=0;}
			}	
			*****/
			contador_tareas_enviadas++;
			if (cacheSize!=0){	
				if (contador_tareas_enviadas <20 ){
					responsable_agreg_asignado = rand()%NP;		//Se elige al azar
				}else{
					responsable_agreg_asignado = seleccion_peer_agregacion(porc_utilizacion_voluntarios);
				}
				en_proceso[mG->id_tarea]->Responsable_agreg=new NodeEntry((*observer)->codigos2[responsable_agreg_asignado],responsable_agreg_asignado);
			}	
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". Enviar tarea a H peers"<<endl;}
			
			busy=true; hold(LATENCIA_BD_TASK);busy=false;
			t_com_wse_bd+=LATENCIA_BD_TASK;
				
			for (int jj=0;jj<H;jj++){
				// Seleccionar Peer: aleatorio
				int repeticion=1;
				if (en_proceso[mG->id_tarea]->peers_consultados_1.size()!=0){
					while (repeticion==1){
						repeticion=0;
						chosen = rand()%NP;
						for (it_peers_consultados=en_proceso[mG->id_tarea]->peers_consultados_1.begin();it_peers_consultados!=en_proceso[mG->id_tarea]->peers_consultados_1.end();it_peers_consultados++)
						{
							if (*it_peers_consultados == chosen){
								repeticion=1;
							}
						}
						//~ if (repeticion==1){escritura_a_archivo::GetStream() << "REPETICION - Tarea: "<< mG->id_tarea << ". Chosen: " << chosen << endl;}
						//~ printLista(en_proceso[mG->id_tarea]->peers_consultados,"peers_consultados");
					}
				}
				else
				{
					chosen = rand()%NP;						
				}
				ends[chosen]++; 
				(*observer)->ends_obs[chosen]++;
				en_proceso[mG->id_tarea]->peers_consultados_1.push_back(chosen);
				for (int k=0; k< (int)((mG->opciones).size());k++){
					opciones_totales_voluntarios[chosen]+=1;
				}
			}	
			std::list<int>::iterator it_lista;
			//~ printLista(en_proceso[mG->id_tarea]->peers_consultados, "en_proceso[mG->id_tarea]->peers_consultados");
			for (it_lista=en_proceso[mG->id_tarea]->peers_consultados_1.begin(); it_lista!=en_proceso[mG->id_tarea]->peers_consultados_1.end(); ++it_lista)
			{	
				if (cacheSize!=0){	
					if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". Chosen = " << chosen <<endl;}
					gq = Peers[chosen];
					handle_cliente_WSE=(*(*gq)->client)->get_ClientWSE_obj();
				}else{
					if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". Chosen = " << chosen <<endl;}
					puntero_cliente_wse = clientes_wse_Peers[chosen];
					handle_cliente_WSE=(*puntero_cliente_wse)->get_ClientWSE_obj();
				}
				chosen=*it_lista;
				//Envío todo al Peer, menos la imagen (id_img)
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". Enviar tarea a H peers DEBUG 5"<<endl;}
				mP = new MessageWSE(handle_cliente_WSE, hashValue, ptr,USER,TTL,0,0,rta);
				busy=true; hold(TIME_GEN2);busy=false;
				(*observer)->Time_tagGen_wse+=TIME_GEN2;
				(*observer)->N_tagGen_wse++;
				mP->set_type(WSE_to_PEER);
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". Enviar tarea con sentido: " << mP->getType() <<endl;}
				(*isp)->add_message(mP);
				if( (*isp)->idle() && !(*isp)->get_busy() ){
					(*isp)->activateAfter( current() );
				}
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". Salida de inner body" <<endl;}
				enviadas++;
				(*observer)->contador_enviadas=enviadas;
			}
			//~ if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<mG->id_tarea<<". TAREA " << mG->id_tarea << " ENVIADA a H peers"<<endl;}
			longitud_en_proceso.push_back(en_proceso.size());
			longitud_queue_in.push_back(queue_in.size());
			tiempos.push_back(this->time());
			busy=true; hold(TIME_GEN3);busy=false;
			(*observer)->Time_tagGen_wse+=TIME_GEN3;
		}
		else if (!queue_in.empty()) 	//Procesa mjes que vienen de peers
		{
			control_TTLs();
			longitud_en_proceso.push_back(en_proceso.size());
			longitud_queue_in.push_back(queue_in.size());
			tiempos.push_back(this->time());
			
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() <<"=================== WSE Peer================="<<endl;}
			ASSERT(!queue_in.empty());
			m = queue_in.front();
			queue_in.pop_front();		
			buffer=(m->key).c_str();
			indice = atoi (buffer);
			int id_peer;
			id_peer =  (*(m->src))->id_peer_client;
			if (AGREGACION_PEERS==0){
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". WSE - AGREGACION EN EL SERVIDOR" <<endl;}
				switch ( m->source ){
					case PEER:
					{
						(*observer)->Time_tagPeer_wse+=this->time();
						(*observer)->N_tagPeer_wse++;
						if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". WSE Peer================= CASE PEER ======================== - ID PEER: " << id_peer <<". TAREA: " << indice <<endl;}
						if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". ''''''''' CASE PEER - Consulta de PEER:"<< id_peer << " al WSE  - Tarea: " << indice <<endl;}
						if (cacheSize!=0){
							m->tag=1;
						}else{
							m->tag=2;
						}
						if (en_proceso.count(indice)!=0) 	//La tarea está en "en_proceso"
						{
							m->id_imagen=en_proceso[indice]->id_img;
							if ( !((this->time() - (en_proceso[indice]->tiempo_inicio) ) <= en_proceso[indice]->TTL_tarea))	{	//Controlamos si el TTL es válido						
								//TTL vencido - Sacar la tarea de en_proceso, enviar la tarea a la BD, y no contestar al responsable. Que la tarea le quede al responsable sin contestar.
								if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". Terminar tarea por TTL vencido" <<endl;}
								termino_tarea(indice,en_proceso,0,bd);
								(*observer)->contador_responsable_elim++; 
							} else {
								m->set_type(WSE_to_PEER);
								busy=true; hold(LATENCIA_BD_IMG);busy=false;
								t_com_wse_bd+=LATENCIA_BD_IMG;
								(*isp)->add_message(m);
								if( (*isp)->idle() && !(*isp)->get_busy() ){
									(*isp)->activateAfter( current() );
								}
								if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". Respuesta enviada desde el WSE al Responsable (con tag 1) " <<endl;}
								(*observer)->contador_responsable_env++; 
							}
						}
						else 	{	//La tarea NO está en "en_proceso"
							if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". ############# La tarea "<< indice<<" ya no está en 'en_proceso' ############# "<<endl;}
						}
						virtual_t_wse_peers+=TIME_PEER1;
						busy=true; hold(TIME_PEER1);busy=false;
						if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". Salida hold TIME_PEER1 - Tarea:" << indice <<endl;}
						break;
					}
					
					case 3:		//Luego de hacer el trabajo, el peer contesta con la lista de opciones elegidas. 
					{	
						(*observer)->Time_tag3_wse+=this->time();
						(*observer)->N_tag3_wse++;
						consultas_totales++;
						SolvedTasks[id_peer]++;
						(*observer)->contador_recibidas=consultas_totales;
						if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". WSE Peer================= CASE 3 ======================== - ID PEER: " << id_peer <<". TAREA: " << indice <<endl;}
						if (MOSTRAR_SALIDA_WSE_CLIENT) {salida_WSE_y_CLIENTE << " ''''''''' CASE 3 - recibo respuesta del PEER:"<<(*(*(*(m->src))->get_tlc_of_client())->pastry)->idTransporte <<" en el WSE, y no envío otra tarea ''''''''' " << endl;}
						if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". Id de tarea: "<<indice <<". Está en 'en_proceso'? "<< (en_proceso.count(indice)!=0) << endl;}
						map<int, Rtas_Peers*>::iterator it3;
						if (en_proceso.count(indice)!=0) 	//La tarea está en "en_proceso"
						{	
							(en_proceso[indice]->rtas_recibidas)++;	//Aumento el contador de respuestas recibidas.
							if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". Chequear TTL" << endl;}
							if ( (this->time() - (en_proceso[indice]->tiempo_inicio) ) <= m->TTL_tarea)	//El TTL de tarea es válido
							{
								acumular_rtas(m->rta, id_peer,indice);
								if (en_proceso[indice]->rtas_recibidas==en_proceso[indice]->rtas_esperadas)	{//Si llegaron todas las que esperaba...
									if (DEBUG_UTILIZACION) {escritura_a_archivo::GetStream() << "Tiempo en Inner Body: "<< this->time() << endl;}
									if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". Calcular votación" << endl;}
									int enviadas_aux=calcular_votacion(	indice,		H,
																en_proceso,	 SolvedTasks,frecuencias_voluntarios,opciones_totales_voluntarios,ranking_voluntarios,porc_utilizacion_voluntarios,
																umbral_votacion,  H,
																NP,			mapa_prioridad,
																ends,		Peers,   clientes_wse_Peers,
																bd,
																longitud_en_proceso,
																longitud_queue_in,
																tiempos
																);
									enviadas+=enviadas_aux;		
									(*observer)->contador_enviadas=enviadas;
								}
								(*observer)->contador_recepcion1++;	
							}
							else
							{	
								/*******Calcular votación para la tarea, enviar a la BD y sacar de 'en_proceso'.****/
								if (DEBUG_UTILIZACION) {escritura_a_archivo::GetStream() << "Tiempo en Inner Body: "<< this->time() << endl;}						
								 int enviadas_aux=calcular_votacion(	indice,		H,
																en_proceso, SolvedTasks, frecuencias_voluntarios,opciones_totales_voluntarios,ranking_voluntarios,porc_utilizacion_voluntarios,
																umbral_votacion,  H,
																NP,			mapa_prioridad,
																ends,		Peers,   clientes_wse_Peers,
																bd,
																longitud_en_proceso,
																longitud_queue_in,
																tiempos
																	);
								enviadas+=enviadas_aux;
								(*observer)->contador_enviadas=enviadas;
								(*observer)->contador_recepcion2++;	
							}
						}
						else 		//La tarea NO está en "en_proceso"
						{	
							if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". CASE 3 - NO existe el Key del mensaje en 'en_proceso'" << endl;}
							contador_miss_en_proceso++;
							(*observer)->contador_recepcion3++;	
						}
						break;
					}		
					
					default: 
					{
						//~ escritura_a_archivo::GetStream() <<"ERROR: WSE Type o msg"<<endl;
						//~ escritura_a_archivo::GetStream() << "TAG: " << m->tag << endl;
						break;
					}

				}
			}else{
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". WSE - AGREGACION EN LOS PEERS" <<endl;}
				switch ( m->source ){
					case PEER:
					{
						(*observer)->Time_tagPeer_wse+=this->time();
						(*observer)->N_tagPeer_wse++;
						if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<".  ''''''''' CASE PEER - Consulta de PEER:"<< (*(*(*(m->src))->get_tlc_of_client())->pastry)->idTransporte << " al WSE  - Tarea: " << indice <<endl;}
						m->tag=1;
						if (en_proceso.count(indice)!=0) 	//La tarea está en "en_proceso"
						{
							if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". Existe el Key del mensaje en 'en_proceso'" << endl;}
							m->id_imagen=en_proceso[indice]->id_img;
							if ( !((this->time() - (en_proceso[indice]->tiempo_inicio) ) <= en_proceso[indice]->TTL_tarea))	{	//Controlamos si el TTL es válido						
								//TTL vencido - Sacar la tarea de en_proceso, enviar la tarea a la BD, y no contestar al responsable. Que la tarea le quede al responsable sin contestar.
								termino_tarea(indice,en_proceso,0,bd);
								(*observer)->contador_responsable_elim++; 
							} else {
								m->set_type(WSE_to_PEER);
								busy=true; hold(LATENCIA_BD_IMG);busy=false;
								t_com_wse_bd+=LATENCIA_BD_IMG;
								(*isp)->add_message(m);
								if( (*isp)->idle() && !(*isp)->get_busy() ){
									(*isp)->activateAfter( current() );
								}
								if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". Respuesta enviada desde el WSE al Responsable (con tag 1) - tarea: " << indice <<endl;}
								(*observer)->contador_responsable_env++; 
							}
						}
						else 	{	//La tarea NO está en "en_proceso"
							if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". ############# La tarea "<< indice<<" ya no está en 'en_proceso' ############# "<<endl;}
						}
						virtual_t_wse_peers+=TIME_PEER1;
						busy=true; hold(TIME_PEER1);busy=false;
						if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". Salida hold TIME_PEER1 - Tarea:" << indice <<endl;}
						break;
					}	
					
					case 3:		//Luego de hacer el trabajo, el peer contesta con la lista de opciones elegidas. 
					{		
						(*observer)->Time_tag3_wse+=this->time();
						(*observer)->N_tag3_wse++;
						int id_peer;
						id_peer = (*(*(*(m->src))->get_tlc_of_client())->pastry)->idTransporte;
						
						if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". (WSE)  Peer================= CASE 3 ======================== - ID PEER: " << id_peer <<". TAREA: " << indice <<endl;}
						// consultas_totales++;			//EN LA AGREGACION EN LOS PEERS (CASO 2 DEL SIMULADOR), A ESTE CONTADOR LO INCREMENTO EN EL CLIENTE-TLC-WSE
						if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". (WSE) CONSENSO en WSE: " << m->tag << endl;}
						//Luego muestro qué peers votaron bien, y les actualizo las frecuencias a esos peers.
						int consenso=m->tag;
						if (en_proceso.count(indice)!=0){	//Primero vemos si está en 'en_proceso'
							if (consenso==1){				//Si está, vemos si tuvo consenso o no.
								en_proceso[indice]->consenso=1;
								if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". (WSE) Actualizar frecuencias de los peers que votaron bien" << endl;}
								for (int i=0; i<(int)(m->rta->votacion.size());i++)		{
									if ( (m->rta->votacion[i].size()>umbral_votacion*H ))	{	//Umbral de votación como porcentaje del H.
									//if ( (m->rta->votacion[i].size()> 7 ))	{					//Umbral de votación fijo, según análisis de bibliografía para conf de parámetros.
										actualizar_frecuencias (frecuencias_voluntarios,mapa_prioridad, m->rta->votacion[i]);
										busy=true; hold(CTE_UPD_FREQ);busy=false;
										(*observer)->t_upd_freq_server+=CTE_UPD_FREQ;
									}
								}
								//Acá también tengo que actualizar el puntaje de los peers en ranking_voluntarios (teniendo en cuenta la utilizacion)
								actualizar_puntajes(SolvedTasks,ranking_voluntarios, porc_utilizacion_voluntarios, frecuencias_voluntarios, opciones_totales_voluntarios,	Peers);
								//Si hubo consenso, hay que terminar.
								termino_tarea(indice,en_proceso,consenso,bd);
							}else{
								if ((en_proceso[indice])->reenviada==0)		{
									if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". DEBUG - Reenviar tarea"<< endl;}
									//Si hay que reenviar, reenvío. Tengo que llamar a una función que elija los peers de reenvío según puntaje.
									m->rta->votacion[0].clear();
									actualizar_frecuencias (frecuencias_voluntarios,mapa_prioridad, m->rta->votacion[0]);
									busy=true; hold(CTE_UPD_FREQ);busy=false;
									(*observer)->t_upd_freq_server+=CTE_UPD_FREQ;
									enviadas+=reenvio_tarea(indice, H, en_proceso, ranking_voluntarios,NP,ends,Peers);
									(*observer)->contador_enviadas=enviadas;
								}else{
									//Si ya se reenvió y no hubo consenso, hay que terminar.
									termino_tarea(indice,en_proceso,consenso,bd);
								}
							}
						}else{
							if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". La tarea ya no está en en_proceso" << endl;}
						}
						break;
					}
					
					case 4:									
					{
						//~ escritura_a_archivo::GetStream() << "CASO 4 - Escape" << endl;
						break;
					}		
									
					default: 
					{
						//~ escritura_a_archivo::GetStream() <<"ERROR: WSE Type o msg"<<endl;
						//~ escritura_a_archivo::GetStream() << "TAG: " << m->tag << endl;
						break;
					}
				}
			}
		}		    
	}
}

int WSE::getVersion(string s, BIGNUM* b)
{
	int ttl;
	Answer* a= ANSWERS->check(s);
	if (a!=NULL)
	{
		if(a->getTimeOut()<this->time())
		{
			ttl = (this->time())+ TTL_WSE;
			// TTL es igual a new random number
			a->setNewVersion(ttl);
			//version +1
		}
	}
	else
	{
		// TTL es igual a new random number y version = 1
		ttl= (this->time()) + TTL_WSE;
		Answer *tmp=new Answer(this->time(), s, b,ttl,1,SIZE_ANSWER);
		a=tmp;
		ANSWERS->insertAnswer(tmp);
	}
	return a->getVersion();
}


// returns the version of the query answer
/*
int WSE::getVersion(BIGNUM* b)
{
int ttl;
  
 
   Answer* a= ANSWERS->check(b);

   if (a!=NULL)
   {
        if(a->getTimeOut()<this->time())
	{
            ttl = (this->time())+ randomTTL();
             // TTL es igual a new random number
            a->setNewVersion(ttl);
            //version +1
        }  
  }
  else
  {
     // TTL es igual a new random number y version = 1
     ttl= (this->time()) +randomTTL();
     Answer *tmp=new Answer(this->time(),  b,ttl,1,SIZE_ANSWER);
     a=tmp;
     ANSWERS->insertAnswer(tmp);
  }

 return a->getVersion();
}
*/
// TODO
long int WSE::randomTTL()
{
   long int r = rand();
  
   return r;
}

void WSE::encontrar_maximos(vector <double> &ranking_voluntarios, vector <int> &peers_seleccionados_reenvio,int H)
{
	std::vector<double> v;
	for (int i=0;i<(int)(ranking_voluntarios.size());i++){
		v.push_back(ranking_voluntarios[i]);
	}
	cout.precision(4);
	for (int i=0;i<H;i++){
		int maxElementIndex = std::max_element(v.begin(),v.end()) - v.begin();
		//~ double maxElement = *std::max_element(v.begin(), v.end());
		//escritura_a_archivo::GetStream() << "maxElementIndex:" << maxElementIndex << ", maxElement:" << maxElement << '\n';
		peers_seleccionados_reenvio.push_back(maxElementIndex);
		v[maxElementIndex]=-1;
	}
}

void WSE::encontrar_minimos(vector <double> &ranking_voluntarios, vector <int> &peers_seleccionados_reenvio,int H)
{
	//Esta función se usa para encontrar los peers con menor utilización, y así seleccionar los responsables
	//de realizar la agregación de tareas en la red (CASO 2 del simulador).
	std::vector<double> v;
	for (int i=0;i<(int)(ranking_voluntarios.size());i++){
		v.push_back(ranking_voluntarios[i]);
	}
	cout.precision(4);
	for (int i=0;i<H;i++){
		int minElementIndex = std::min_element(v.begin(),v.end()) - v.begin();
		//~ double minElement = *std::min_element(v.begin(), v.end());
		//~ escritura_a_archivo::GetStream() << "minElementIndex:" << minElementIndex << ", minElement:" << minElement << '\n';
		peers_seleccionados_reenvio.push_back(minElementIndex);
		v[minElementIndex]=0;
	}
}

int WSE::calcular_votacion(int indice, 
						int peers_reenvio, 
						map<int, Rtas_Peers*> &en_proceso,
						vector <int> &SolvedTasks,
						vector <int> &frecuencias_voluntarios,
						vector <int> &opciones_totales_voluntarios,
						vector <double> &ranking_voluntarios,
						vector <double> &porc_utilizacion_voluntarios,
						int umbral_votacion, 
						int H, 
						int NP,
						multimap<int,int> &mapa_prioridad, 
						int *ends,
						vector<handle<TlcProtocol> *> &Peers,
						vector<handle<ClientTlcWse> *> &clientes_wse_Peers,
						handle<BD> *bd,
						vector<int> &longitud_en_proceso,
						vector<int> &longitud_queue_in,
						vector<double> &tiempos
						
						)
	{
		//int nro_respuestas=0;
		//for (int i=0; i<(int)(en_proceso[indice])->votacion.size();i++) {
		//	nro_respuestas=en_proceso[indice]->rtas_recibidas;
		//}
		//escritura_a_archivo::GetStream().open ("escritura_a_archivo::GetStream().txt",ios_base::app);
		if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "================== calcular_votacion ==================" << endl;}
		if (DEBUG_UTILIZACION) {escritura_a_archivo::GetStream() << "Tiempo en calcular_votacion: "<< this->time() << endl;}						
		Mje_WSE_BD *mBD;
		MessageWSE *mP;
		handle<TlcProtocol> *gq;
		handle<ClientTlcWse> *puntero_cliente_wse;	

		int chosen=0;
		handle<ClientWSE>* handle_cliente_WSE;
		BIGNUM* hashValue;
		std::stringstream str;
		Hash* h = new Hash();
		string ptr;
		int enviadas=0;
		list<int> lista_reenvio;
		double multiplicador_TTL2=2;
		double TTL2 = multiplicador_TTL2*(en_proceso[indice]->TTL_tarea);
		if (cacheSize!=0){	
			gq = Peers[0];
		}else{
			puntero_cliente_wse = clientes_wse_Peers[0];
		}
		//===================PARTE DE CALCULO DE CONSENSO=====================================================================
		//----------------------------------------------------------------------
		//Usando la lista de votación (map)
		//Para analizar la tarea que se completó, recorrer las listas de votación y ver cuántos peers votaron.
		int consenso=0;
		if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Calcular consenso" << endl;}
		if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Cantidad de opciones en la tarea " <<indice<<": " << (en_proceso[indice])->votacion.size() << endl;}
		for (int i=0; i<(int)(en_proceso[indice])->votacion.size();i++) {
			// escritura_a_archivo::GetStream() << "Cantidad de opciones en la tarea " <<indice<<": " << (en_proceso[indice])->votacion.size() << endl;
			if ( (int)(en_proceso[indice])->votacion[i].size()>umbral_votacion*H ) {
			//if ( (int)(en_proceso[indice])->votacion[i].size()> 7  ) {		//Umbral de votación fijo, según análisis de bibliografía para conf de parámetros.
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Tamaño de la lista de peers que votaron para la opción "<< i<< ": " << (en_proceso[indice])->votacion[i].size() << endl;}
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Entro a actualizar frecuencias" << endl;}
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Salgo de actualizar frecuencias" << endl;}
				consenso=1;
				en_proceso[indice]->consenso=1;
			}
		}
		busy=true; hold(CTE_CONSENSO_WSE);busy=false;
		(*observer)->t_consenso_server+=CTE_CONSENSO_WSE;
		//===================PARTE DE CALCULO DE CONSENSO=====================================================================
		
		//===================PARTE DE ACTUALIZAR FRECUENCIAS=====================================================================
		for (int i=0; i<(int)(en_proceso[indice])->votacion.size();i++) {
			actualizar_frecuencias (frecuencias_voluntarios,mapa_prioridad, (en_proceso[indice])->votacion[i]);
			busy=true; hold(CTE_UPD_FREQ);busy=false;
			(*observer)->t_upd_freq_server+=CTE_UPD_FREQ;
		}	
		//===================PARTE DE ACTUALIZAR FRECUENCIAS=====================================================================
		
		//===================PARTE DE ACTUALIZAR PUNTAJES=====================================================================
		if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "================== actualizar_puntajes ==================" << endl;}
		actualizar_puntajes(SolvedTasks,ranking_voluntarios, porc_utilizacion_voluntarios, frecuencias_voluntarios, 
														opciones_totales_voluntarios,	Peers);		
		//(El hold() va en actualizar puntajes)													
		//===================PARTE DE ACTUALIZAR PUNTAJES=====================================================================
		//~ print_vec(frecuencias_voluntarios,"frecuencias_voluntarios");
		//~ print_vec(opciones_totales_voluntarios,"opciones_totales_voluntarios");
		//~ print_vec_double(porc_utilizacion_voluntarios,"Porc_utilizacion");
		//----------------------------------------------------------------------
	
		//Ahora usamos el multimapa de prioridad
		if ((consenso==0) && ((en_proceso[indice])->reenviada==0))
		{
			//=================== PARTE DE REENVIO (SIN BENCHMARK XQ YA HAY HOLD) =====================================================================
			(*observer)->contador_recepcion4++; 
			(en_proceso[indice])->votacion.clear(); 
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "====================== REENVIO TAREA "<< indice << "==========. Flag reenvío = "<< (en_proceso[indice])->reenviada << endl;}
			en_proceso[indice]->reenviada=1;
			//----------------------------------------------------------------------
			//Encontrar los H puntajes máximos
			//----------------------------------------------------------------------
			vector <int> peers_seleccionados_reenvio;
			//~ peers_seleccionados_reenvio.clear();
			encontrar_maximos(ranking_voluntarios,peers_seleccionados_reenvio,H);
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() << " ---------------------------------          ------------------------------ Consenso calculado" << endl;}
			//----------------------------------------------------------------------
			
			busy=true; hold(LATENCIA_BD_TASK);busy=false;
			t_com_wse_bd+=LATENCIA_BD_TASK;
				
			while (peers_seleccionados_reenvio.size() !=0)
			{
				chosen=peers_seleccionados_reenvio.back();
				if (cacheSize!=0){	
					gq = Peers[chosen];
					handle_cliente_WSE=(*(*gq)->client)->get_ClientWSE_obj();
				}else{
					puntero_cliente_wse = clientes_wse_Peers[chosen];
					handle_cliente_WSE=(*puntero_cliente_wse)->get_ClientWSE_obj();	
				}
				ends[chosen]++;		//incrementar ends[id_peer]
				en_proceso[indice]->rtas_esperadas++;
				en_proceso[indice]->peers_consultados_2.push_back(chosen);
				(*observer)->ends_obs[chosen]++;
				if (cacheSize!=0){	
					//~ handle_cliente_WSE=(*(*gq)->client)->get_ClientWSE_obj();	
				}else{
					//~ handle_cliente_WSE=(*puntero_cliente_wse)->get_ClientWSE_obj();	;
				}
				enviadas++;		//Incrementar el contador de enviadas del WSE (sirve para controlar ends[NP] entrando al while).
				//Envío todo al Peer, menos la imagen (id_img)
				str.str(std::string());;
				str << indice;
				str.str();
				hashValue = h->GenerateKey((char*)(str.str()).c_str());
				ptr=(char*)(str.str()).c_str();
				if (cacheSize!=0){
					Rtas_Peers* rta=new Rtas_Peers();
					rta->copy(en_proceso[indice]);
					mP = new MessageWSE(handle_cliente_WSE, hashValue, ptr,USER,TTL2, 0,0,rta);
				}else{
					Rtas_Peers* rta=new Rtas_Peers();
					rta->copy(en_proceso[indice]);
					mP = new MessageWSE(handle_cliente_WSE, hashValue, ptr,USER,TTL2, 2,en_proceso[indice]->id_img,rta);
				}
				(*observer)->Time_tagReenv_wse+=this->time();
				(*observer)->N_tagReenv_wse++;
				mP->set_type(WSE_to_PEER);
				(*isp)->add_message(mP);
				if( (*isp)->idle() && !(*isp)->get_busy() ){
					(*isp)->activateAfter( current() );
				}
				//~ lista_reenvio.pop_front();
				peers_seleccionados_reenvio.pop_back();
			}
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Hago el clear" << endl;}
			//lista_reenvio.clear();
			//=================== PARTE DE REENVIO (SIN BENCHMARK XQ YA HAY HOLD) =====================================================================
		}	
		else
		{
			//=================== PARTE DE TERMINAR TAREA (SIN BENCHMARK XQ YA HAY HOLD) =====================================================================
			if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Calcular votación - Rama 2" << endl;}
			if ((this->time() - (en_proceso[indice]->tiempo_inicio) ) >= TTL2 )	{
				(*observer)->contador_recepcion5++;
			}
			if (consenso==0){
				(*observer)->contador_recepcion6++;
			}
			if ( ((this->time() - (en_proceso[indice]->tiempo_inicio) ) >= TTL2) ||  (en_proceso[indice]->rtas_recibidas>=en_proceso[indice]->rtas_esperadas) || consenso==1)	//El TTL2 se agotó
			{
				//Medir tamaños de colas para estadísticas
				//~ escritura_a_archivo::GetStream() << "Longitud en proceso WSE: " << en_proceso.size() << ". Tiempo: " << this->time() << endl;
				//~ escritura_a_archivo::GetStream() << "Longitud queue_in WSE: " << queue_in.size() << ". Tiempo: " << this->time() << endl;
				longitud_en_proceso.push_back(en_proceso.size());
				longitud_queue_in.push_back(queue_in.size());
				tiempos.push_back(this->time());
				en_proceso[indice]->tiempo_finalizacion=this->time();
				Rtas_Peers* rta = new Rtas_Peers();
				rta->copy(en_proceso[indice]);
				//~ print_rta(rta);
				//Crear mensaje para BD.
				mBD = new Mje_WSE_BD(indice,	en_proceso[indice]->id_img,
					en_proceso[indice]->TTL_tarea,	en_proceso[indice]->opciones,
					en_proceso[indice]->prioridad_tarea, rta);
				//Enviar mensaje a BD.
				busy=true; hold(LATENCIA_BD_TASK);busy=false;
				t_com_wse_bd+= (LATENCIA_BD_TASK);
				(*bd)->add_request(mBD);
				if ((*bd)->idle() && !(*bd)->get_busy()) 
					(*bd)->activateAfter(current() );
				//Borrar tarea
				en_proceso.erase(indice);
				lista_reenvio.clear();
				if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Tarea "<< indice << " quitada de 'en_proceso'.============================== " << endl;}
				//~ (*bd)->print_tareas_resueltas();
			}
			//===================PARTE DE TERMINAR TAREA (SIN BENCHMARK XQ YA HAY HOLD) =====================================================================
		}
		if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Salgo de calcular votación " << endl;}
		return enviadas;
	}

//===============================================
//Resumen calcular_votacion
//calcular_consenso()
//loop for de actualizar_frecuencias
//actualizar_puntajes
//if ((consenso==0) && ((en_proceso[indice])->reenviada==0))
//	reenvio_tarea
//else
//	termino_tarea
//===============================================


void WSE::print_vec(vector<int> &vector_in, const char* nombre,int indice){
	//escritura_a_archivo::GetStream().open ("escritura_a_archivo::GetStream().txt",ios_base::app);
	escritura_a_archivo::GetStream() << "#"<<indice<<". Vector: " << nombre << endl;
	escritura_a_archivo::GetStream() << "#"<<indice<<".";
	for (int k=0;k<(int)(vector_in.size());k++){
		escritura_a_archivo::GetStream() << " " << vector_in[k];
	}escritura_a_archivo::GetStream() << "\n";
}

void WSE::print_vec_double(vector<double> &vector_in, const char* nombre){
	escritura_a_archivo::GetStream() << "Vector: " << nombre << endl;
	escritura_a_archivo::GetStream().precision(4);
	for (int k=0;k<(int)(vector_in.size());k++){
		escritura_a_archivo::GetStream() << " " << vector_in[k];
	}escritura_a_archivo::GetStream() << "\n";
}

void WSE::finalizacion()
{
	ends[NP] = 1; 
	end_WSE = 1;
	//~ escritura_a_archivo::GetStream() << "DETECCION DE PARADA. queue_in.size(): " << queue_in.size() << endl;
	//~ cout << "DETECCION DE PARADA. queue_in.size(): " << queue_in.size() << endl;
	if (cacheSize!=0){
		
		//handle<TlcProtocol> *gq;
		//gq = Peers[0];
		//(*gq)->sendEndBroadcast();
		for (int i=0; i<NP;i++){
						
			handle<ClientTlcWse> * puntero_cliente;
			puntero_cliente = clientes_wse_Peers[i];
			(*puntero_cliente)->terminar();
			
			handle<TlcProtocol> *gq;
			gq = Peers[i];
			(*gq)->terminar();		//Acá se activa el flag y también se notifica a la capa Pastry.
		}
		/*****Terminar red de transporte**********/
		//La termina el nodo pastry número 0
	}
	/**************Terminar ISP****************/
	(*isp)->terminar();
	
	(*observer)->tiempo_tareas_1=(*bd)->tiempo1;
	(*observer)->tiempo_tareas_2=(*bd)->tiempo2;
	(*observer)->ends_obs[NP]=1;
	(*observer)->T_finalizacion=this->time();
	//~ passivate();
}

void WSE::control_TTLs()
{
	//escritura_a_archivo::GetStream().open ("escritura_a_archivo::GetStream().txt",ios_base::app);
	if (0){escritura_a_archivo::GetStream() << "control de TTLs" << endl;}
	if (0){escritura_a_archivo::GetStream() << "this->time(): " << this->time() << " - control_TTLs_anterior: " << control_TTLs_anterior << " - parametro_TTL: " << parametro_TTL << endl;}
	if ( (this->time() - control_TTLs_anterior) > parametro_TTL ) {
		revisar_vencimientos();
		control_TTLs_anterior=this->time();
	}
}

void WSE::revisar_vencimientos()
{
	
	for(map<int,Rtas_Peers*>::const_iterator it = en_proceso.begin(); it != en_proceso.end(); ++it){
		double _tiempo_inicio = it->second->tiempo_inicio;
		if ( (this->time() - _tiempo_inicio ) > 2*it->second->TTL_tarea ){
			termino_tarea(it->second->id_tarea,en_proceso,0,bd);
		}
	}
}

void WSE::actualizar_puntajes(vector <int> &SolvedTasks, 	vector <double> &ranking_voluntarios,			vector <double> &porc_utilizacion_voluntarios,
						vector <int> &frecuencias_voluntarios,				vector <int> &opciones_totales_voluntarios,
						vector<handle<TlcProtocol> *> &Peers		)
{	
	double tiempo_actual=this->time();
	for (int i=0; i<(int)ranking_voluntarios.size();i++){
		porc_utilizacion_voluntarios[i]=(*observer)->utilizacion[i]/tiempo_actual;
		if ((((double)opciones_totales_voluntarios[i]) != 0 )    &&   ( (double)ends[i]!=0 )    &&   ( (double)opciones_totales_voluntarios[i])   ) {
			// Cuando envío la tarea, aumento el contador de Task[] respectivo al peer. Son las tareas que le fueron enviadas.
			// Cuando recibo la respuesta, aumento el contador de "TaskSolved", porque el peer respondió (después se verá si bien o mal).
			// ¿Qué hago cuando reenvío? ¿Qué pasa si reenvío la tarea al mismo peer?
			//ranking_voluntarios[i] = ((double)frecuencias_voluntarios[i]/(double)opciones_totales_voluntarios[i]) +  (1.0/porc_utilizacion_voluntarios[i]);
			ranking_voluntarios[i] = ((double)SolvedTasks[i]/(double)ends[i])*((double)frecuencias_voluntarios[i]/(double)opciones_totales_voluntarios[i]) +  (1.0/porc_utilizacion_voluntarios[i]);			
		}else{
			ranking_voluntarios[i] = (1.0/porc_utilizacion_voluntarios[i]);
		}
	}	
	busy=true; hold(CTE_SCORES);busy=false;
	(*observer)->t_scores_server+=CTE_SCORES;
}

int WSE::reenvio_tarea(int indice, 
						int H, 
						map<int, Rtas_Peers*> &en_proceso,
						vector <double> &ranking_voluntarios,
						int NP,
						int *ends,
						vector<handle<TlcProtocol> *> &Peers
					)
{
		
		if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "#"<<indice<<". (WSE) REENVIO de tarea (casos 1 y 2): " << indice<< endl;}
		//~ print_vec_double(ranking_voluntarios, "ranking_voluntarios");
		MessageWSE *mP;
		handle<TlcProtocol> *gq;
		int chosen=0;
		handle<ClientWSE>* handle_cliente_WSE;
		BIGNUM* hashValue;
		std::stringstream str;
		Hash* h = new Hash();
		string ptr;
		int enviadas=0;
		list<int> lista_reenvio;
		double multiplicador_TTL2=3;
		double TTL2 = multiplicador_TTL2*(en_proceso[indice]->TTL_tarea);
		//~ if(DEBUG_WSE) {escritura_a_archivo::GetStream() << "(WSE) ====================== REENVIO TAREA "<< indice << "==========. Escribo datos observador" << endl;}
		(*observer)->contador_recepcion4++;
		(en_proceso[indice])->votacion.clear(); 
		if(DEBUG_WSE) {escritura_a_archivo::GetStream() << "(WSE) ====================== REENVIO TAREA "<< indice << "==========. Flag reenvío = "<< (en_proceso[indice])->reenviada << endl;}
		en_proceso[indice]->reenviada=1;
		//Encontrar los H puntajes máximos
		vector <int> peers_seleccionados_reenvio;
		encontrar_maximos(ranking_voluntarios,peers_seleccionados_reenvio,H);	
		//~ if(DEBUG_WSE) {escritura_a_archivo::GetStream() << "(WSE) ====================== REENVIO TAREA "<< indice << "========== Peers seleccionados" << endl;}
		
		//Reenviar tarea
		for (int i=0; i<(int)peers_seleccionados_reenvio.size();i++)
		{
			 //~ print_vec_double(peers_seleccionados_reenvio,"peers_seleccionados_reenvio");
			chosen=peers_seleccionados_reenvio[i];
			gq = Peers[chosen];
			ends[NP]=0;			//Desactivar ends[NP]
			ends[chosen]++;		//incrementar ends[id_peer]
			//~ en_proceso[indice]->rtas_esperadas++;
			en_proceso[indice]->peers_consultados_2.push_back(chosen);	
			(*observer)->ends_obs[chosen]++;
		}
		if (DEBUG_WSE){printLista(en_proceso[indice]->peers_consultados_2,"en_proceso[indice]->peers_consultados_2");}
		str.str(std::string());			str << indice;			str.str();
		hashValue = h->GenerateKey((char*)(str.str()).c_str());		ptr=(char*)(str.str()).c_str();
		Rtas_Peers* rta = new Rtas_Peers();
		rta->copy(en_proceso[indice]);
		rta->Responsable_agreg=en_proceso[indice]->Responsable_agreg;
		rta->reenviada=1;
		rta->enviada_al_wse_1=1;
		rta->enviada_al_wse_2=0;
		//~ if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Información de tarea reenviada a los nuevos peers:" << endl;}
		//~ rta->print_rta();
		
		busy=true; hold(LATENCIA_BD_TASK);busy=false;
		t_com_wse_bd+=LATENCIA_BD_TASK;		
			
		for (int i=0; i<(int)peers_seleccionados_reenvio.size();i++)
		{
			chosen=peers_seleccionados_reenvio[i];
			gq = Peers[chosen];
			handle_cliente_WSE=(*(*gq)->client)->get_ClientWSE_obj();	
			enviadas++;		//Incrementar el contador de enviadas del WSE (sirve para controlar ends[NP] entrando al while).
			//mP = new MessageWSE(handle_cliente_WSE, hashValue, ptr,USER,TTL2,en_proceso[indice]->opciones_originales,en_proceso[indice]->prioridad_tarea, 0,0);	
			mP = new MessageWSE(handle_cliente_WSE, hashValue, ptr,USER,TTL2, 0,0,rta);			//Envío todo al Peer, menos la imagen (id_img)
			(*observer)->Time_tagReenv_wse+=this->time();
			(*observer)->N_tagReenv_wse++;
			mP->set_type(WSE_to_PEER);
			(*isp)->add_message(mP);
			if( (*isp)->idle() && !(*isp)->get_busy() ){
				(*isp)->activateAfter( current() );
			}			
		}
		//~ if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "(WSE) Hago el clear" << endl;}
		peers_seleccionados_reenvio.clear();
		lista_reenvio.clear();
		return enviadas;
}

void WSE::termino_tarea(	int indice,	map<int, Rtas_Peers*> &en_proceso,	int consenso,	handle<BD> *bd	)
{
	if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "(WSE) Entro a la función Termino_tarea()" << endl;}
	//~ handle<TlcProtocol> *gq;
	//~ handle<ClientTlcWse> *puntero_cliente_wse;			
	if (consenso==1)   {
		en_proceso[indice]->consenso=1;
		if ((this->time() - (en_proceso[indice]->tiempo_inicio) ) >= 2*parametro_TTL ) {
			(*observer)->contador_recepcion5++;
		}
	}else {
		if ( (en_proceso[indice])->reenviada==1 ){
			(*observer)->contador_recepcion6++;
		}
	}
	//Crear mensaje para BD.
	Mje_WSE_BD *mBD;
	en_proceso[indice]->tiempo_finalizacion=this->time();
	Rtas_Peers* rta = new Rtas_Peers();
	rta->copy(en_proceso[indice]);
	mBD = new Mje_WSE_BD(indice,	en_proceso[indice]->id_img,
		en_proceso[indice]->TTL_tarea,	en_proceso[indice]->opciones,
		en_proceso[indice]->prioridad_tarea	,rta);
	//Enviar mensaje a BD.
	busy=true; hold(LATENCIA_BD_TASK);busy=false;
	t_com_wse_bd+=(LATENCIA_BD_TASK);
	//~ if (DEBUG_WSE) {escritura_a_archivo::GetStream() << "Salida hold WSE msje a BD - Tarea:" << indice <<endl;}
	(*bd)->add_request(mBD);
	if ((*bd)->idle() && !(*bd)->get_busy()) 
		(*bd)->activateAfter(current() );
	//Borrar tarea
	en_proceso.erase(indice);
	lista_reenvio.clear();
	if (0) {
		escritura_a_archivo::GetStream() << "(WSE) ================================ Tarea "<< indice << " quitada de 'en_proceso'.============================== " << endl;
		mostrar_tareas_en_proceso(indice);
	}
}

void WSE::mostrar_tareas_en_proceso(int indice)
{
	escritura_a_archivo::GetStream() << "#"<<indice<<". en_proceso (WSE) .size(): " << en_proceso.size() << endl;
	escritura_a_archivo::GetStream() << "#"<<indice<<". en_proceso (WSE): ";
	for(map<int,Rtas_Peers*>::const_iterator it = en_proceso.begin(); it != en_proceso.end(); ++it){
		escritura_a_archivo::GetStream() << it->second->id_tarea <<",";
	}escritura_a_archivo::GetStream() << "\n";
}

void WSE::mostrar_tareas_en_proceso_cout(int indice)
{
	cout << "#"<<indice<<". en_proceso (WSE) .size(): " << en_proceso.size() << endl;
	cout << "#"<<indice<<". en_proceso (WSE): ";
	for(map<int,Rtas_Peers*>::const_iterator it = en_proceso.begin(); it != en_proceso.end(); ++it){
		cout << it->second->id_tarea <<",";
	}cout << "\n";
}

void WSE::acumular_rtas(Rtas_Peers* rta, int id_peer, int indice){

	
	list<int>::iterator it1;	//Con it1 recorro la lista de opciones del mensaje que llega.
	list<int>::iterator it2;	//Con it2 recorro la lista "en proceso"
	int jj=0;
	for (it1=rta->opciones.begin();it1!=rta->opciones.end();it1++){	
		if ((*it1)==1){
			(en_proceso[indice])->votacion[jj].push_back(id_peer);
		}
		jj++;	//Sirve sólo para mostrar qué opción se está sumando
	}
}


int WSE::calcular_consenso(int indice, 
							int peers_reenvio, 
							map<int, Rtas_Peers*> &en_proceso,
							double umbral_votacion, 
							int H,
							MessageWSE* &temp
							)
	{
		int consenso=0;
		if (0) {escritura_a_archivo::GetStream() << "(WSE) Cantidad de opciones votadas en la tarea " <<indice<<": " << (en_proceso[indice])->votacion.size() << endl;}
		//~ en_proceso[indice]->print_rta();
		//Para analizar la tarea que se completó, recorrer las listas de votación y ver cuántos peers votaron.
		for (int i=0; i<(int)(en_proceso[indice])->votacion.size();i++)  {
			if ((int) (en_proceso[indice])->votacion[i].size()>umbral_votacion*H )	{
			//if ((int) (en_proceso[indice])->votacion[i].size()> 7)	{		//Umbral de votación fijo, según análisis de bibliografía para conf de parámetros.
				consenso=1;
				en_proceso[indice]->consenso=1;
			}
		}
		busy=true; hold(CTE_CONSENSO_WSE);busy=false;
		(*observer)->t_consenso_server+=CTE_CONSENSO_WSE;
		if (0) {escritura_a_archivo::GetStream() << "(WSE) Consenso: " << consenso << endl;}
		return consenso;
	}


int WSE::seleccion_peer_agregacion(vector<double> &porc_utilizacion_voluntarios){
	int elegido=0;
	vector<int> top_20;
	encontrar_minimos(porc_utilizacion_voluntarios, top_20, 20);	//Encontrar 20 peers con utilización mínima
	elegido = select_random(top_20);
	return elegido;
}

int WSE::select_random(vector<int> &top_20){
	//Random Seed-------------------------------------------------
	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);
	//------------------------------------------------------------
	std::uniform_int_distribution<int> distribution(0,19);
	int number = distribution(generator);
	//~ cout << "Random int: " << number << endl;
	return number;
}
