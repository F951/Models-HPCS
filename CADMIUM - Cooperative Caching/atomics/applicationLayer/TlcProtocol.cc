#include "TlcProtocol.h"
#include <iostream>
#include <fstream>

void TlcProtocol::inner_body(void)
{
	int MOSTRAR_SALIDA_TLC = 0;
	// ofstream salida_TLC_Protocol;
	// salida_TLC_Protocol.open ("mensajes_sim/salida_TLC_Protocol.txt",ios_base::app);	//CUIDADO, ESTO ES PARA USAR APPEND. VER LA OTRA FORMA.
	//salida_TLC_Protocol.open ("salida_TLC_Protocol.txt");
	
   while (1) 
   {
		while ( flag_terminar==1 ){
			(*replicator)->end();
			passivate();
		}
		if  ((*observer)->get_end()){
				Broadcast=true;
				(*observer)->broadcasts[(*pastry)->ne->getIP()]=1;										
				(*observer)->finalizacion[(*pastry)->ne->getIP()]=1;
				(*replicator)->end();
				//~ cout << "Pasivar Observador, PEER:"<< ((*pastry)->ne)->getIP()<<endl;
				//(*observer)->end();
				(*(this->client))->end_cliente=1;
				passivate();
			}			
	    //if (MOSTRAR_SALIDA_TLC) {cout <<  "PEER:"<< ((*pastry)->ne)->getIP()<< endl;fflush(stdout);}
		//cout << "NUEVO LOOP TLC: "<< ((*pastry)->ne)->getIP() <<endl;
		if(queue_in.empty())     
			passivate();
		// recovers messages from the lower layer
		if (MOSTRAR_SALIDA_TLC) {cout<<"======================= TLCProtocol ======================= PEER:"<< ((*pastry)->ne)->getIP()<< endl; fflush(stdout);}
		if((*observer)->getIteracion() != iteracion)
		{
			//    inlinks.clear();
			//  RCACHE->reset();
			iteracion = (*observer)->getIteracion();
			checkWaitTimeOut();
		}
		//if (MOSTRAR_SALIDA) {cout<<"1: TLCProtocol "<<endl; fflush(stdout);}
		while(!queue_in_lower.empty())
		{
			PastryMessage *temp1 = (PastryMessage*)queue_in_lower.front();
			queue_in_lower.pop_front();

			TlcMessage *temp =(TlcMessage*)temp1->getData();

			if(temp1->finalDestination)
				temp->setFinalDest();

			if(temp1->getType()!=5)
				delete temp1;

			queue_in.push_back(temp);
		}
		//cout << "DEBUG 3 "<< ((*pastry)->ne)->getIP()<<endl;
		ASSERT(!queue_in.empty());

		TlcMessage* tm = queue_in.front();
		//~ cout << "TLC - m->getDataType(): " << tm->getDataType() << endl;
		queue_in.pop_front(); 
		
		// cout << "tm->getKey() antes de algoritmo TLC: " << tm->getKey() << endl;fflush(stdout);
		//~ cout << "tm->getType() antes de algoritmo TLC: " << tm->getType() << endl;fflush(stdout);
		//cout << "DEBUG 4 "<< ((*pastry)->ne)->getIP()<<endl;
		switch ((int)Rep_Strategy)
		{
			case (0):
			{//cout << "DEBUG 1LeafsetBubble "<< ((*pastry)->ne)->getIP()<<endl;
				LeafsetBubble(tm);
				break;
			}
			case (1):
			{//cout << "DEBUG 1 LeafsetBubble"<< ((*pastry)->ne)->getIP()<<endl;
				LeafsetBubble(tm);
				break;
			}
			case (2):
			{  //cout << "DEBUG TlcAlgorithm "<< ((*pastry)->ne)->getIP()<<endl;
				TlcAlgorithm(tm);
				break;
			}
			default:
			{
				// if (MOSTRAR_SALIDA_TLC) {archivo_salida <<  "default " << Rep_Strategy << endl;fflush(stdout);}
			}
		}
	}
}


void TlcProtocol::TlcAlgorithm(TlcMessage * tm)
{	
	int MOSTRAR_SALIDA_TLC = 0;
	int DEBUG_TLC = 0;
	// ofstream salida_TLC_Protocol;
	// salida_TLC_Protocol.open ("mensajes_sim/salida_TLC_Protocol.txt",ios_base::app);
	
	Entry *e;
	LCache *lc;
	Query *q;
	NodeEntry *lcPeer;

	switch (tm->getType())
	{
		case TLC_LC_ACK:
		{
			lc = tm->getLCache();
			lcack.push_back(lc);
			delete tm;
			break;
		}
		
		case TLC_WSE:
		{
			//*****************************************************************************************/
			//Saco la entry "e" del mensaje "tm" que recibo del WSE y la inserto en la Rcaché.
			//Si el Peer recibió este mensaje, es porque se lo pidió al WSE. Esto quiere decir que el Peer es 
			//el responsable de este objeto.
			//Posteriormente recorro outstanding y waiting.
			//Waiting es una lista de ID de objetos. Lo único que hago es borrar la entrada del mensaje que llegó.
			//Es como si tuviera una lista de flags, y sacara el flag de la consulta que recibí del WSE. Es sólo un flag.
			//Outstanding, por otro lado, se recorre hasta que se encuentra el término del objeto que se recibió.
			//Si se encuentra, se clona la entrada "e", y se envía un mensaje al nodo "source" del mensaje que quedó guardado en la lista.
			//Outstanding también puede traducirse como pendiente.
			//Luego, si se cumple una condición, se guarda el Inlink. Esto todavía no lo entiendo bien. 
			//Pareciera ser que, si el ID de este Peer no coincide con el del mensaje, 
			//se enlaza el penúltimo elemento del path con la entrada de caché. Si el ÚLTIMO
			//Elemento del Path es este Peer, el penúltimo es el vecino (con el que hay comunicación directa
			//en una topología de anillo). Por lo tanto, lo que se hace es establecer como Inlink para la 
			//entrada "e", el vecino por el cuál vino el mensaje.
			//*****************************************************************************************/
			e= tm->getEntry();
			//q=tm->getQuery(); 
			if (DEBUG_TLC) {cout <<  "Mensaje con etiqueta TLC_WSE llega al PEER:"<< ((*pastry)->ne)->getIP()<<". TLC: INSERT GLOBAL "<< pid << " - key " << e->key << endl;fflush(stdout);}
			//COMENTADO::::::: 

			RCACHE->insertEntry ( e->clone() );
			//~ vector<Entry*> Vector_aux_entries=RCACHE->getEntries();
			//~ cout << "tareas en caché:" << endl;
			//~ for (int i =0; i<Vector_aux_entries.size();i++){
				//~ cout << Vector_aux_entries[i]->rta->id_tarea << ",";
			//~ }
			//~ cout << endl;		
			
			//~ e->rta->Responsable=(*pastry)->ne;
			//ESTO ES DE DEPURACION, EN EL ORIGINAL NO ESTABA. LO AGREGO PARA ANALIZAR LAS RELACIONES ENTRE HITS Y MISSES DE CACHE, CON LOS OBJETOS QUE LLEGAN DESDE EL WSE LUEGO DE SOLICITARLOS.
			(*observer)->add_objArrival();
			//Itero respecto de las listas de mensajes "outstanding" y "waiting".
			vector<TlcMessage*>::iterator it;	    
			vector< Query*>::iterator at;
			it=outstanding.begin();
			if(0){cout << "TLC_WSE - Recorro outstanding en peer: " << ((*pastry)->ne)->getIP() << ". Muestro mensajes TLC para ver los destinos de las respuestas."<< endl;}
			while(it!=outstanding.end())
			{	//Comparo el hash del mensaje con los de los de la lista
				if(BN_cmp(((*it)->getQuery())->hashValue, e->hashValue)==0)	 //BN_cmp devuelve 0 si los números son iguales.
				{
					TlcMessage* tmp = (*it);
					//~ tmp->print_info();
					Entry* clone = e->clone();
					sendReply(clone,tmp);			//Si coincide, respondo con SendReply() y tmp (es el mensaje de outstanding)
					RCACHE->update(e->hashValue);	//Por qué actualizo el valor en la Rcaché?
					outstanding.erase(it);			//Lo borro de la lista "outstanding"
					if (tmp->getPathSize()>=2)
					{
						if(BN_cmp(((*pastry)->ne)->getNodeID(),(tmp->src)->getNodeID())!=0)
							storeInlink(e->hashValue, tmp->path[tmp->getPathSize()-2]);		//Guardo el recorrido del mensaje
					}
				}
				else
				{
				   it++;	//Cambio it para que lea el próximo mensaje de la lista
				}
			}
			
			at = waiting.begin();
			while(at!=waiting.end())		//Recorro la lista "waiting"
			{
				if(BN_cmp((*at)->hashValue, e->hashValue)==0)	//Si coincide, borro el mensaje de la lista
				{
					waiting.erase(at);		
				}
				else
				{
					at++;
				}
			}

			delete tm;
			break;
		}
		case TLC_LOOKUP:
		{
			//*****************************************************************************************/
			//Saco la query del mensaje "tm" y la pongo en una nueva query "q".
			//Comparo el ID de este Peer con el último nodo que figura en el path del mensaje.
				//Si son distintos, añado el ID de este Peer al Path del mensaje.
			//Luego se lee del mensaje la variable booleana FinalDestination ("it's true when the message has arrived to the responsible peer"-TlcMessage.h)
				//Si es el responsable, le digo al observador cuántos Hops hubo, pasándole el valor de PathSize.
			//Luego de los controles de arriba para Path y Hops, me fijo si el objeto está en la Rcaché.
				//Si está y no es vieja, agrego un HitGlobal (sea o no el responsable), envío la respuesta al Peer que pidió el objeto,
					//actualizo la entrada de al Rcaché (¿Qué hace el update? Actualiza la frecuencia y el timestamp del entry).
					//Luego, si vino de más lejos, guardo el InLink (relaciono el ID de objeto con el vecino de donde vino).
				//Si está y es vieja, o si no está, pero soy el responsable, pongo el objeto en outstanding (lo marco como "pendiente").
					//También agrego un Miss de caché, y le mando un mensaje al WSE pidiendo el objeto.
				//Si no está y no soy el responsable (la última opción),
					//reviso la caché Local (cuidado: no la Lcaché, sino la parte LOCAL de la Rcaché).
					//si la entrada aparece y es vieja, la saco de la Rcaché Local.
					//si no es vieja, añado un hitLocal, envío el objeto al Peer que lo pide, y actualizo la entrada de la Rcaché Local.
				//Si no está tampoco en la caché Local, añado un Miss (CREO QUE ESTE NO ESTABA EN EL ORIGINAL) y reviso la Lcaché (Location caché)
					//Si la entrada está, busco el "DONANTE" (¿Es lo mismo que responsable?). Añado un HitLocal, contesto y dejo 
						//vinculado al DONANTE con la query.
					//Si la entrada no está en la Lcaché, o si el Donante es igual al source del pedido, voy a GOSALIDA
						//En GOSALIDA me fijo si está en "waiting". Si está en waiting, pongo el mensaje en outstanding (pendiente), 
							//agrego un addHitWaiting (esto es para distinguirlo de los hitLocales comunes) y le paso los Hops al observer.
						//Si no está en waiting, sigo pasando el mensaje.
			//Finalmente hago controles de PathSize:
				//Si PathSize==2 (el vecino pidió el objeto), añado una entrada a la Lcaché. También creo un amortiguador (¿Que es? ¿Es una réplica?).
				//Si PathSize==1 (este Peer armó el mensaje pidiendo el objeto) lo agrego a waiting.
	
			//ACÁ ES MUY INTERESANTE, CASI AL FINAL, CUANDO SE RELACIONA WAITING CON OUTSTANDING. WAITING ES UN FLAG, OUTSTANDING ES UNA LISTA DE MENSAJES (QUE PODRIAN SER RESPUESTAS PENDIENTES)
			//*****************************************************************************************/
			q = tm->getQuery();
			if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP() << ". Tarea: " << q->rta->id_tarea << endl;}
			//~ cout << "Tlc: lookup - key:" << q->term;fflush(stdout); cout << " id: " << q->id << endl;fflush(stdout);
			if((tm->path).size()>0)
			{
				if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP() << ". El mensaje pidiendo el objeto de la tarea " << q->rta->id_tarea << " viene de otro peer."<<endl;}
				if(BN_cmp(((*pastry)->ne)->getNodeID(),(tm->path[(tm->path).size()-1])->getNodeID())!=0)
					tm -> addPath((*pastry)->ne);
			}
			else
			{
				if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP() << ".  El mensaje pidiendo el objeto de la tarea " << q->rta->id_tarea << " viene de este peer. Lo añado al Path."<<endl;}
				tm -> addPath((*pastry)->ne);
			}
			if (tm->getFinalDest())
			{
				//if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "PEER:"<< ((*pastry)->ne)->getIP() << " es el responsable de la query:" << q->term << endl;}
				if (DEBUG_TLC) {cout <<  "TLC LOOKUP, el PEER:"<< ((*pastry)->ne)->getIP() << " es el responsable de la query:" << q->rta->id_tarea <<  " pedida por el peer:" << tm->src->getIP() << endl;}
				// cout << "hop2: " << tm->path.size() << endl;
				(*observer)->addHops(tm->path.size()-1);
				//printPath(tm);   
				//Calculate average queue size
				//setAvgQueue();
			}
			// check global cache entries               
			e = RCACHE->check(q->hashValue);
			if(e!=NULL && !e->old(this->time()))
			{
				//cout << "debug2"<< endl;
				if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Solicitante: " <<q->src->getIP()<<". El objeto está en la Rcache Global"<<endl;}
				//~ e->rta->print_rta(); 
				//~ tm->rta->print
				(*observer)->addHitGlobal();     
				sendReply(e->clone(), tm );
				RCACHE->update(e->hashValue);
				if (tm->getPathSize()>=2)
				{
					if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Solicitante: " <<q->src->getIP()<<". PathSize=" << tm->getPathSize() << ", mayor o igual que 2. Guardar inlink en este PEER."<<endl;}
				  if(BN_cmp(((*pastry)->ne)->getNodeID(),(tm->src)->getNodeID())!=0)
				   storeInlink(q->hashValue, tm->path[tm->getPathSize()-2]);
				}
			}
			// if it is the final Dest or it the entry is old.
			else if( (e==NULL && tm->getFinalDest()) || (e!=NULL && e->old(this->time())) )
			{ //cout << "DEBUG TLC_LOOKUP 5: "<< ((*pastry)->ne)->getIP()<<endl;
				//if(e!=NULL)
				 // cout << "TTL: "<< q->id <<  endl;
				 if( (e==NULL && tm->getFinalDest())) {if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Solicitante: " <<q->src->getIP()<<". No está y soy el destino final."<<endl;}}
				 if( (e!=NULL && e->old(this->time()))) {if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Solicitante: " <<q->src->getIP()<<". Está en la global pero es vieja."<<endl;}}
				if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Solicitante: " <<q->src->getIP()<<". Añadir a outstanding."<<endl;}
				//CONTROL DE OUTSTANDING (FER)...........................................................
				int pedir = 1;
				for (int i =0; i<(int)outstanding.size();i++){	
					if (q->rta->id_tarea== outstanding[i]->query->rta->id_tarea ){
						pedir=0;
					}
				}//...........................................................................
				outstanding.push_back(tm);
				(*observer)->addMiss_global();
				 e = tm->getEntry();
				(*(*client)->wse)->consultas_enviadas_desde_tlc++;
				//~ cout << "Tarea e: " << e->rta->id_tarea <<endl;    //da violacion de segmento
				if (DEBUG_TLC) {cout << "Tarea " <<  q->rta->id_tarea <<". Tamaño waiting: " << waiting.size() << ". tamaño outstanding: " << outstanding.size() << endl;}
				//CONTROL DE OUTSTANDING (FER)...........................................................
				if (pedir==1){
					if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Solicitante: " <<q->src->getIP()<<". Pedir tarea al WSE." <<endl;}
					(*client)->sendWSEQuery(q);
					(*observer)->N_ask_obj++;
				}
				else
				{
					if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Solicitante: " <<q->src->getIP()<<". La consulta ya está en outstanding." <<endl;}
				}
				//...........................................................................
			}
			else 
			{
				// check local cache entries
				e= localCache->check(q->hashValue);
				if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Solicitante: " <<q->src->getIP()<<". Busco en la Rcaché local."<<endl;}
				if(e!=NULL && e->old(this->time()))
				{
					if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Está en la Rcaché local, pero es vieja. " << endl;}
					localCache->remove(e);
					e=NULL;
				}
				if (e!=NULL)
				{
					if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<".  Está en la Rcaché local." << endl;}
					(*observer)->addHitLocal();
					// send reply message and update entry
					if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Enviar respuesta presente en la Rcache Local." <<endl;}
					//~ e->rta->print_rta();
					sendReply(e->clone(),tm);
					localCache->update(e->hashValue);
				}
				else 
				{ 	
					if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". No está en la Rcaché global ni local. "<< endl;}
					(*observer)->addMiss_local();
					// check Lcache
					if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Busco en la Lcache."<<endl;}
					lc = LCACHE->check(q->hashValue);
					// if found add src to Lcache if expiration is ok
					if ( lc!=NULL)
					{
						if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Está en la Lcache. Pido Donante"<<endl;}
						lcPeer = lc->getDonante(q->src, (double)this->time() );
						if ( lcPeer !=NULL && lcPeer != q->src)//if ( lc !=NULL && lc->getLocation()!=q->src)
						{
							// if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "TLC LOOKUP, query:" << q->term <<". No está en la Rcaché global ni local. Está en la Lcaché. PEER:"<< ((*pastry)->ne)->getIP() << endl;}
							(*observer)->addHitLCache();
							if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". El donante existe y no es quien envió la query."<<endl;}
							lc->addNewLocation(lcPeer,this->time());
							if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Envío con sendLcache."<<endl;}
							sendLCache(lc->clone(),tm);
							/*if(q->src!=(*pastry)->ne)
							{
								lc->addNewLocation(q->src,this->time());
							}*/
						}
						else goto GOSALIDA;           
					}
					else
					{ 	
					GOSALIDA:
						if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". GOSALIDA."<<endl;}
						vector<Query*>::iterator ite;
						ite=waiting.begin();
						bool wait=false;
						bool force=false;
						if (tm->getForceRouting())
						{
							force=true;
							if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Tiene force_routing."<<endl;}
						}
						if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Busco en waiting si no tiene force_routing."<<endl;}
						while(ite!=waiting.end() && !wait && !force)
						{
							if( BN_cmp(q->hashValue, (*ite)->hashValue)==0)
							{	
								(*observer)->addHitWaiting();
								//cout << "hop3: " << tm->path.size() << endl;
								(*observer)->addHops(tm->path.size()-1);
								//printPath(tm);
								//cout << "outstanding q:" << q->id << " wait " << (*ite)->id << endl;
								outstanding.push_back(tm);
								wait=true;
								if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Está en waiting."<<endl;}
								break;
							}
							ite++;
						}
						if(!wait)
						{
							if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Continue Routing."<<endl;}
							continueRouting(tm);
						}
						
						if (tm->getPathSize() == 2  )
						{ 	
							//cout<< "Insert Lcache : << " << BN_bn2hex(q->hashValue) << " id:" << q->id << " src: " << BN_bn2hex(q->src->getNodeID()) << endl;
							if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". PathSize==2. Inserto en Lcaché. Routing."<<endl;}
							//NodeEntry* nextStep = (*pastry)->route(q->hashValue);
							if(q->src!=(*pastry)->ne)
							{	
								//cout << "HASH route1 " << q->hashValue << endl;
								//cout << "route1 " << BN_bn2hex(q->hashValue) << endl;
								fflush(stdout);
								LCache *temp = new LCache(q->term, 
													   q->hashValue,
													   q->src,
													   (*pastry)->route(q->hashValue),
													   this->time());
								temp->CreateAmortiguador(Thao);
								temp->getDonante(q->src, (double)this->time());
								LCACHE->insertLCache(temp);
							}	
						}
					}
					if (tm->getPathSize()==1)
					{
						//cout << "Inserting in waiting path==1" << endl;		
						if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". PathSize==1. Inserto en waiting."<<endl;}     
						waiting.push_back(tm->query);
					}
				}
				if (DEBUG_TLC) {cout <<  "TLC LOOKUP,  PEER:"<< ((*pastry)->ne)->getIP()<< ". tarea: "<<q->rta->id_tarea<<". Salgo de TLC_LOOKUP."<<endl;}
			}
			break;
		}    
		case TLC_ENDS:
		{	
			nFin++;
			(*observer)->nFin[(*pastry)->ne->getIP()]=nFin;			
			if ((*observer)->get_end()){
				Broadcast=true;
				(*observer)->broadcasts[(*pastry)->ne->getIP()]=1;										
				sendEndBroadcast();
				(*observer)->finalizacion[(*pastry)->ne->getIP()]=1;
				(*replicator)->end();
				//~ cout << "Pasivar Observador, PEER:"<< ((*pastry)->ne)->getIP()<<endl;
				(*observer)->end();
			}			
			//if (MOSTRAR_SALIDA_TLC) {cout <<  "TLC_ENDS - nFin: " << nFin << " - NP: " << NP <<" - ID_PEER: " << (*pastry)->ne->getIP() << endl;}
			//if (nFin == NP)
			//{
			//	if (MOSTRAR_SALIDA_TLC) {cout << "TLC_ENDS FINAL - nFin: " << nFin << " - NP: " << NP << endl;}
			//	(*observer)->finalizacion[(*pastry)->ne->getIP()]=1;
			//	(*replicator)->end();
			//	//cout << "Pasivar Observador, PEER:"<< ((*pastry)->ne)->getIP()<<endl;
			//	(*observer)->end();
			//	//salida_TLC_Protocol.close();
			//}
			//(*observer)->terminadas[(*pastry)->ne->getIP()]=terminadas;						
			////if (terminadas!=ends[NP]){
			//	//if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "T: " << terminadas << " end[NP]: " << ends[NP] << " ends[i] "  << ends[((*pastry)->ne)->getIP()] <<". nFin: " << nFin << ". NP: " << NP << endl;}
			////}
			//if (ends[NP]==1 && !Broadcast)
			//{
			//	if (terminadas!=ends[NP]){
			//		// if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "T: " << terminadas << " ends[i]: "  << ends[((*pastry)->ne)->getIP()] << ". PEER:"<< ((*pastry)->ne)->getIP() << ". end[NP]: " << ends[NP] <<". nFin: " << nFin << ". NP: " << NP << endl;}
			//	}
			//	if(0==ends[(*pastry)->ne->getIP()] || terminadas==ends[(*pastry)->ne->getIP()] )
			//	{
			//		// if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "send BCD -" << (*pastry)->ne->getIP() << endl;}
			//		// cout <<  "send BCD -" << (*pastry)->ne->getIP() << endl;
			//		Broadcast=true;
			//		(*observer)->broadcasts[(*pastry)->ne->getIP()]=1;										
			//		sendEndBroadcast();
			//	}
			//}
			break;
		}

		case TLC_REPLY:
		{    
			//*****************************************************************************************/
			//Si llego acá, es porque recibo el objeto de un Peer que me lo envió desde TLC_LOOKUP (Ya sea porque lo encontró en la Rcaché o Lcaché, o porque lo pidió al WSE o a otro Peer).
			//Al recibir el objeto, hago el trabajo (esto ya es código de crowdsourcing).
			//Luego contesto al WSE, enviando primero un mensaje al clienteWSE. (Código de crowdsourcing). 
				//¿Siempre recibo y hago el trabajo? ¿Está bien esto?
				//Se me ocurre que quedaría mejor moviendo esto al clienteWSE, que es una capa superior. Así queda mejor delimitada cada cosa.
			//Luego recorro la lista de waiting, y si está la entrada, la borro.
			//Luego recorro la lista de outstanding. Si la query está en outstanding, envío la respuesta al nodo que corresponda.
				//Esto es para el caso de que este Peer haya hecho de intermediario. ¿LAS DE OUTSTANDING SIEMPRE SON PARA CONTESTAR A OTRO?
			//Luego recorro la lista de LC acknowledge, y si la entrada está, la borro.
			//Después aumento el contador de QueriesIn (Queries que son enviadas correctamente a los usuarios de los Peers),
			//y envío al observador la latencia de la consulta.
			//Si es la última consulta, respecto de las que creó el generador para este Peer, envío una señal para finalizar la sim.
			//*****************************************************************************************/
			
			if (MOSTRAR_SALIDA_TLC) {cout << "Mensaje con etiqueta TLC_REPLY llega al peer "<< ((*pastry)->ne)->getIP()<< " desde el peer: " << (*observer)->codigos[(tm->src)->getNodeID()]<< endl;}
			//list<int> opciones;
			e = tm->getEntry();
			q= tm->getQuery(); 
			//cout << "DEBUG TLC_REPLY 1 "<< ((*pastry)->ne)->getIP()<<endl;
			//    if(e == NULL)
			//cout << "TLC: Entry received NULL" << endl;

			// NO SIRVE CREO // if (MOSTRAR_SALIDA_TLC)cout << "TLC: INSERT LOCAL "<< UTIL::truncateNodeId(pid) << " - key " << UTIL::truncateNodeId(e->hashValue) << endl;fflush(stdout);
			if (MOSTRAR_SALIDA_TLC) {cout << "PEER:"<< ((*pastry)->ne)->getIP()<<". TLC: INSERT LOCAL "<< pid << " - e->key " << e->key << " - q->term " << q->term<< " - e->id_imagen " << e->id_imagen << endl;fflush(stdout);}		
			if (MOSTRAR_SALIDA_TLC) {cout << "==================================== DEBUG TLC_REPLY 2  - MENSAJE TIPO WSE HACIA LA CAPA SUPERIOR (ClientTlcWSE) PEER: "<< ((*pastry)->ne)->getIP()<<endl;}
			//~ MessageWSE* respuesta_al_wse = new MessageWSE(((*client)->get_ClientWSE_obj()), e->hashValue,e->key, 0, e->TTL_tarea, e->opciones,e->prioridad_tarea, 2, e->id_imagen,(tm->src)->getNodeID());
			//~ respuesta_al_wse->tag=2;
			//~ //cout << "Creo mensaje para el WSE en TLC:"<<endl;
			//~ (*(*client)->get_ClientWSE_obj())->receiveWSE(respuesta_al_wse);
			//~ if( (*(*client)->get_ClientWSE_obj())->idle() && !(*(*client)->get_ClientWSE_obj())->get_busy() )
				//~ (*(*client)->get_ClientWSE_obj())->activateAfter( current() );
			
			//~ cout << "Le doy al constructor el id de la imagen que está en la rta de la entry: " << e->rta->id_img << endl;
			Rtas_Peers *rta = new Rtas_Peers();
			rta->copy(e->rta);
			rta->id_img=e->rta->id_img;
			rta->Responsable_agreg=e->rta->Responsable_agreg;
			MessageWSE* respuesta_al_peer = new MessageWSE(((*client)->get_ClientWSE_obj()), e->hashValue,e->key, 0, e->TTL_tarea, 2, e->rta->id_img,rta,(tm->src));
			//cout << "Creo mensaje para el WSE en TLC:"<<endl;
			(*(*client)->get_ClientWSE_obj())->receiveWSE(respuesta_al_peer);
			if( (*(*client)->get_ClientWSE_obj())->idle() && !(*(*client)->get_ClientWSE_obj())->get_busy() )
				(*(*client)->get_ClientWSE_obj())->activateAfter( current() );
			//cout << "MENSAJE ENVIADO:"<<endl;
			// save in localCache
			//COMENTADO::::::: 
			localCache->insertEntry(e->clone());


			vector<Query*>::iterator at;
			at= waiting.begin();


			while(at!=waiting.end())
			{ 
				if(BN_cmp((*at)->hashValue, e->hashValue)==0)
				{  
				   waiting.erase(at); 
				} 
				else
				{
					at++;
				}
			}
			vector<TlcMessage*>::iterator it1;

			it1 = outstanding.begin();
			while(it1!=outstanding.end())
			{
				if(BN_cmp(e->hashValue, ((*it1)->getQuery())->hashValue)==0)
				{
					//(*observer)->addHitLocal();
					//cout << "TLC: REPLY OUTSTANDING"<< ((*it1)->getQuery())->id << endl;
					TlcMessage* tmp = *it1;
					sendReply(e->clone(),tmp);
					outstanding.erase(it1); 
				}
				else
				{
					it1++;
				}
			} 
			vector<LCache*>::iterator ilc;
			ilc= lcack.begin();
			while(ilc!=lcack.end())
			{
				if(BN_cmp(e->hashValue, (*ilc)->hashValue)==0)
				   lcack.erase(ilc);
				else
					ilc++;
			}

			//Query *q = tm->getQuery();
			//  cout << "Arriving Query " << q->id <<"/" << this->time()  << endl;
			(*observer)->addNQueriesIn();
			(*observer)->addLatencia((double)((this->time())-(q->t_start)));
			
			terminadas++;
			(*observer)->terminadas[(*pastry)->ne->getIP()]=terminadas;			
			
			//~ if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "T: " << terminadas << " ends[NP]: " << ends[NP] << " ends[i] "  << ends[((*pastry)->ne)->getIP()] << " -  PEER:"<< ((*pastry)->ne)->getIP() << endl;fflush(stdout);}

			if (ends[NP]==1)
			{
				// if (MOSTRAR_SALIDA) cout<<"DEBUG ENDS--------------------------------------------------"<<endl;
				if(terminadas==ends[(*pastry)->ne->getIP()])
				{
					//~ if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "send BDC - PEER:" << (*pastry)->ne->getIP() << endl;}
					// if (MOSTRAR_SALIDA) cout<< "send BDC - PEER:" << (*pastry)->ne->getIP() << endl;
					// if (MOSTRAR_SALIDA) {cout<<"END Broadcast desde PEER:"<< (*pastry)->ne->getIP() <<endl;}
					Broadcast=true;
					(*observer)->broadcasts[(*pastry)->ne->getIP()]=1;								
					sendEndBroadcast();
				}
				else 
				{
					// if (MOSTRAR_SALIDA){cout<<"DEBUG ENDS 2--xxxxxxxxxxxxxxxxxxxxx------------------------"<<endl;}
				}
			}
			delete tm->getQuery();
			delete tm;
			//cout << "SALGO DEL NODO TLC"<<endl;
			break;
		} 

		case TLC_LCACHE:
		{	
			//*****************************************************************************************/
			//Añado al Path este Peer.
			//Armo un puntero a la Lcaché y saco la query del mensaje.
			//Luego me fijo si la query está en la Rcaché LOCAL.
				//Si no está o si es vieja, reviso si está en la lista waiting o lcack (es decir, compruebo flags).
				//Entonces me fijo si está en la Lcaché. Si está, activo un flag.
			
				//Si está en waiting, y no está en la lista lcack, ni en la Lcaché, la pido a la red con TLC_LOOKUP, la agrego a outstanding y le agrego un Hop.
				//Si no está en waiting o está en alguna de las Lcaché, lo envío ahí.
			//Si no se cumple lo anterior, quiere decir que está en la Rcaché Local, por lo que agarro la entrada y contesto al source.
			//*****************************************************************************************/

			//cout << "DEBUG 10 "<< ((*pastry)->ne)->getIP()<<endl;
			//ADD TO PATH
			if((tm->path).size()>0)
			{
				if(BN_cmp(((*pastry)->ne)->getNodeID(),(tm->path[(tm->path).size()-1])->getNodeID())!=0)
					tm -> addPath((*pastry)->ne);
			}
			else	
			{		
				tm -> addPath((*pastry)->ne);
			}

			//tm -> addPath((*pastry)->ne);

			lc = tm->getLCache();
			q = tm->getQuery(); 

			//~ if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "TLC: LCACHE RECEIVED -" << q->id << " key " << BN_bn2hex(q->hashValue)<< endl;fflush(stdout);}
			Entry* tmp = localCache->check(q->hashValue);

			if(tmp == NULL || tmp->old(this->time()) )
			{
				//cout << "not in local cache?" << endl;
				vector<Query*>::iterator at;
				bool wait = false;
				at = waiting.begin();
				while(at!=waiting.end())
				{
					if(BN_cmp((*at)->hashValue, q->hashValue)==0)
					{
						//cout << "ID wait"<< (*at)->id  << endl;
						wait=true;
						break;
					}
					at++;
				}
				bool isLcack= false;

				vector<LCache*>::iterator ilc;
				ilc= lcack.begin();
				while(ilc!=lcack.end())
				{
					if(BN_cmp(q->hashValue, (*ilc)->hashValue)==0)
					{   
						isLcack=true;
						break;
					}
					ilc++;
				}
				bool isLack2=false;
				LCache* lc_temp = LCACHE->check(q->hashValue);
				if (lc_temp!=NULL)
				{
					if (BN_cmp(((lc_temp->getLocation())->getNodeID()), (q->src)->getNodeID())==0)
					{
						isLack2=true;
					}
				}
				if(wait && !isLcack && !isLack2)
				{
					//COMENTADO:: 
					(*observer)->addHitLocal();	//DEBERIA SER HitLcaché???
					//		  cout << "Hops:" << tm->path.size() << endl;
					(*observer)->addHops(tm->path.size()-1);
					tm->setType(TLC_LOOKUP);
					outstanding.push_back( tm);

					  //printPath(tm);
				}
				else // 
				{
					continueRouting(tm, lc->getNextStep());
					delete lc;
				}
			}
			else 
			{
				//cout << "replying" << endl;
				(*observer)->addHitLocal();
				//	        cout << "Local 2-" << q->id << endl;
				sendReply(tmp->clone(), tm);
				localCache->update(tmp->hashValue);
			}
			//delete lc;
			break;
		}	
		
		case TLC_REPLICATE:
		{	
			//*****************************************************************************************/
			//Acá simplemente me llega una query y la pido al WSE.
			//Buscar diferencia con otra opción del case que también pida la consulta al WSE.
				//Se usa en TLC_LOOKUP, acá, y también en LeafsetBubble.
			//Si me llega este mensaje, tengo que pedir al WSE el objeto... Y ahí ver si lo guardo en alguna memoria mía.
			//La replicación en este trabajo consiste en replicar la responsabilidad. 
			//¿Implica esto que al recibir el objeto, lo guardo en la Rcaché global de este Peer?
			//Claro, porque en las líneas que siguen acá abajo, le estamos mandando un mensaje al WSE
			//como si nosotros fuéramos el responsable de ese objeto.
			//*****************************************************************************************/
			
			//cout << "DEBUG 11 "<< ((*pastry)->ne)->getIP()<<endl;
			// add entry to replicaCache
			if (DEBUG_TLC) {cout << "TLC-REPLICATE Received!!"<<endl;	}
			e = tm->getEntry();
			//if(e->hashValue == NULL)
			//cout << "ENTRY NULL!!"<<endl;
			//cout << "TLC-REPLICATE recovered!! " << BN_bn2hex(e->hashValue) <<endl;
		    (*(*client)->wse)->consultas_enviadas_desde_tlc++;
			//(*client)->sendWSEQuery( new Query(0, (char*)(e->key).c_str(),BN_dup( e->hashValue), this->time(),e->TTL_tarea,e->opciones,e->prioridad_tarea));
			//~ busy=1;hold(LATENCIA_PEER_WSE);busy=0;
			(*client)->sendWSEQuery( new Query(0, (char*)(e->key).c_str(),BN_dup( e->hashValue), this->time(),e->TTL_tarea,e->rta));
			(*observer)->N_ask_replica++;
			delete tm;
			//freeMessage(tm);
			break;
		}
		
		case TLC_SEND_RESP:
		{
			//*****************************************************************************************/
			//Recibo este mensaje desde el cliente del mismo nodo, y tengo que enviarlo
			//con algo parecido a sendReply, hacia el responsable.
			//El ID del responsable tiene que estar en un formato determinado.
			//*****************************************************************************************/
			Entry* e= tm->getEntry();
			//q=tm->getQuery(); 
			//if (MOSTRAR_SALIDA_TLC) {cout <<  "Mensaje con etiqueta TLC_WSE llega al PEER:"<< ((*pastry)->ne)->getIP()<<". TLC: INSERT GLOBAL "<< pid << " - key " << e->key << endl;fflush(stdout);}
			//~ Entry* clone = e->clone();
			//~ cout << "====================================================================="<< endl;
			//~ cout << "(Nodo TLC) TLC_SEND_RESP - ENVIAR DESDE EL TLC DE " << (*pastry)->ne->getIP() << " LA RESPUESTA AL RESPONSABLE: " <<  (tm->destNode)->getIP() << endl;
			//~ cout << "(Nodo TLC) TLC_SEND_RESP - Peer: "<<  (*pastry)->ne->getIP()  << ". Fuente en tm: " << (tm->src)->getIP() << ". Destino en tm: " <<  (tm->destNode)->getIP() << endl;
			if (0) {cout << "(Nodo TLC) TLC_SEND_RESP - ENVIAR DESDE EL TLC DE " << (*pastry)->ne->getIP() << " LA RESPUESTA AL RESPONSABLE: " <<  (tm->entry->rta->Responsable_agreg)->getIP() << endl;}
			if (0) {cout << "(Nodo TLC) TLC_SEND_RESP - Peer: "<<  (*pastry)->ne->getIP()  << ". Fuente en tm: " << (tm->src)->getIP() << ". Destino en tm: " <<  (tm->entry->rta->Responsable_agreg)->getIP() << endl;}
			//~ cout << "====================================================================="<< endl;
			sendReply(e->clone(), tm);			//Si coincide, respondo con SendReply() y tmp (es el mensaje de outstanding)
			break;
		}
		
		case TLC_AGGREGATE:
		{
			//*****************************************************************************************/
			//Recibo este mensaje desde el cliente del mismo nodo, y tengo que enviarlo
			//con algo parecido a sendReply, hacia el responsable.
			//El ID del responsable tiene que estar en un formato determinado.
			//*****************************************************************************************/
			//~ cout << "(Nodo TLC) ::::::::::      LA RESPUESTA LLEGA AL RESPONSABLE. CALCULAR VOTACION " <<endl;
			//~ cout << "(Nodo TLC) ::::::::::      MOSTRAR OPCIONES VOTADAS Y PEER QUE CONTESTÓ " <<endl;
			//~ cout << "(Nodo TLC) El peer " << (tm->src)->getIP() << " respondió con las opciones: " << endl;
			//~ for (int i =0; i<tm->entry->opciones.size();i++){cout << tm->entry->opciones.front() << ",";tm->entry->opciones.pop_front();}
			//~ cout << "\n";
			//~ cout << "A la tarea: " << tm->entry->key << " correspondiente a la imagen: " << tm->entry->id_imagen << endl;
			//~ cout << "(Nodo TLC) ::::::::::      PASAR HACIA EL CLIENTE DE ESTE NODO TLC EL MENSAJE PARA QUE AGREGUE LA RESPUESTA" <<endl;
			//~ cout << "(Nodo TLC) ::::::::::      HACERLO COMO EN TLC_REPLY" <<endl;
			//~ cout << "====================================================================="<< endl;
			
			//if (0) {cout << "(Nodo TLC) Mensaje con etiqueta TLC_REPLY llega al peer "<< ((*pastry)->ne)->getIP()<< " desde el peer: " << (*observer)->codigos[(tm->destNode)->getNodeID()]<< endl;}
			if (0) {cout << "(Nodo TLC) Mensaje con etiqueta TLC_REPLY llega al peer "<< ((*pastry)->ne)->getIP()<< " desde el peer: (tm->destNode)->getIP(): " << (tm->destNode)->getIP() << ". (tm->src)->getIP():"<< (tm->src)->getIP() << endl;}
			e = tm->getEntry();
			q= tm->getQuery(); 
			//MessageWSE* respuesta_al_peer = new MessageWSE(((*client)->get_ClientWSE_obj()), e->hashValue,e->key, 0, e->TTL_tarea, e->opciones,e->prioridad_tarea, 3, e->id_imagen,(tm->src));
			MessageWSE* respuesta_al_peer = new MessageWSE(((*client)->get_ClientWSE_obj()), e->hashValue,e->key, 0, e->TTL_tarea, 3, e->id_imagen, e->rta,(tm->src));
			//~ MessageWSE* respuesta_al_peer = new MessageWSE(((*client)->get_ClientWSE_obj()), e->hashValue,e->key, 0, e->TTL_tarea, 3, e->id_imagen, e->rta,e->rta->Responsable);
			respuesta_al_peer->tag=3;
			//cout << "Creo mensaje para el WSE en TLC:"<<endl;
			(*(*client)->get_ClientWSE_obj())->receiveWSE(respuesta_al_peer);
			if( (*(*client)->get_ClientWSE_obj())->idle() && !(*(*client)->get_ClientWSE_obj())->get_busy() )
				(*(*client)->get_ClientWSE_obj())->activateAfter( current() );
			break;
			
			
	
			
		}
		//~ cout <<"(TLC) Salgo del algoritmo TLC" << endl;
		
	}
}

void TlcProtocol::sendEndBroadcast(){

   /**** TlcMessage *EB= new TlcMessage(TLC_ENDS, msgID);
	EB->setDataType((DataType)(27));
	
   PastryMessage *pm = new PastryMessage(PASTRY_BDC, EB);
	pm->setDataType((DataType)(27));
	
   (*pastry)->add_message(pm);
   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());
     *****/
}


void TlcProtocol::printPath(TlcMessage* tm)
{
	int MOSTRAR_SALIDA_TLC=0;
   Query* q  = tm->getQuery();
   //ORIGINAL:::::: const char* s = UTIL::myBN2HEX( q->hashValue);
   //~ const char* s = BN_bn2hex(q->hashValue);
   //ORIGINAL::::::  if (MOSTRAR_SALIDA){cout<< "--------- PATH for " << s << " ---------"<<endl;fflush(stdout);}
   if (MOSTRAR_SALIDA_TLC){cout<< "--------- PATH for " << q->hashValue << " ---------"<<endl;fflush(stdout);}
  // free((char*)s);
   for(unsigned int k = 0; k < tm->path.size(); k++)
   {
      //ORIGINAL:::::: const char* t = UTIL::myBN2HEX((tm->path[k])->getNodeID()) ;
      //ORIGINAL:::::: const char* t = BN_bn2hex((tm->path[k])->getNodeID()) ;
      
      //ORIGINAL:::::: if (MOSTRAR_SALIDA){cout << k << " -> " <<  t <<endl;fflush(stdout);}
      //if (MOSTRAR_SALIDA){cout << k << " -> " <<  (tm->path[k])->getNodeID() <<endl;fflush(stdout);}
      if (MOSTRAR_SALIDA_TLC){cout << k << " -> " <<  (tm->path[k])->getIP()  <<endl;fflush(stdout);}
     // free((char*)t);
   }
   //(*observer)->addHops(tm->path.size()-1);
   cout<<"--------------------------------------------------------------------"<<endl;fflush(stdout);

    
}


// TODO
void TlcProtocol::continueRouting(TlcMessage* tm, NodeEntry* neD)
{
    //cout << "Tlc: continueRouting LCACHE " << endl;
    tm->setType(TLC_LOOKUP);
    tm->dest = (tm->getQuery())->hashValue;

    PastryMessage *pm = new PastryMessage(PASTRY_SENDDIRECT,(*pastry)->ne, neD, tm);
    pm->setDataType((DataType)tm->getDataType());    
      
    (*pastry)->add_message(pm);
    if((*pastry)->idle() && !(*pastry)->get_busy())
       (*pastry)->activateAfter(current());
      
   
}


void TlcProtocol::continueRouting(TlcMessage* tm)
{
 //cout << "Tlc: continueRouting " << endl;

   PastryMessage *pm = new PastryMessage(PASTRY_SENDDHT,(*pastry)->ne,tm->dest, tm);  
   pm->setDataType((DataType)tm->getDataType());   
   
   (*pastry)->add_message(pm);
   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());

}


void TlcProtocol::sendLCache(LCache* lc, TlcMessage* tm)
{
    
	TlcMessage* reply = new TlcMessage(TLC_LCACHE, msgID, (*pastry)->ne, (lc->getLocation()));
	reply->setDataType(SOLICITUD_OBJETO);

	msgID = msgID +1;
	reply->setLCache(lc->clone());
	reply->setQuery(tm->getQuery());
	reply->setPath(tm->path);

	PastryMessage *pm = new PastryMessage( PASTRY_SENDDIRECT,
									  (*pastry)->ne,
				  lc->getLocation(), 
				  reply);
	pm->setDataType(SOLICITUD_OBJETO);
				  
	(*pastry)->add_message(pm);
	if((*pastry)->idle() && !(*pastry)->get_busy())
	(*pastry)->activateAfter(current());
	// delete tm;

	TlcMessage* lc_ack = new TlcMessage(TLC_LC_ACK, msgID, (*pastry)->ne,(tm->getQuery())->src );
	lc_ack->setDataType(SOLICITUD_OBJETO);
    
	msgID = msgID +1;
	lc_ack->setLCache(lc->clone());
	lc_ack->setQuery(tm->getQuery());
	lc_ack->setPath(tm->path);

	PastryMessage *pm2 = new PastryMessage( PASTRY_SENDDIRECT,
								   (*pastry)->ne,
					   (tm->getQuery())->src,
						lc_ack);
	pm2->setDataType(SOLICITUD_OBJETO);
						
	(*pastry)->add_message(pm2);
	if((*pastry)->idle() && !(*pastry)->get_busy())
	(*pastry)->activateAfter(current());

}

void TlcProtocol::sendReply(Entry* _e, TlcMessage* tm)
{
	int MOSTRAR_SALIDA_TLC=0;
	// ofstream salida_TLC_Protocol;
	// salida_TLC_Protocol.open ("mensajes_sim/salida_TLC_Protocol.txt",ios_base::app);
	setAvgQueue();
	overloaded++;
	// (*observer)->addNQueries();
	(*observer)->addLoad(((*pastry)->ne)->getIP());
	Query * q= tm->getQuery();
	if (tm->getType()==7){
		//~ if (0) {cout <<  "(Nodo TLC) TLC AGREGATE - PEER: "<< (*pastry)->ne->getIP() << " envía el objeto al peer: " << (tm->destNode)->getIP() << " con la función sendReply() PARA AGREGAR" << endl;fflush(stdout);}
		if (MOSTRAR_SALIDA_TLC) {cout <<  "(Nodo TLC) TLC SEND REPLY PARA AGREGATE - PEER: "<< (*pastry)->ne->getIP() << " envía el objeto al peer: " << (tm->entry->rta->Responsable_agreg)->getIP() << " con la función sendReply() PARA AGREGAR" << endl;fflush(stdout);}
		if (MOSTRAR_SALIDA_TLC) {cout <<  "(Nodo TLC) TLC SEND REPLY - Debería ir al peer: " << (tm->entry->rta->Responsable_agreg)->getIP() <<  endl;fflush(stdout);}
		//if (0) {cout <<  "(Nodo TLC) TLC AGREGATE - PEER: "<< (*pastry)->ne->getIP() << " envía el objeto al peer: " << (tm->src)->getIP() << " con la función sendReply() PARA AGREGAR" << endl;fflush(stdout);}
		TlcMessage* reply = new TlcMessage(TLC_AGGREGATE, msgID, (*pastry)->ne, tm->entry->rta->Responsable_agreg);
		reply->setDataType(TRABAJO_TERMINADO);
		
		msgID = msgID + 1;
		reply->setEntry(_e);
		reply->setQuery(tm->getQuery());
		//~ PastryMessage *pm = new PastryMessage(PASTRY_SENDDIRECT,(*pastry)->ne,tm->destNode, reply);
		PastryMessage *pm = new PastryMessage(PASTRY_SENDDIRECT,(*pastry)->ne,tm->entry->rta->Responsable_agreg, reply);
		pm->setDataType(TRABAJO_TERMINADO);
		
		(*pastry)->add_message(pm);
		if((*pastry)->idle() && !(*pastry)->get_busy())
			(*pastry)->activateAfter(current());
	}else {
		//cout <<  "(Nodo TLC) TLC_REPLY - PEER: "<< (*pastry)->ne->getIP() << ". A quién envío? (q->src)->getIP(): " << (q->src)->getIP() << "(tm->src)->pastry->ne:" << (tm->src)->getIP() << endl ;
		//cout <<  "(Nodo TLC) TLC_REPLY - PEER: "<< (*pastry)->ne->getIP() << ". Envío a (q->src)->getIP(): " << (q->src)->getIP() <<  endl ;
		TlcMessage* reply = new TlcMessage(TLC_REPLY, msgID, (*pastry)->ne, q->src);
		reply->setDataType(OBJETO_OPCIONES);
		
		msgID = msgID + 1;
		reply->setEntry(_e);
		if (MOSTRAR_SALIDA_TLC) {cout << "(Nodo TLC) TLC SEND REPLY (Responsable): Mando objeto al peer que va a trabajar "<< endl;}
		//~ _e->rta->print_rta();
		if (MOSTRAR_SALIDA_TLC) {cout << "(Nodo TLC) Responsable de la tarea según modificación: " << _e->rta->Responsable_agreg->getIP() << endl;}
		if (MOSTRAR_SALIDA_TLC) {cout << "(Nodo TLC) TLC SEND REPLY - PEER: "<< (*pastry)->ne->getIP() << " envía objeto para trabajo a peer de destino: " << q->src->getIP() << ". Tarea:: " << _e->rta->id_tarea << endl;}
		reply->setQuery(tm->getQuery());
		PastryMessage *pm = new PastryMessage(PASTRY_SENDDIRECT,(*pastry)->ne,q->src, reply);
		pm->setDataType(OBJETO_OPCIONES);
		
		(*pastry)->add_message(pm);
		if((*pastry)->idle() && !(*pastry)->get_busy())
			(*pastry)->activateAfter(current());
	}
}

void TlcProtocol::sendReplica(Entry* e ,NodeEntry* nodeEntry)
{
   TlcMessage* replica = new TlcMessage(TLC_REPLICATE, msgID, (*pastry)->ne, nodeEntry);
   replica->setDataType(SOLICITUD_OBJETO);
   
   msgID = msgID+1;
   replica->setEntry(e);
   (*observer)->addReplicas();
   //cout << "addReplicas en TlcProtocol::sendReplica" << endl;
   PastryMessage *pm = new PastryMessage (PASTRY_SENDDIRECT, (*pastry)->ne, nodeEntry, replica);
   pm->setDataType(SOLICITUD_OBJETO);
   
   (*pastry)->add_message(pm);
   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());
}



void TlcProtocol::setAvgQueue()
{
   t_act = this->time() ;
   //cout << "TLC: T-actual "<< t_act<< endl;
   //cout << "TLC: avg_queue " << avg_queue<<endl;
   //fflush(stdout);

   avg_queue = avg_queue + ( (queue_in.size() + 1) *(t_act - t_pas) );
   t_pas = t_act;
  // cout << "TLC: AVG_QUEUE " << avg_queue << endl;
  // fflush(stdout);
}

void TlcProtocol::resetQueue()
{
   avg_queue = 0;
   overloaded=0;
}

bool TlcProtocol::isReplicated(BIGNUM* nodeId, BIGNUM* key)
{
   map<BIGNUM*, Replicas*>::iterator it;
   it = replicaTrack.begin();
  // cout << "TLC-IsReplicated?"<< endl;
   
   while(it != replicaTrack.end())
   {
      if( BN_cmp(it->first,nodeId) == 0 && (it->second)->isContained(key)  )
      {  
         //cout << "Esta Replicado" << endl;
	 return true;
      }
      ++it;
   }
   return false;
}


bool TlcProtocol::isNewReplica(BIGNUM* id)
{
   map<BIGNUM*, Replicas*>::iterator it;
   it = replicaTrack.begin();
   while(it != replicaTrack.end())
   {
     if( BN_cmp(it->first, id) == 0 )
		return false;
     ++it;
   }

   return true;
}




void TlcProtocol::trackReplica(BIGNUM* nodeId, BIGNUM* key)
{
  // bool flag = isReplicated(nodeId, key);
  bool flag = isNewReplica(nodeId);

  //DEBUG
  /*
  cout <<"TLC - Replica Track " <<BN_bn2hex(nodeId) << " "<<BN_bn2hex(key)<< endl;  
  map<BIGNUM*, Replicas*>::iterator it;
  it = replicaTrack.begin();
  cout << "------------ANTES-------------" << endl;
  while(it != replicaTrack.end())
  {
    (it->second)->print();
    //cout <<BN_bn2hex(it->first) << " "<<BN_bn2hex(it->second)<< endl;
    ++it;
  }
  cout <<"--------------------------"<<endl;
  */
  
  //No esta
  flag=0;
  if(flag)
  {
       Replicas* r = new Replicas(key ,nodeId);
       replicaTrack.insert(pair <BIGNUM*, Replicas*>(nodeId, r) );
       replicaTrack[nodeId] = r;

       (*observer)->addReplicas();
		//cout << "addReplicas en TlcProtocol::trackReplica1" << endl;
   //Existe hay q revisar
  }else
  {
      map<BIGNUM*, Replicas*>::iterator it5;
      it5 = replicaTrack.begin();

      while(it5 != replicaTrack.end())
      {
         if(BN_cmp(nodeId,it5->first) == 0)
         {
	   (it5->second)->putReplica(key);
           (*observer)->addReplicas();
           //cout << "addReplicas en TlcProtocol::trackReplica2" << endl;
	 }
	 ++it5;
      }
   }


 //DEBUG  
  /*
  cout <<"TLC - Replica Track "<<BN_bn2hex(nodeId) << " "<<BN_bn2hex(key)<< endl;
   map<BIGNUM*, Replicas*>::iterator it2;
   it2 = replicaTrack.begin();
   cout << "------------DESPUES-------------" << endl;
   while(it2 != replicaTrack.end() )
   {
      (it2->second)->print();
      //cout <<BN_bn2hex(it2->first) << " "<<BN_bn2hex(it2->second)<< endl;
      ++it2;
   }
   cout <<"--------------------------"<<endl;
*/
}


void TlcProtocol::storeInlink(BIGNUM* key, NodeEntry* n)
{
   bool inlinkStored = false;
   Inlink* i;
   map<BIGNUM*,Inlink*>::iterator it;
   it = inlinks.begin();

  // cout <<"-------------------------- I -------------" << endl;
   while(it != inlinks.end())
   {
      //cout<< BN_bn2hex(it -> first)<< endl;
      if(BN_cmp(it->first, n->getNodeID())==0)
      {
	  inlinkStored = true;
          i = it ->second;
	  break;
      }
      ++it;
   }
   //cout << "Storing Inlink -> "<<  inlinkStored <<endl;
   //Existe Inlink
   if( inlinkStored )
   {
      //cout <<"Put Query "<< BN_bn2hex(key)<< endl;
      //Consulta llega por ese inlink?
      i->putQuery(key);

      //Inlink no existe, se inserta y se inserta la query
   }
   else
   {
      //cout <<"New Inlink " << BN_bn2hex(n -> getNodeID() )<< endl;
      Inlink* i2 = new Inlink(n);
      i2->putQuery(key);

      inlinks.insert(pair<BIGNUM*,Inlink*>((n)->getNodeID(), i2));
      inlinks[n->getNodeID()] = i2;
   }
}
/*

//TODO:
void TlcProtocol::checkTTL(BIGNUM* query, BIGNUM* urlsId)
{
   int query_TTL, last_TTL;
   BIGNUM* lastUrlId = urlsID.find(query)->second;

   //STALE ENTRY
   if(BN_cmp(urlsId, lastUrlId )!= 0)
   {
      // TTL to min
      query_TTL  = MIN_TTL;
      
      //Update URLs_ID
      urlsID.erase(urlsID.find(query));
      urlsID.insert(pair<BIGNUM*, BIGNUM*> (query, urlsId));
      
      //Update TTLs
      queryTTL.erase(queryTTL.find(query));
      queryTTL.insert(pair<BIGNUM*,int>(query, query_TTL) );

      //STILL FRESH ENTRY
      }else
      {
         last_TTL = queryTTL.find(query)->second;

	 //Estimate TTL using incremental Function F
	 //query_TTL = last_TTL + F(last_TTL);
	 
	 if(query_TTL > MAX_TTL)
	    query_TTL = MAX_TTL;
	 
	 //Update URLs_ID
	 urlsID.erase(urlsID.find(query));
	 urlsID.insert(pair<BIGNUM*, BIGNUM*> (query, urlsId));
	 
	 queryTTL.erase(queryTTL.find(query)); 
	 queryTTL.insert(pair<BIGNUM*,int>(query, query_TTL) ); 
   }
}

*/

void TlcProtocol::freeMessage(TlcMessage * msg)
{
   free(msg->dest);
   free(msg);
}


void TlcProtocol::EndReplicator()
{ 
	//~ cout<<"End replicator "<<endl;
	(*replicator)->end();
}

void TlcProtocol::checkWaitTimeOut()
{
   vector<Query*>::iterator at;
   at = waiting.begin();
   while(at!=waiting.end())
   {

   
   if((*at)->getTimeOut()< this->time() && !((*at)->getForceRouting()))
      {
         Query *q=(*at);// new Query(at->id,at->term,at->hashValue,0.0);
         q->renewTimeOut(this->time());
	 q->forceRouting();
	 this->add_query(q);
	 waiting.erase(at);
      }
      else
      {
         at++;
      }
   }
}

void TlcProtocol::LeafsetBubble(TlcMessage *tm)
{
	// int MOSTRAR_SALIDA_TLC=1;
	// ofstream salida_TLC_Protocol;
	// salida_TLC_Protocol.open ("mensajes_sim/salida_TLC_Protocol.txt",ios_base::app);

	Entry *e;
	Query *q;

	switch (tm->getType())
	{
		case TLC_WSE:				//Responsible Peer gets object from server
		{
			e= tm->getEntry();
			//q=tm->getQuery();
			//COMENTADO::::::: 
			RCACHE->insertEntry ( e->clone() );
			vector<TlcMessage*>::iterator it;
			vector< Query*>::iterator at;

			it=outstanding.begin();
			while(it!=outstanding.end())
			{
				if(BN_cmp(((*it)->getQuery())->hashValue, e->hashValue)==0)
				{
					TlcMessage* tmp = (*it);
					Entry* clone = e->clone();
					sendReply(clone,tmp);
					RCACHE->update(e->hashValue);
					outstanding.erase(it);

					if (tmp->getPathSize()>=2)
					{
						if(BN_cmp(((*pastry)->ne)->getNodeID(),(tmp->src)->getNodeID())!=0)
						{
							storeInlink(e->hashValue, tmp->path[tmp->getPathSize()-2]);
						}
					}
				}
				else
				{
					it++;
				}
			}
			at = waiting.begin();
			while(at!=waiting.end())
			{
				if(BN_cmp((*at)->hashValue, e->hashValue)==0)
				{
					waiting.erase(at);
				}
				else
				{
					at++;
				}
			}
			delete tm;
			break;
		}
		
		case TLC_LOOKUP:			//Intermediate or final Peer gets Query about an object
		{
			q = tm->getQuery();
			if((tm->path).size()>0)
			{
				if(BN_cmp(((*pastry)->ne)->getNodeID(),(tm->path[(tm->path).size()-1])->getNodeID())!=0)
				{
					tm -> addPath((*pastry)->ne);
				}
			}
			else
			{
				tm -> addPath((*pastry)->ne);
			}
			if (tm->getFinalDest())
			{
				(*observer)->addHops(tm->path.size()-1);
			}
			// check global cache entries               
			e = RCACHE->check(q->hashValue);
			if(e!=NULL && !e->old(this->time()))
			{
				(*observer)->addHitGlobal();
				sendReply(e->clone(), tm );
				RCACHE->update(e->hashValue);
				if (tm->getPathSize()>=2)
				{
					if(BN_cmp(((*pastry)->ne)->getNodeID(),(tm->src)->getNodeID())!=0)
					{
						storeInlink(q->hashValue, tm->path[tm->getPathSize()-2]);
					}
				}
			}
			// if it is the final Dest or it the entry is old.   
			else if( ( tm->getFinalDest()))
			{
				outstanding.push_back(tm);
				(*observer)->addMiss();
				(*client)->sendWSEQuery(q);
			}
			else if (e!=NULL && e->old(this->time()))
			{
				switch ((int)Case_Responsable)
				{
					case (0):
					{
						(*observer)->addMiss();
						RCACHE->remove(e);
						continueRouting(tm);
						break;
					}
					
					case (1):
					{
						outstanding.push_back(tm);
						(*observer)->addMiss();
						(*client)->sendWSEQuery(q);
						break;
					}
				}//switch
			}
			else
			{
				// check local cache entries
				e= localCache->check(q->hashValue);
				if(e!=NULL && e->old(this->time()))
				{
					localCache->remove(e);
					e=NULL;
				}
				if (e!=NULL)
				{
					(*observer)->addHitLocal();
					sendReply(e->clone(),tm);
					localCache->update(e->hashValue);
				}
				else
				{
					// vector<Query*>::iterator ite;
					//  ite=waiting.begin();
					//  bool wait=false;
					//  while(ite!=waiting.end() && !wait)
					//  {
					//   if( BN_cmp(q->hashValue, (*ite)->hashValue)==0)
					//   {
					//      (*observer)->addHitLocal();
					//      (*observer)->addHops(tm->path.size()-1);
					//      outstanding.push_back(tm);
					//      wait=true;
					//      break;
					//   }
					//   ite++;
					//  }
					//  if(!wait )
					//  {
					continueRouting(tm);
					//  }
					(*observer)->addMiss();	//AGREGADO
				}
				if (tm->getPathSize()==1)
				{
					waiting.push_back(tm->query);
				}
			}
			break;
		}
		
		case TLC_ENDS:
		{
			nFin++;
			//if (nFin == NP)
			//{
			//	(*replicator)->end();
			//	(*observer)->end();
			//}
			//if (ends[NP]==1 && !Broadcast)
			//{
			//	if(0==ends[(*pastry)->ne->getIP()] || terminadas==ends[(*pastry)->ne->getIP()] )
			//	{
			//		Broadcast=true;
			//		sendEndBroadcast();
			//	}
			//}
			break;
		}
		
		case TLC_REPLY:					//Peer author of query gets the retrieved object and must send it up to its application layer.
		{
			e = tm->getEntry();
			q= tm->getQuery();
			// COMENTADO::::: 
			localCache->insertEntry(e->clone());

			vector<Query*>::iterator at;
			at= waiting.begin();
			while(at!=waiting.end())
			{
				if(BN_cmp((*at)->hashValue, e->hashValue)==0)
				{
					waiting.erase(at);
				}
				else
				{
					at++;
				}
			}
			vector<TlcMessage*>::iterator it1;
			it1 = outstanding.begin();
			while(it1!=outstanding.end())
			{
				if(BN_cmp(e->hashValue, ((*it1)->getQuery())->hashValue)==0)
				{
					TlcMessage* tmp = *it1;
					sendReply(e->clone(),tmp);
					outstanding.erase(it1);
				}
				else
				{
					it1++;
				}
			}
			(*observer)->addNQueriesIn();
			(*observer)->addLatencia((double)((this->time())-(q->t_start)));
			terminadas++;
			if (ends[NP]==1)
			{
				//if(terminadas==ends[(*pastry)->ne->getIP()])
				//{
				//	Broadcast=true;
				//	sendEndBroadcast();
				//}
			}
			delete tm->getQuery();
			delete tm;
			break;
		}

		case TLC_REPLICATE:
		{
			switch((int)Case_Responsable)
			{
				// ask directly to the WSE
				case (1):
				{
					e = tm->getEntry();
					//~ if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "/// /// /// e->key en algoritmo TLC al generar query: " << e->key<< endl;}
					//~ if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "/// /// /// e->key en algoritmo TLC al generar query: " << (char*)(e->key).c_str()<< endl;}
					//(*client)->sendWSEQuery( new Query(0, (char*)(e->key).c_str(),BN_dup( e->hashValue), this->time(),e->TTL_tarea,e->opciones,e->prioridad_tarea),0 );
					(*client)->sendWSEQuery( new Query(0, (char*)(e->key).c_str(),BN_dup( e->hashValue), this->time(),e->TTL_tarea,NULL));
					break;
				}
				
				// stores entry locally
				case (0):
				{
					//COMENTADO::::::: 
					RCACHE->insertEntry((tm->getEntry())->clone());
					break;
				}
			
				default:
				{ 
					//~ if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "default Tlc rep" << endl;fflush(stdout);}
					break;
				}
			}
			delete tm;
			break;
		}
		
		default:
		{
			//~ if (MOSTRAR_SALIDA_TLC) {salida_TLC_Protocol <<  "error LeafsetBubble" << endl;}
			break;
		}
	}
}

int TlcProtocol::calcular_consenso(int indice, 
							int peers_reenvio, 
							map<int, Rtas_Peers*> &en_proceso_local,
							double umbral_votacion, 
							int H,
							MessageWSE* &temp
							)
	{
		//-------------------------------------------------------------------------------------
		int nro_respuestas=0;
		for (int i=0; i<(int)((en_proceso_local[indice])->votacion.size());i++) {
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
				consenso=1;
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
		MessageWSE* m = new MessageWSE((*client)->I_obj,temp->getQuery(),temp->getKey(),3, temp->TTL_tarea, consenso,temp->id_imagen,rta);
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
	
void TlcProtocol::printLista(std::list<int> & lista,const char* nombre, int indice)
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
void TlcProtocol::acumular_rtas(Rtas_Peers* rta, int id_peer, int indice){

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

void TlcProtocol::diferencias_en_listas(list<int> &lista1, list<int>& lista2, int indice){
	vector <int> diferencias;
	list<int>::iterator it_prueba1,it_prueba2;
	for (it_prueba1=lista1.begin(); it_prueba1!=lista1.end(); ++it_prueba1)
	{
		it_prueba2 = find(lista2.begin(), lista2.end(), *it_prueba1);
		if(it_prueba2 == lista2.end()){
			diferencias.push_back(*it_prueba1);
		}
	}
	(*(*client)->wse)->print_vec(diferencias,"diferencias",indice);	
}
