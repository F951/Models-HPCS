/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* TLC_node_Simple:
* Cadmium implementation of CD++ TLC_node_Simple atomic model
*/

#ifndef __TLC_node_Simple_HPP__
#define __TLC_node_Simple_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include "util.hpp"

#include "applicationLayer/Entry.h"
#include "applicationLayer/Replicas.h"
#include "applicationLayer/InLink.h"
#include "applicationLayer/Lru.h"
#include "applicationLayer/Query.h"

#include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"
#include "../data_structures/PastryMessage.hpp"
#include "../data_structures/TLCmessage.hpp"

using namespace cadmium;
using namespace std;




//Port definition
struct TLC_node_Simple_defs{
	struct DataOutUp : public out_port<ServerMessage> { };
	struct DataOutDown : public out_port<PastryMessage> { };
	struct DataOutDB : public out_port<ServerMessage> { };
	//~ struct doneOut : public out_port<bool> { };
	struct DataIn : public in_port<TLCMessage> { };
};

template<typename TIME>
class TLC_node_Simple{
	public:
	
		struct sorter{
			BIGNUM* key;
			sorter(BIGNUM* k) : key(k){}
			bool operator() (Inlink *i1, Inlink *i2)
			{
				if(i1->getFrequency(key) > i2 ->getFrequency(key))
				return true;
				else
				return false;
			}
		};
		
		//Parameters to be overwriten when instantiating the atomic model
		int debug=0;
		int debug_clock=0;
		int debug_terminadas=0;
		TIME   preparationTime;
		int ID_numb;
		NodeEntry* ne;
		//For replicator
		vector<NodeEntry*> leafset;
		double start_Control_time=0;
		double Control_TIME=1;	// last 180
		vector <Entry*> entries;
		int overload;
		int Rep_Strategy;
		double avg_queue;
		int nodes;
		TIME clock1;
		
		// default constructor
		TLC_node_Simple() noexcept{
			preparationTime  = TIME("00:00:00");
			state.estado=0;
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			clock1=TIME("00:00:00");
		}
		 TLC_node_Simple(int _ID_numb, int _nodes, NodeEntry* _ne, vector<NodeEntry*> _leafset) noexcept{
			 NDTime::startDeepView();
			preparationTime  = TIME("00:00:00");
			//~ state.estado=2;
			//~ state.next_internal    = TIME("00:00:00");
			state.estado=0;
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			ID_numb			= _ID_numb;		
			ne = _ne;
			leafset = _leafset;
			nodes = _nodes;	
			state.Case_Responsable=0;
			state.Rep_Strategy=0;
			state.overloaded=0;
			state.terminadas=0;
			state.global_hits=0;
			state.local_hits=0;
			state.misses=0;
			state.hops=0;
			state.nFin=0;
			state.msgID=0;
			state.load=0;
			state.replicas=0;
			state.sentQueries=0;
			state.queries_in=0;
			state.latencia=0;
			state.Processing_time=0;
			clock1=TIME("00:00:00");
			state.cacheSize=100;
			state.localCacheSize=50;
			state.RCACHE = new LRU(&state.cacheSize);
			state.localCache = new LRU(&state.localCacheSize);
			state.t_pas=0;
			state.t_act=0;
			state.queueSize=0;
			state.avg_queue=0;
			state.queries_WSE=0;
			state.outstanding.clear();
			state.stats_TLC.reset_stats();
			state.RCACHE->reset();
			state.localCache->reset();

		}
		

		
		// state definition
		struct state_type{
			int estado;
			vector<ServerMessage> Msg_queue;
			vector<ServerMessage> Msg_list_up;
			vector<PastryMessage> Msg_list_down;
			vector<ServerMessage> Msg_list_DB;
			TIME next_internal;
			bool send_data;
			LRU *RCACHE;		//Global
			int cacheSize;
			LRU *localCache;
			int localCacheSize;
			vector<Query*> waiting;
			vector<TLCMessage> outstanding;
			//track of nodes replicas
			map <BIGNUM*, Replicas*> replicaTrack;
			// query TTL <queryID, TTL>
			map <BIGNUM*, int> queryTTL;
			// query resultID <queryID, resultsID>
			map <BIGNUM*, BIGNUM*> urlsID;
			// one inlink for each NODE identified by its NODEID (BIGNUM)
			map <BIGNUM*,Inlink*> inlinks;
			int Rep_Strategy;
			int Case_Responsable;
			int global_hits;
			int local_hits;
			int misses;
			int hops;
			int overloaded;
			int load;
			int replicas;
			int terminadas;
			int queries_WSE;
			int nFin;
			int msgID;
			int sentQueries;
			int queries_in;
			double latencia;
			double Processing_time;
			
			double t_pas;
			double t_act;
			double avg_queue;
			int queueSize;
			stats_msg stats_TLC;
		}; 
		state_type state;
		
		//ports definition
		using input_ports=std::tuple<typename TLC_node_Simple_defs::DataIn>;
		//~ using output_ports=std::tuple<typename TLC_node_Simple_defs::DataOutUp,typename TLC_node_Simple_defs::DataOutDown,typename TLC_node_Simple_defs::DataOutDB,typename TLC_node_Simple_defs::doneOut>;
		using output_ports=std::tuple<typename TLC_node_Simple_defs::DataOutUp,typename TLC_node_Simple_defs::DataOutDown,typename TLC_node_Simple_defs::DataOutDB>;
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
					if (debug){std::cout << "TLC_node_Simple"<< ID_numb << " - Estado 1." << std::endl; }
					state.Msg_list_down.clear();
					state.Msg_list_up.clear();
					state.Msg_list_DB.clear();
					if (state.Msg_queue.size()!=0){
						if (debug){std::cout << "TLC_node_Simple"<< ID_numb << " - Voy a estado 1." << std::endl; }
						state.estado=1;
						state.next_internal=preparationTime;
						//~ clock1 = clock1 + state.next_internal;
					}else{
						if (debug){std::cout << "TLC_node_Simple"<< ID_numb << " - Voy a estado 0." << std::endl; }
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
			
			if (debug){std::cout << "TLC_node_Simple"<< ID_numb << " - External Transition  ####################################################################################################################### Queue Size: "<< state.queueSize << std::endl; }
			clock1+=e;
			//~ if (1){cout << "TLC_node_Simple "<< ID_numb << " - ET - Nuevo Clock1: " << clock1 << endl;}
			if (debug){cout << std::setprecision(10) << "TLC Node: " << ID_numb << "  - time: " << clock1 << endl;}
			//~ std::cout << "TLC_node_Simple"<< ID_numb << " - Transicion externa. Estado: "<< state.estado << std::endl; 
			int aux_source,aux_dest;
			for( auto &x : get_messages<typename TLC_node_Simple_defs::DataIn>(mbs)){				
				state.queueSize=x.queueSize;				
				load_control();
				if (debug){cout << "~"<<x.id_tarea<<"-TLC Node"<< ID_numb << ". ET - OUTSTANDING SIZE: "<< state.outstanding.size() << endl;}
				state.Processing_time = process_msg(x);
				ServerMessage stats_DB;
				stats_DB.source=ID_numb;
				stats_DB.dest=nodes+1;
				stats_DB.tag=3;
				stats_DB.stats_TLC=new stats_msg();
				stats_DB.stats_TLC->reset_stats();
				stats_DB.stats_TLC->global_hits	= state.global_hits;		
				stats_DB.stats_TLC->local_hits	= state.local_hits;			
				stats_DB.stats_TLC->misses		= state.misses;			
				stats_DB.stats_TLC->hops			= state.hops;			
				stats_DB.stats_TLC->latencia		= state.latencia;			
				stats_DB.stats_TLC->load			= state.load;			
				stats_DB.stats_TLC->queries_in	= state.queries_in;			
				stats_DB.stats_TLC->replicas		= state.replicas;		
				stats_DB.stats_TLC->terminadas		= state.terminadas;		
				stats_DB.stats_TLC->queries_WSE		= state.queries_WSE;		
				stats_DB.id_tarea		= x.id_tarea;		
				//~ if (debug){cout << "STATS_TLC in mSG: "<< ID_numb << ": " << stats_DB.stats_TLC->global_hits << ";" << stats_DB.stats_TLC->local_hits << ";" << stats_DB.stats_TLC->misses << ";" << stats_DB.stats_TLC->hops << ";" << stats_DB.stats_TLC->latencia << ";" << stats_DB.stats_TLC->queries_in << ";" << stats_DB.stats_TLC->replicas << endl;}
				state.Msg_list_DB.push_back(stats_DB);
				reset_statistics();
			}
			
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
					//clock1 = clock1 + state.next_internal;		//El clock ya lo acumulé antes
					break;
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
			if (debug_clock){std::cout << std::setprecision(10) << "TLC_node_Simple"<< ID_numb << " - Output function - Clock1: " << clock1<<  endl;}
			if (debug){std::cout << "TLC_node_Simple"<< ID_numb << " - Output function"<<  endl;}
			if(state.estado==0){
				//
			}
			else if (state.estado==1){
				if (state.Msg_list_up.size() !=0){
					ServerMessage out_aux;
					for (int i=0; i< state.Msg_list_up.size(); i++){					
						out_aux = state.Msg_list_up[i];
						out_aux.dest=ID_numb;
						if (debug){std::cout << "TLC_node_Simple"<< ID_numb << " - Output function. Enviar hacia arriba: "<< out_aux.dest <<  " - TIME_clock1: "<< clock1<< std::endl;}
						get_messages<typename TLC_node_Simple_defs::DataOutUp>(bags).push_back(out_aux);
					}	
				}
				if (state.Msg_list_down.size() !=0){
					PastryMessage out_aux;					
					for (int i=0; i< state.Msg_list_down.size(); i++){					
						out_aux = state.Msg_list_down[i];
						TLCMessage temp =out_aux.getData();		//SOLO PARA MOSTRAR EL CONTENIDO DEL MSJE PASTRY
						if (debug){cout << "OUTPUT TLC "<< ID_numb <<" - temp->src->getNodeID():" << BN_bn2dec(temp.src->getNodeID()) <<  " - TIME_clock1: "<< clock1<< std::endl;}
						if (debug){cout << "OUTPUT TLC "<< ID_numb <<" - Pastry MSG - src_Transport:" << out_aux.src_Transport << " - Pastry MSG - dest_Transport:" << out_aux.dest_Transport  << " - TIME_clock1: "<< clock1<< std::endl;}
						get_messages<typename TLC_node_Simple_defs::DataOutDown>(bags).push_back(out_aux);
					}
				}
				if (state.Msg_list_DB.size() !=0){
					for (int i=0; i< state.Msg_list_DB.size(); i++){					
						ServerMessage out_aux_DB;
						//~ out_aux_DB.clone(state.Msg_list_DB[i]);
						out_aux_DB=state.Msg_list_DB[i];
						out_aux_DB.stats_TLC->global_hits=state.Msg_list_DB[i].stats_TLC->global_hits;
						if (0){cout << "OUTPUT TLC "<< ID_numb <<" - Send data to DB - TIME_clock1: "<< clock1<< std::endl;}
						if (0){std::cout << "TLC_node_Simple"<< ID_numb << " - Output function. Send data to DB. GLOBAL HITS: " << out_aux_DB.stats_TLC->global_hits << "; "<< state.Msg_list_DB[i].stats_TLC->global_hits << std::endl;}
						if (0){std::cout << "TLC_node_Simple"<< ID_numb << " - Output function. Send data to DB. local_hits: " << out_aux_DB.stats_TLC->local_hits << "; "<< state.Msg_list_DB[i].stats_TLC->local_hits << std::endl;}
						if (0){std::cout << "TLC_node_Simple"<< ID_numb << " - Output function. Send data to DB. misses: " << out_aux_DB.stats_TLC->misses << "; "<< state.Msg_list_DB[i].stats_TLC->misses << std::endl;}
						if (0){std::cout << "TLC_node_Simple"<< ID_numb << " - Output function. Send data to DB. hops: " << out_aux_DB.stats_TLC->hops << "; "<< state.Msg_list_DB[i].stats_TLC->hops << std::endl;}
						if (0){std::cout << "TLC_node_Simple"<< ID_numb << " - Output function. Send data to DB. load: " << out_aux_DB.stats_TLC->load << "; "<< state.Msg_list_DB[i].stats_TLC->load << std::endl;}
						if (0){std::cout << "TLC_node_Simple"<< ID_numb << " - Output function. Send data to DB. latencia: " << out_aux_DB.stats_TLC->latencia << "; "<< state.Msg_list_DB[i].stats_TLC->latencia << std::endl;}
						if (0){std::cout << "TLC_node_Simple"<< ID_numb << " - Output function. Send data to DB. replicas: " << out_aux_DB.stats_TLC->replicas << "; "<< state.Msg_list_DB[i].stats_TLC->replicas << std::endl;}
						get_messages<typename TLC_node_Simple_defs::DataOutDB>(bags).push_back(out_aux_DB);
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
		  //~ TIME next_internal;
		  //~ next_internal = preparationTime;
		  //~ return next_internal;
		}

		friend std::ostringstream& operator<<(std::ostringstream& os, const typename TLC_node_Simple<TIME>::state_type& i) {
			//~ os << "ackNum: " << i.ackNum; 
			 os << "Estado: " << i.estado << ". next_internal: " << i.next_internal; 
		return os;
		}
		
	
	
		//====================================================================================================================================
		// Process Msg
		//====================================================================================================================================		
		double process_msg(TLCMessage m)	
		{	
			Entry *e;
			Query *q;
			double processing_time=0;
			if (debug){cout << "~"<<m.id_tarea<<"-======================================================================" << endl;
			cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << " - Process Message. Msg Type:" << m.getType() << " . MSG Source: "<< m.src->getIP() << endl;
			if (m.getType()==4){
				cout << "!!!TLC Node"<< ID_numb << " - Process Message. Msg Type:" << m.getType() << " . MSG Source: "<< m.src->getIP() << " . id_tarea: "<< m.id_tarea << endl;
			}
			else if (m.getType()==1){
				cout << "###TLC Node"<< ID_numb << " - Process Message. Msg Type:" << m.getType() << " . MSG Source: "<< m.src->getIP() << " . id_tarea: "<< m.id_tarea << endl;
			}
			cout << "======================================================================" << endl;}

			switch (m.getType())
			{
				case TLC_WSE:
				{
					state.msgID = state.msgID + 1;
					e= m.getEntry();
					if (debug){e->print();}
					//q=m->getQuery();
					//~ if (debug){cout << "TLC Node"<< ID_numb << ". Entry Recovered" << endl;}
					state.RCACHE->insertEntry ( e->clone() );
					if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Entry inserted in RCACHE" << endl;}
					vector<TLCMessage>::iterator it;
					vector< Query*>::iterator at;
					it=state.outstanding.begin();
					if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Look for entry in outstanding" << endl;}
					if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING SIZE: "<< state.outstanding.size() << endl;}
					int count=0;
					while(it!=state.outstanding.end()){
						if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". count: "<< count << endl;}
						count++;
						if(BN_cmp(((*it).getQuery())->hashValue, e->hashValue)==0){
							TLCMessage tmp = copy_tlc_message((*it));
							if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Entry is in outstanding - HASH: " << e->hashValue << ". Source: "<< tmp.src->getIP() << endl;}
							Entry* clone = e->clone();
							if (debug){clone->print();}
							if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Send Reply." << endl;}
							//~ if (ID_numb == m.query->src->getIP()){
								//~ cout << "~"<<m.id_tarea<<"-======================== TLC NODE: "<< ID_numb<<". Entry Source: "<< m.query->src->getIP() <<" EL PEER YA TIENE EL OBJETO EN LA CACHË LOCAL. TERMINAR." << endl;
								//~ goto TLC_REPLY_tag;
							//~ }
							//~ else
							//~ {
								//~ cout << "~"<<m.id_tarea<<"-======================== TLC NODE: "<< ID_numb<<". Entry Source: "<< m.query->src->getIP() <<" RESPONDER AL PEER." << endl;
								//~ sendReply(clone,tmp);
							//~ }
							sendReply(clone,tmp);
							if (debug){cout << "----------------------------------------------------------------------------------------------" << endl;
							cout << "~"<<m.id_tarea<<"-======================== TLC NODE: "<< ID_numb<< " ======================= SEND REPLY 1 (TLC_WSE)" << endl;
							cout << "----------------------------------------------------------------------------------------------" << endl;}						
							state.RCACHE->update(e->hashValue);
							state.outstanding.erase(it);
							if (tmp.getPathSize()>=2){
								if(BN_cmp((ne)->getNodeID(),(tmp.src)->getNodeID())!=0){
									if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Store InLink." << endl;}
									storeInlink(e->hashValue, tmp.path[tmp.getPathSize()-2]);
								}
							}
						}
						else
						{
							it++;
						}
					}
					if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Look for entry in waiting." << endl;}
					at = state.waiting.begin();
					while(at!=state.waiting.end()){
						if(BN_cmp((*at)->hashValue, e->hashValue)==0){
							state.waiting.erase(at);
						}
						else{
							at++;
						}
					}
					//~ delete m;
					break;
				}
				
				case TLC_LOOKUP:
				{
					//~ cout << "==========================================================================================================" << endl;
					//~ cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". TLC_LOOKUP - PRE_ADD_PATH" << endl;
					//~ printPath(m);
					if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". TLC_LOOKUP - GetQuery" << endl;}
					/*********************************/
					q = m.getQuery();
					/*********************************/
					if((m.path).size()>0){
						if(BN_cmp((ne)->getNodeID(),(m.path[(m.path).size()-1])->getNodeID())!=0){
							if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Add this node to Path" << endl;}
							m.addPath(ne);
						}
					}else{
						m.addPath(ne);
					}
					if (m.getFinalDest()){
						state.hops+=(m.path.size()-1);
					}
					if (debug){
						cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". TLC_LOOKUP - POST_ADD_PATH" << endl;
						printPath(m);}
		
					if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Check RCACHE - q->hashValue: "<< BN_bn2dec(q->hashValue) << endl;}
					e = state.RCACHE->check(q->hashValue);
					//~ if(e!=NULL && !e->old(this->time())){
					if(e!=NULL && !e->old(convert_ND_to_double(clock1))){
						state.global_hits++;
						if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Query is in RCACHE. SendReply()" << endl;}
						Entry* clone = e->clone();
						if (debug){clone->print();}
						sendReply(clone,m);
							if (debug){cout << "----------------------------------------------------------------------------------------------" << endl;
							cout << "~"<<m.id_tarea<<"-==================== TLC NODE: "<< ID_numb<< " ========================== SEND REPLY 2" << endl;
							cout << "----------------------------------------------------------------------------------------------" << endl;	}		
						state.RCACHE->update(e->hashValue);
						if (m.getPathSize()>=2){
							if(BN_cmp((ne)->getNodeID(),(m.src)->getNodeID())!=0){
								storeInlink(q->hashValue, m.path[m.getPathSize()-2]);
							}
						}
					}
					// if it is the final Dest or if the entry is old.
					else if( ( m.getFinalDest())){
						if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". This node is final destination. Send Query to Server" << endl;}
						if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING PUSHBACK 1" << endl;}
						
						if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING SIZE PRE INSERCION: "<< state.outstanding.size() << endl;}
						state.outstanding.push_back(m);
						state.misses++;
						sendWSEQuery( q );
						state.queries_WSE++;
						if (debug){cout << "============================================================================" << endl;
						cout << "~"<<m.id_tarea<<"-======================== TLC NODE: "<< ID_numb<< " ============= SEND QUERY WSE CR -1" << endl;
						cout << "============================================================================" << endl;}
						if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING SIZE: "<< state.outstanding.size() << endl;}
					}
					else if (e!=NULL && e->old(convert_ND_to_double(clock1))){
						if (debug){cout << "TLC Node"<< ID_numb << ". Query in RCACHE, but OLD" << endl;}
						switch ((int)state.Case_Responsable){
							case (0):
							{
								state.misses++;
								state.RCACHE->remove(e);
								/*********************************/
								TLCMessage tmp;
								tmp.type		=m.type;
								tmp.id		=m.id;
								tmp.src		=m.src;
								tmp.dest		=m.dest;
								tmp.final_destination		=m.final_destination;
								tmp.query=m.query;
								tmp=copy_tlc_message(m);
								
								PastryMessage pm;
								pm.type =PASTRY_SENDDHT;
								pm.srcNode =ne;
								pm.dest =tmp.dest;
								pm.DATA =tmp;  
								pm.src_Transport=ID_numb;
								pm.dest_Transport=ID_numb;
								pm.id_tarea=tmp.id_tarea;
								state.Msg_list_down.push_back(pm);
								if (debug){cout << "============================================================================" << endl;
								cout << "~"<<m.id_tarea<<"-========================== TLC NODE: "<< ID_numb<< " ===================== SEND QUERY WSE CR 0" << endl;
								cout << "============================================================================" << endl;}
								//~ continueRouting(&m);
								/*********************************/
								break;
							}
							case (1):
							{
								if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING SIZE: " << state.outstanding.size() << endl;}
								if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING PUSHBACK 2" << endl;}
								state.outstanding.push_back(m);
								if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING PUSHBACK 2" << endl;}
								if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING SIZE: " << state.outstanding.size() << endl;}
								state.misses++;
								sendWSEQuery( q );
								state.queries_WSE++;
								if (debug){cout << "============================================================================" << endl;
								cout << "~"<<m.id_tarea<<"-========================== TLC NODE: "<< ID_numb<< " ===================== SEND QUERY WSE CR 1" << endl;
								cout << "============================================================================" << endl;}
								break;
							}
						}
					}
					else
					{
						if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Query not in RCACHE, check LocalCACHE" << endl;}
						e= state.localCache->check(q->hashValue);
						if(e!=NULL && e->old(convert_ND_to_double(clock1))){
							if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Cache Local - Old entry" << endl;}
							state.localCache->remove(e);
							e=NULL;
						}
						if (e!=NULL){
							
							state.local_hits++;
							if (debug){cout << "========================================================================================================================================================================" << endl;}
							if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Query in LocalCACHE, sendReply()" << endl;}
							if (debug){cout << "========================================================================================================================================================================" << endl;}
							Entry* clone = e->clone();
							if (debug){clone->print();}
							//if (ID_numb == m.query->src->getIP()){
							//	cout << "~"<<m.id_tarea<<"-======================== TLC NODE: "<< ID_numb<<". Entry Source: "<< m.query->src->getIP() <<" EL PEER YA TIENE EL OBJETO EN LA CACHË LOCAL. TERMINAR." << endl;
							//	goto TLC_REPLY_tag;
							//}
							//else
							//{
							//	cout << "~"<<m.id_tarea<<"-======================== TLC NODE: "<< ID_numb<<". Entry Source: "<< m.query->src->getIP() <<" RESPONDER AL PEER." << endl;
							//	sendReply(clone,m);
							//}
							if (debug){cout << "~"<<m.id_tarea<<"-======================== TLC NODE: "<< ID_numb<<". Entry Source: "<< m.query->src->getIP() <<" RESPONDER AL PEER." << endl;}
							sendReply(clone,m);
							if (debug){cout << "----------------------------------------------------------------------------------------------" << endl;
							cout << "~"<<m.id_tarea<<"-======================== TLC NODE: "<< ID_numb<< " ======================= SEND REPLY 3" << endl;
							cout << "----------------------------------------------------------------------------------------------" << endl;	}
							state.localCache->update(e->hashValue);
						}else{
							if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". Query not LocalCACHE, continue Routing" << endl;}
							//~ PastryMessage *pm = new PastryMessage(PASTRY_SENDDHT,ne,tm->dest, tm);  
							TLCMessage tmp;
							tmp.type		=m.type;
							tmp.id		=m.id;
							tmp.src		=m.src;
							tmp.dest		=m.dest;
							tmp.final_destination		=m.final_destination;
							tmp.query=m.query;
							tmp=copy_tlc_message(m);
							
							PastryMessage pm;
							pm.type =PASTRY_SENDDHT;
							pm.srcNode =ne;
							pm.dest =tmp.dest;
							pm.DATA =tmp;  
							pm.src_Transport=ID_numb;
							pm.dest_Transport=ID_numb;
							pm.id_tarea=tmp.id_tarea;
							state.Msg_list_down.push_back(pm);
							//~ continueRouting(&m);
						}
						if (m.getPathSize()==1){
							state.waiting.push_back(m.query);
						}
					}
					break;
				}
				
				case TLC_ENDS:
				{
					state.nFin++;
					break;
				}
				
				case TLC_REPLY:
				{
					e = m.getEntry();
					q= m.getQuery();
					if (debug){q->printQuery3();}
					if (debug){cout << "========================================================================================================================================================================" << endl;
					cout << "~"<<m.id_tarea<<"-TLC "<< ID_numb << " insert Entry =========================================================================================================" << endl;
					cout << "========================================================================================================================================================================" << endl;}
					state.localCache->insertEntry(e->clone());
/*TLC_REPLY_tag:*/	if (debug){cout << "~"<<m.id_tarea<<"-TLC - Check Waiting" << endl;}
					vector<Query*>::iterator at;
					at= state.waiting.begin();
					while(at!=state.waiting.end())
					{
						if(BN_cmp((*at)->hashValue, e->hashValue)==0){
							state.waiting.erase(at);
						}else{
							at++;
						}
					}
					if (debug){cout << "~"<<m.id_tarea<<"-TLC - Check Outstanding" << endl;}
					vector<TLCMessage>::iterator it1;
					if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING SIZE: "<< state.outstanding.size() << endl;}
					it1 = state.outstanding.begin();
					while(it1!=state.outstanding.end())
					{
						if(BN_cmp(e->hashValue, ((*it1).getQuery())->hashValue)==0)
						{
							if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING HIT EN TLC REPLY: "<< endl;}
							//~ TLCMessage tmp = *it1;
							TLCMessage tmp = copy_tlc_message((*it1));
							Entry* clone = e->clone();
							if (debug){clone->print();}
							sendReply(clone,tmp);
							if (debug){cout << "----------------------------------------------------------------------------------------------" << endl;
							cout << "~"<<m.id_tarea<<"-========================= TLC NODE: "<< ID_numb<< " ====================== SEND REPLY 4" << endl;
							cout << "----------------------------------------------------------------------------------------------" << endl;	}
							state.outstanding.erase(it1);
						}else{
							it1++;
						}
					}
					if (debug){cout << "~"<<m.id_tarea<<"-TLC Node"<< ID_numb << ". OUTSTANDING SIZE: "<< state.outstanding.size() << endl;}
					//~ (*observer)->addNQueriesIn();
					//~ state.queries_in++;
					//~ (*observer)->addLatencia((double)((this->time())-(q->t_start)));
					state.latencia+=(double)(convert_ND_to_double(clock1) - (q->t_start));
					state.terminadas++;
					if (debug_terminadas){cout << "~"<<m.id_tarea<<"- ===========================================================" <<  ID_numb << endl;
					cout << "~"<<m.id_tarea<<"- ==================== FINALIZACION EN NODO TLC: " <<  ID_numb << ". id_tarea: "<< e->id_tarea << endl;
					cout << "~"<<m.id_tarea<<"- ===========================================================" <<  ID_numb << endl;}
					break;
				}
				
				case TLC_REPLICATE:
				{
					switch((int)state.Case_Responsable)
					{
						// ask directly to the WSE
						case (1):
						{
							e = m.getEntry();
							//~ (*client)->sendWSEQuery( new Query(0, (char*)(e->key).c_str(),BN_dup( e->hashValue), this->time()) );
							sendWSEQuery( new Query(0, e->id_tarea, (char*)(e->key).c_str(),BN_dup( e->hashValue), convert_ND_to_double(clock1)) );
							state.queries_WSE++;
							if (debug){cout << "~"<<m.id_tarea<<"-============================================================================" << endl;
							cout << "~"<<m.id_tarea<<"-====================== TLC NODE: "<< ID_numb<< " ========================= SEND WSE QUERY for Replication" << endl;
							cout << "~"<<m.id_tarea<<"-============================================================================" << endl;	}					
							break;
						}
						// stores entry locally
						case (0):
						{
							state.RCACHE->insertEntry((m.getEntry())->clone());
							break;
						}
						
						default:
						{ 
							if (debug){cout << "default Tlc rep" << endl;}
							break;
						}
					}
					//~ delete m;
					break;
		
				}
				
				default:
				{
					if (debug){cout << "error LeafsetBubble" << endl;}
					abort();
					break;
				}
			}
			return processing_time;
		}	
	
		void sendReply(Entry* _e, TLCMessage tm){
			setAvgQueue();
			state.overloaded++;
			state.queries_in++;		//~ (*observer)->addNQueries();
			state.load++; 			//~ (*observer)->addLoad(((*pastry)->ne)->getIP());
			Query * q= tm.getQuery();
			if (debug){q->printQuery2();}
			//~ TLCMessage* reply = new TLCMessage(TLC_REPLY, state.msgID, ne, q->src);
			TLCMessage reply;
			reply=copy_tlc_message(tm);
			reply.type=TLC_REPLY;
			reply.id=state.msgID;
			reply.src = ne;
			reply.destNode=tm.destNode;				//O bien tm.query->src
			reply.final_destination=false;
			reply.setEntry(_e);
			reply.setQuery(tm.getQuery());
			state.msgID = state.msgID + 1;
			
			PastryMessage pm;
			if (ID_numb == tm.query->src->getIP()){
				pm.type =PASTRY_RECEIVE;
			}else{
				pm.type =PASTRY_SENDDIRECT;	
			}
			pm.srcNode =ne;
			if (debug){cout << "~"<<reply.id_tarea<<"-!!!DEGUB: cuál era el source?: IP:"<< tm.query->src->getIP() << ". nodeID"<<  BN_bn2dec(tm.query->src->getNodeID()) << endl;}
			pm.dest =tm.query->src->getNodeID();				// O bien tm.query->src->getIP();
			pm.destNode = tm.query->src;
			pm.DATA =reply;  
			//~ pm.src_Transport=tm.query->src->getIP();	//ORIGINAL
			pm.src_Transport=ID_numb;		//Esto es para que la queue de Pastry no lo filtre.
			pm.dest_Transport=ID_numb;		//Esto es para que la queue de Pastry no lo filtre.
			//~ pm.dest_Transport=tm.query->src->getIP();		//En pastry trato de ponerle el destino correcto.
			pm.id_tarea=reply.id_tarea;
			state.Msg_list_down.push_back(pm);
		}	
			
		void storeInlink(BIGNUM* key, NodeEntry* n)
		{
			bool inlinkStored = false;
			Inlink* i;
			map<BIGNUM*,Inlink*>::iterator it;
			it = state.inlinks.begin();
			while(it != state.inlinks.end()){
				if(BN_cmp(it->first, n->getNodeID())==0){
					inlinkStored = true;
					i = it ->second;
					break;
				}
				++it;
			}
			//Existe Inlink
			if( inlinkStored ){
				i->putQuery(key);
			}else{
				//cout <<"New Inlink " << BN_bn2hex(n -> getNodeID() )<< endl;
				Inlink* i2 = new Inlink(n);
				i2->putQuery(key);
				state.inlinks.insert(pair<BIGNUM*,Inlink*>((n)->getNodeID(), i2));
				state.inlinks[n->getNodeID()] = i2;
			}
		}	
			
		void continueRouting(TLCMessage* tm)
		{
			//Esto se hace en el lugar.
		}	
		
		void sendWSEQuery(Query *q){
			ServerMessage msg;
			msg.source = ID_numb;
			msg.tag = 1 ;
			msg.key.assign(q->term);
			msg.id_tarea=q->id_tarea;
			msg.query = q->hashValue;
			state.Msg_list_up.push_back(msg);
			
		}
		
		TLCMessage copy_tlc_message(TLCMessage m_in){
			TLCMessage m_out;
			m_out.type=m_in.type;
			m_out.id=m_in.id;
			m_out.query=m_in.query;
			m_out.entry=m_in.entry;
			m_out.lc=m_in.lc;
			m_out.src=m_in.src;
			m_out.destNode=m_in.destNode;
			m_out.dest=m_in.dest;
			m_out.final_destination=m_in.final_destination;
			m_out.path.clear();
			m_out.path=m_in.path;
			m_out.id_tarea=m_in.id_tarea;
			return m_out;
		}
		
			
		void printPath(TLCMessage tm)
		{
			Query* q  = tm.getQuery();
			cout<< "--------- PATH for " << BN_bn2dec(q->hashValue) << " ---------"<<endl;
			for(unsigned int k = 0; k < tm.path.size(); k++){
				cout << k << " -> " <<  BN_bn2dec((tm.path[k])->getNodeID() ) <<endl;
			}
			cout<<"--------------------------------------------------------------------"<<endl;
		}
	
		//====================================================================================================================================
		// Replicator
		//====================================================================================================================================		
		void sendReplica(Entry* e ,NodeEntry* nodeEntry)
		{
			/****************************************************
			 * Send Replica
			 * Crear mensajes TLC y Pastry, y enviar a Pastry con PASTRY_SEND_DIRECT
			 * **************************************************/			
			if(debug){cout << "========================================================= sendReplica ----- TIME: " << convert_ND_to_double(clock1) << endl;}
			//TlcMessage* replica = new TlcMessage(TLC_REPLICATE, msgID, (*pastry)->ne, nodeEntry);

			TLCMessage replica;
			replica.type=TLC_REPLICATE;
			replica.id=state.msgID;
			replica.id_tarea=e->id_tarea;
			replica.src=ne;
			replica.destNode=nodeEntry;
			replica.dest=nodeEntry->getNodeID();
			replica.final_destination=0;
			replica.setEntry(e);
			//~ e->print();
			state.replicas++;
			state.msgID = state.msgID+1;
			
			PastryMessage pm;
			pm.type =PASTRY_SENDDIRECT;
			pm.srcNode =ne;
			pm.time_aux=convert_ND_to_double(clock1);
			pm.id_tarea=replica.id_tarea;
			int IP_AUX=nodeEntry->getIP();			
			pm.src_Transport=IP_AUX;		//O bien tm.query->src->getIP()
			pm.dest_Transport=ID_numb;					//O bien tm.query->src->getIP()

			if (debug){cout << "DEGUB TLC sendReplica: " << ID_numb<< ", enviar la réplica a: IP: "<< nodeEntry->getIP() << ". nodeID: "<<  BN_bn2dec(nodeEntry->getNodeID()) << endl;}
			//~ if (debug){cout << "DEGUB TLC: " << ID_numb<< ". Otras fuentes de error: "<< ne->getIP() << ". nodeID: "<<  BN_bn2dec(ne->getNodeID()) << endl;}
			pm.dest =nodeEntry->getNodeID();			//O bien tm.query->src->getIP();
			pm.destNode= new NodeEntry(nodeEntry->getNodeID(),IP_AUX);
			pm.DATA =replica;  
			if (debug){cout << "DEGUB TLC sendReplica: " << ID_numb<< ", src_Transport: "<< pm.src_Transport << ". dest_Transport "<<  pm.dest_Transport << endl;}
			state.Msg_list_down.push_back(pm);
		}

		void setAvgQueue()
		{
		   state.t_act = convert_ND_to_double(clock1);
		   state.avg_queue = state.avg_queue + ( ( state.queueSize + 1) *(state.t_act - state.t_pas) );
		   state.t_pas = state.t_act;
		}

		void resetQueue()
		{
		   state.avg_queue = 0;
		   state.overloaded=0;
		}

		bool isReplicated(BIGNUM* nodeId, BIGNUM* key)
		{
		   map<BIGNUM*, Replicas*>::iterator it;
		   it = state.replicaTrack.begin();
		  // cout << "TLC-IsReplicated?"<< endl;
		   
		   while(it != state.replicaTrack.end())
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

		bool isNewReplica(BIGNUM* id)
		{
		   map<BIGNUM*, Replicas*>::iterator it;
		   it = state.replicaTrack.begin();
		   while(it != state.replicaTrack.end())
		   {
			 if( BN_cmp(it->first, id) == 0 )
				return false;
			 ++it;
		   }

		   return true;
		}

		void trackReplica(BIGNUM* nodeId, BIGNUM* key)
		{
			// bool flag = isReplicated(nodeId, key);
			bool flag = isNewReplica(nodeId);		  
			//No esta
			flag=0;				//???
			if(flag)
			{
				Replicas* r = new Replicas(key ,nodeId);
				state.replicaTrack.insert(pair <BIGNUM*, Replicas*>(nodeId, r) );
				state.replicaTrack[nodeId] = r;
				//~ (*observer)->addReplicas();
				state.replicas++;
				if (debug){cout << "addReplicas en trackReplica1" << endl;}
				//Existe hay q revisar
			}else	{
				map<BIGNUM*, Replicas*>::iterator it5;
				it5 = state.replicaTrack.begin();

				while(it5 != state.replicaTrack.end())
				{
					if(BN_cmp(nodeId,it5->first) == 0)
					{
						(it5->second)->putReplica(key);
						//~ (*observer)->addReplicas();
						state.replicas++;
						//cout << "addReplicas en trackReplica2" << endl;
					}
					++it5;
				}
			}
		}
				
		//*****************************************************Funciones REPLICATOR
		static bool compare(Entry *e1 , Entry *e2){
			if(e1->frequency > e2->frequency)
				return true;
			else
				return false;
		}
		  
			
		void load_control(void)		//Ex inner body
		{
			if(debug){cout << "======================================================================================================================================" << endl;}
			if(debug){cout << "LOAD CONTROL" << endl;}
			//~ if(debug){cout << "======================================================================================================================================" << endl;}
			if ( ( convert_ND_to_double(clock1) - start_Control_time ) < Control_TIME ){	//No hacer el control antes de tiempo.
				if(debug){cout << "Still not control Time." << endl;}
				return;
			}
			if(debug){cout << "Control load." << endl;}
			start_Control_time = convert_ND_to_double(clock1);
			

			overload = state.overloaded;		//En overloaded tengo que acumular la carga en relación a las consultas.
			
			if(debug){cout << "Peer is overloaded? " << state.overloaded << endl;}

			//Checks nodes load
			if( isOverloaded() )
			{  
				// Replication
				// 1) Sort Entries
				// 2) Select the most frequent
				// 3) Replicate based on strategy
				if(debug){cout << "Peer Overloaded!!" << endl;}
				// Copy LocalCACHE and RCACHE entries to Vector entries
				retrieveEntries();
				// Sort by frequency  
				sort(entries.begin(), entries.end(), compare);
				//Select strategy and replicate
				replicate(Rep_Strategy);
				//RESET
				resetQueue();
				state.inlinks.clear();
				state.RCACHE->reset();
			}
			
		}

		double getQueueSize(){
			double size;
			size =  ( avg_queue/DELTA_T);		// En avg_queue tengo que almacenar el tamaño promedio
			return size;
		}

		bool isOverloaded(){
			//Si esta sobrecargado --> cola mayor a un tamano t
			if (overload > OVERLOAD_THRESHOLD)
				return true;
			return false;
		}

		int getAvailableCapacity()
		{
			if(getQueueSize() > OVERLOAD_THRESHOLD)
				return 0;
			return int (OVERLOAD_THRESHOLD - getQueueSize() );
		}

		void retrieveEntries(){
			//Copy the entries from RCACHE to Vector entries to be sorted
			entries = state.RCACHE->getEntries();			
		}

		void replicate(int strategy)
		{
			switch (strategy)
			{
				//LEAFSET
				case 0:
					replicateLeafset();
					break;
				//BUBBLE
				case 1:
					replicateBubble();
					break;
			}
		}


		void replicateLeafset()
		{
			//Verificar linea
			vector<NodeEntry*> ls;
			//~ vector<NodeEntry*> ls = leafset->listAllNodes();
			for (int i=0; i<leafset.size(); i++) 
				{
					ls.push_back(leafset[i]); 
				}
			Entry *e = NULL;
			int j = 0;

			//Sort vector by Freq
			sort(entries.begin(), entries.end(), compare);
			//cout << "REP - SORTING"<< endl;

			vector< Entry*>::iterator it;
			it = entries.begin();
			//cout <<"------------------------------------------------------"<< endl;
			//while(it != entries.end())
			//{
			//   cout << BN_bn2hex((*it) ->hashValue)<<" - " <<(*it)->frequency << endl;
			//  ++it;
			//}
			
			//Search for the first not replicated entry
			if(entries.size()>0){
				do{
					e = entries[j];
					j++;
				}while(entries.size() > (unsigned)j && e->isReplicated);
				if(e->isReplicated){
					//cout << "NO SE PUEDE REPLICAR MAS..."<< BN_bn2hex((*tlc)->pid) <<endl;
					return;
				}
			}else{
				//cout << "ENTRIES VACIAS" << BN_bn2hex((*tlc)->pid) << endl;
				return;
			}

			//cout << "REP- Entry " << BN_bn2hex(e->hashValue ) <<" - " << e->frequency<< endl;
			if(debug){cout << "REP- Entry " << BN_bn2dec(e->hashValue ) <<" - " << e->frequency<< endl;}

			for(int i = 0 ; (unsigned) i < ls.size(); i++)
			{
				if(!isReplicated(ls[i]->getNodeID(), e->hashValue))
				{
					//Send Replication message to leafset node
					//Entry e over nodes
					// cout << "REP - FOR" << endl;
					e->isReplicated = true;

					if(debug){cout << "REP- Tracking Replica"<< endl;}
					//Guardamos quienes son las replicas?
					trackReplica(ls[i]->getNodeID(), e->hashValue);

					if(debug){cout << "REP- Sending Replica"<<endl;}
					//Send Replication Message
					sendReplica( e->clone() , ls[i]);
				} 
			}
		}


		void replicateBubble()
		{
			// Sort vector entries by freq
			sort(entries.begin(), entries.end(), compare);

			vector< Entry*>::iterator it;
			it = entries.begin();
			if(debug){cout <<"----------------------ENTRIES------------------------------"<< endl;
				 while(it != entries.end())
				 {
				  if(debug){cout << BN_bn2hex((*it) ->hashValue)<<" - " <<(*it)->frequency << endl;}
				  ++it;
				 }
			}
			//Most frequent query
			//Search for the first not replicated entry
			Entry *e = NULL;
			int j = 0;
			if(entries.size()>0){
				do{
					e = entries[j];
					j++;
				}while(entries.size() > (unsigned) j && e->isReplicated);

				if(e->isReplicated){
					//cout << "NO SE PUEDE REPLICAR MAS..."<<endl;
					return;
				}				  
			}else{
				//cout <<"ENTRIES VACIAS!!"<< endl;
				return;
			}

			// if(e==NULL)
			//  cout << "ENTRY NULL!!" <<endl;

			//Which inlinks forwards the entry e
			// list ??? inlinks
			vector<Inlink*> list;
			map<BIGNUM*, Inlink*>::iterator it2;

			it2 = state.inlinks.begin();
			//cout << "INLINK - Size: " << (*tlc)->inlinks.size() << endl ; 
			// cout << "Entry: " << BN_bn2hex( e->hashValue) << endl;

			while(it2 != state.inlinks.end())
			{
				//  cout<< "Inlink : "<< BN_bn2hex(((it2->second)->getInlinkID())->getNodeID() )<< endl;

				if(( it2-> second)->isKeyContained(e->hashValue) )
				{
					//terminar
					//push inlink to list
					//	cout << "Entry Contained!!" << endl;
					list.push_back(it2->second );
				}
				++it2;
			}
			sort( list.begin(), list.end(), sorter(e->hashValue));
			
			//DEBUG
			vector< Inlink*>::iterator it3;
			it3 = list.begin();
			if(debug){cout <<"----------------------INLINKS------------------------------"<< endl;
				while(it3 != list.end())
				{
					 {cout << BN_bn2hex( ((*it3)->getInlinkID())->getNodeID() )<< " - " <<(*it3)->getFrequency(e->hashValue) << endl; }
					++it3;    
				}
			}
			unsigned int index = 0 ;
			// cout << "REP - list Size " << list.size() << endl;
			while(index < list.size())
			{
				if(!isReplicated(list[index]->getInlinkID()->getNodeID(), e->hashValue)
				&& list[index]->getFrequency(e->hashValue) > 0)
				{
					e->isReplicated = true;
					//Guardamos quienes son las replicas
					trackReplica((list[index]->getInlinkID())->getNodeID(), e->hashValue);

					// Replica Send
					sendReplica( e->clone() , list[index]->getInlinkID());
				}
				index++;
			}  
		}

		void reset_statistics(){
			state.global_hits=0;
			state.local_hits=0;	
			state.misses=0;		
			state.hops=0;	
			state.latencia=0;
			state.load=0;
			state.queries_in=0;
			state.replicas=0;
			state.terminadas=0;
			state.queries_WSE=0;
		}
	
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
		
	};     


#endif // __TLC_node_Simple_HPP__
