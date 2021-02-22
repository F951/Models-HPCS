/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* ClientSimple:
* Cadmium implementation of CD++ ClientSimple atomic model
*/

#ifndef __ClientSimple_HPP__
#define __ClientSimple_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include "util.hpp"

#include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"
#include "../data_structures/TLCmessage.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct ClientSimple_defs{
	struct DataOutUp : public out_port<ServerMessage> { };
	struct DataOutDown : public out_port<TLCMessage> { };
	struct DataOutDB : public out_port<ServerMessage> { };
	//~ struct doneOut : public out_port<bool> { };
	struct DataIn : public in_port<ServerMessage> { };
};

template<typename TIME>
class ClientSimple{
	public:
		//Parameters to be overwriten when instantiating the atomic model
		int debug=0;
		int debug_clock=0;
		TIME   preparationTime;
		int ID_numb;
		int nodes;
		NodeEntry* ne;
		int sentQueries;
		int msgID;
		TIME clock1;
		
		// default constructor
		ClientSimple() noexcept{
			preparationTime  = TIME("00:00:00");
			state.estado=0;
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			clock1=TIME("00:00:00");
		}
		 ClientSimple(int _ID_numb, int _nodes, NodeEntry* _ne) noexcept{
			state.estado=0;
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			ID_numb			= _ID_numb;
			state.OutputUp = 0;
			state.OutputDown = 0;	
			nodes = _nodes;	
			state.Processing_time=0;
			ne = _ne;
			clock1=TIME("00:00:00");
		}
		
		// state definition
		struct state_type{
			int estado;
			vector<ServerMessage> Msg_queue;
			vector<ServerMessage> Msg_list_up;
			vector<ServerMessage> Msg_list_DB;
			vector<TLCMessage> Msg_list_down;
			TIME next_internal;
			bool send_data;
			int OutputUp;
			int OutputDown;
			double Processing_time;
			
		}; 
		state_type state;
		
		//ports definition
		using input_ports=std::tuple<typename ClientSimple_defs::DataIn>;
		//~ using output_ports=std::tuple<typename ClientSimple_defs::DataOutUp,typename ClientSimple_defs::DataOutDown,typename ClientSimple_defs::DataOutDB,typename ClientSimple_defs::doneOut>;
		using output_ports=std::tuple<typename ClientSimple_defs::DataOutUp,typename ClientSimple_defs::DataOutDown,typename ClientSimple_defs::DataOutDB>;
		//====================================================================================================================================
		//internal transition
		//====================================================================================================================================
		void internal_transition() {
			//~ assert(state.estado!=0);
			clock1 =  clock1 + state.next_internal;
			switch (state.estado)
			{
				case 0:
				{
					//No se debe llegar hasta acá por el assert
					break;
				}
				case 1:
				{
					if (debug){std::cout << "ClientSimple"<< ID_numb << " - Estado 1 - Internal transition." << std::endl; }
					state.Msg_list_down.clear();
					state.Msg_list_up.clear();
					state.Msg_list_DB.clear();
					if (state.Msg_queue.size()!=0){
						if (debug){std::cout << "ClientSimple"<< ID_numb << " - Voy a estado 1." << std::endl; }
						state.estado=1;
						state.next_internal=preparationTime;
						//~ clock1 = clock1 + state.next_internal;
					}else{
						if (debug){std::cout << "ClientSimple"<< ID_numb << " - Voy a estado 0." << std::endl; }
						state.estado=0;
						state.next_internal=std::numeric_limits<TIME>::infinity();	//Cuánto tiempo voy a estar en el estado 0
					}
					break;
				}
			}
		}
		
		//====================================================================================================================================
		// external transition
		//====================================================================================================================================
		void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
			//std::cout << "Server"<< ID_numb << " - Transicion externa. Estado: "<< state.estado << std::endl; 
			int flag_dataIn=0;
			int aux_source,aux_dest;
			if (debug){std::cout << "ClientSimple_node"<< ID_numb << " - Transicion externa. Estado: "<< state.estado << std::endl; }
			clock1+=e;
			if (debug_clock){cout << "ClientSimple "<< ID_numb << " - ET - Nuevo Clock1: " << clock1 << endl;}

			for( auto &x : get_messages<typename ClientSimple_defs::DataIn>(mbs)){
				//Si llega un mensaje quiere decir que estoy en el estado 3. Acá puedo poner un assert para que, si no es el estado 3, me de un error.
				//~ assert (state.estado!= 0 || state.estado!= 3);		
				if (x.dest==ID_numb){
					if (debug_clock){cout << "ClientSimple "<< ID_numb << " - ET - Nuevo Clock1: " << clock1 << endl;}
					state.Processing_time = process_msg(x);		
					flag_dataIn=1;
				}
				
				//~ Msg_queue.push_back(x);				
			}
			
			if (flag_dataIn==1){
				switch (state.estado)
				{
					case 0:
					{
						state.estado=1;
						state.next_internal=preparationTime;
						//~ clock1 = clock1 + state.next_internal;
						break;
					}
					case 1:
					{
						state.estado=1;
						preparationTime = preparationTime - e;
						state.next_internal=preparationTime;
						assert(state.next_internal >= TIME("00:00:00"));
						break;
					}
				}
			}
			else
			{
				if (state.estado==0){
					state.next_internal=std::numeric_limits<TIME>::infinity();
				}
				else
				{
					preparationTime = preparationTime - e;
					state.next_internal=preparationTime;
				}
			}
		}
		//====================================================================================================================================
		// confluence transition
		//====================================================================================================================================
		void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
			internal_transition();
			external_transition(TIME(), std::move(mbs));
		}
		
		//====================================================================================================================================
		// output function
		//====================================================================================================================================
		typename make_message_bags<output_ports>::type output() const {
			typename make_message_bags<output_ports>::type bags;
			const NDTime Time_aux=clock1 + state.next_internal;
			ServerMessage out_aux_DB;
			for (int i=0; i< state.Msg_list_DB.size(); i++){					
				out_aux_DB = state.Msg_list_DB[i];
				if (debug){std::cout << "ClientSimple_node"<< ID_numb << " - Output function. Send data to DB - Time_aux: "<< Time_aux<< std::endl;}
				get_messages<typename ClientSimple_defs::DataOutDB>(bags).push_back(out_aux_DB);
			}	
			if(state.estado==0)
			{
				//Do nothing
			}
			else if (state.estado==1){
				//~ std::cout << "ClientSimple_node"<< ID_numb << ". state.Msg_list_up.size():" <<state.Msg_list_up.size() << ". state.Msg_list_down.size(): "<< state.Msg_list_down.size() << endl;
				if (state.Msg_list_up.size() !=0){
					ServerMessage out_aux;
					for (int i=0; i< state.Msg_list_up.size(); i++){					
						out_aux = state.Msg_list_up[i];
						if (debug){std::cout << "ClientSimple_node"<< ID_numb << " - Output function. Tag: "<< out_aux.tag << ". Enviar dato a server: "<< out_aux.dest << " - Time_aux: "<< Time_aux << std::endl;}
						get_messages<typename ClientSimple_defs::DataOutUp>(bags).push_back(out_aux);
					}	
				}
				if (state.Msg_list_down.size() !=0){
					TLCMessage out_aux;
					for (int i=0; i< state.Msg_list_down.size(); i++){					
						out_aux = state.Msg_list_down[i];
						//~ std::cout << "ClientSimple_node"<< ID_numb << " - Output function. Tag: "<< out_aux.getType() << ". Enviar dato a Peer: "<< out_aux.destNode->getIP() << std::endl;
						if (debug){std::cout << "ClientSimple_node"<< ID_numb << " - Output function. Tag: "<< out_aux.getType()  << " - Time_aux: "<< Time_aux<< std::endl;}
						if (debug){std::cout << "Client: " << ID_numb << ". Message to TLC layer - Time: " << Time_aux << std::endl;}
						get_messages<typename ClientSimple_defs::DataOutDown>(bags).push_back(out_aux);
					}
				}
			}

			return bags;

		}
		
		//====================================================================================================================================
		// time_advance function
		//====================================================================================================================================
		TIME time_advance() const {  
			return state.next_internal;
		}
		
		//====================================================================================================================================
		// Process Msg
		//====================================================================================================================================
		//~ void ClientSimpleTlcWse::inner_body(void)
		double process_msg(ServerMessage temp)
		{
			double processing_time=0;	
			if (debug){cout << "~"<<temp.id_tarea<<"-======================================================================" << endl;
			cout << "~"<<temp.id_tarea<<"-ClientSimple" << ID_numb << ". Process Message. Message TAG: " << temp.tag << endl;
			cout << "~"<<temp.id_tarea<<"-======================================================================" << endl;}
			if (temp.tag==0){							//Receive query from server (as if it's generated from this peer)
				//~ Query q = new Query(sentQueries, temp.key, temp.query,  this->time(),temp.TTL_tarea);			
				if (debug){cout << "~"<<temp.id_tarea<<"-ClientSimple" << ID_numb << " gets request from server. Send query to p2p net" << endl;}
				Query *q = new Query(sentQueries, temp.id_tarea,temp.key, temp.query, convert_ND_to_double(clock1));					
				q->setQuerySrc(ne);	
				if (debug){cout << "~"<<temp.id_tarea<<"-ClientSimple" << ID_numb << " - Query SRC: "<< q->src->getIP() << endl;}
				msgID = 0;			//contador global del nodo TLC. No sirve para nada.
				//~ TlcMessage *tm = new TlcMessage(TLC_LOOKUP, msgID, (*pastry)->ne, q->hashValue) ;
				//~ TLCMessage *tm = new TLCMessage(TLC_LOOKUP, msgID, ne, q->hashValue) ;
				TLCMessage tm;
				tm.type = TLC_LOOKUP;
				tm.id_tarea = q->id_tarea;
				tm.id = msgID;
				tm.src=ne;
				tm.dest = q->hashValue;
				tm.final_destination=false;
				tm.setQuery(q);		
				if (debug){q->printQuery();	}
				if (debug){cout << "~"<<temp.id_tarea<<"-ClientSimple" << ID_numb << " - Put Message in output to lower layer."<< endl;}
				state.Msg_list_down.push_back(tm);				//Pasar el mensaje a la capa TLC desde el CLiente en este caso.
			}
			else if (temp.tag==1){						//Alternativa a sendWSEQuery()
				//~ MessageWSE* wseQuery = new MessageWSE(ClientSimpleTlcWse::get_ClientSimpleWSE_obj(), q->hashValue, q->term, PEER);
				if (debug){cout << "~"<<temp.id_tarea<<"-ClientSimple" << ID_numb << " (responsible) wants object from server. Send object request to Server." << endl;}
				//~ ServerMessage *wseQuery = new ServerMessage( temp.query_TLC->hashValue, temp.query_TLC->term, PEER, temp.tag,temp.dest,temp.id_tarea);
				ServerMessage wseQuery;
				wseQuery.query=temp.query;
				wseQuery.query_TLC=temp.query_TLC;
				wseQuery.key=temp.key;
				wseQuery.source=ID_numb;
				wseQuery.tag=1;
				wseQuery.dest=nodes;
				wseQuery.id_tarea=temp.id_tarea;
				state.Msg_list_up.push_back(wseQuery);
			}
			else if (temp.tag==2) {						//receive answer from server
				if (debug){cout << "~"<<temp.id_tarea<<"-ClientSimple" << ID_numb << " (responsible) gets object from server. Send object to lower layer." << endl;}
				double processing_time=0;				
				//~ Entry *newEntry = new Entry(this->time(), temp.key, temp.query,  temp.version, ENTRY_SIZE, getTTL(temp.query));
				double TTL_query = MAX_TTL;
				int version=0;
				//~ Entry *newEntry = new Entry(convert_ND_to_double(clock1), temp.key, temp.query, version, ENTRY_SIZE, TTL_query, temp.id_imagen);	//TO DO: corregir cómo se calcula el TTL. En el original hay que acceder a la caché global del nodo TLC!!
				Entry *newEntry = new Entry(convert_ND_to_double(clock1), temp.key, temp.query, version, ENTRY_SIZE, TTL_query, temp.id_tarea);	//TO DO: corregir cómo se calcula el TTL. En el original hay que acceder a la caché global del nodo TLC!!
				//~ newEntry.timeOut= TTL_query;		//TO DO: corregir cómo se calcula el TTL. En el original hay que acceder a la caché global del nodo TLC!!
				TLCMessage tm;
				tm.type=TLC_WSE;
				msgID = 0;
				tm.id=msgID;						//TO DO: corregir el msgID
				tm.entry=newEntry;
				tm.id_tarea=temp.id_tarea;
				//~ tm.src->ip=ID_numb;
				tm.query=temp.query_TLC;
				state.Msg_list_down.push_back(tm);

			}
			return processing_time;			
		}
		
		//long int getTTL(BIGNUM* b){
		//	long int query_TTL;
		//	long int last_TTL;
		//	Entry* old = (*tlc)->RCACHE->check(b);
		//	if(old==NULL){
		//		query_TTL = MIN_TTL;
		//	}else{
		//		last_TTL = old->getTimeOut();
		//		query_TTL = getTimeIncremental(last_TTL);
		//	}
		//	return query_TTL;
		//}
		//
		//long int getTimeIncremental(long int last_TTL){
		//	last_TTL= this->time() + last_TTL + STATIC_TIMEOUT;
		//	if((last_TTL-this->time())>MAX_TTL){
		//		last_TTL=this->time() + MAX_TTL;
		//	}
		//	return last_TTL;
		//}
		
		//====================================================================================================================================
		double convert_ND_to_double(TIME clock_in){
			double hours=0;
			double minutes=0;
			double seconds=0;
			double milisec=0;
			double microsec=0;
			hours=(double)clock_in.getHours();
			minutes=(double)clock_in.getMinutes();
			seconds=(double)clock_in.getSeconds();
			milisec=(double)clock_in.getMilliseconds();
			microsec=(double)clock_in.getMicroseconds();
			hours=hours*3600.0;
			minutes=minutes*60.0;
			seconds=seconds;
			milisec=milisec/1000.0;
			microsec=microsec/1000000.0;
			double total=0;
			total=hours+minutes+seconds+milisec+microsec;
			return total;
		}
		
		friend std::ostringstream& operator<<(std::ostringstream& os, const typename ClientSimple<TIME>::state_type& i) {
			//~ os << "ackNum: " << i.ackNum; 
			 os << "Estado: " << i.estado << ". next_internal: " << i.next_internal; 
		return os;
		}
	};     


#endif // __ClientSimple_HPP__
