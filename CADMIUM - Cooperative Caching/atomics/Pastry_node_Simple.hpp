/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* Pastry_node_Simple:
* Cadmium implementation of CD++ Pastry_node_Simple atomic model
*/

#ifndef __Pastry_node_Simple_HPP__
#define __Pastry_node_Simple_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include "p2pLayer/Pastry_Node.h"

#include <limits>
#include <assert.h>
#include <string>
#include "util.hpp"
#include "p2pLayer/Leafset.h"

#include "../data_structures/PastryMessage.hpp"
#include "../data_structures/TLCmessage.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Pastry_node_Simple_defs{
	struct DataOutUp : public out_port<TLCMessage> { };
	struct DataOutDown : public out_port<PastryMessage> { };
	struct DataOutDB : public out_port<ServerMessage> { };
	//~ struct doneOut : public out_port<bool> { };
	struct DataIn : public in_port<PastryMessage> { };
	
};

template<typename TIME>
class Pastry_node_Simple{
	public:
		//Parameters to be overwriten when instantiating the atomic model
		int debug=0;
		int debug_clock=0;
		TIME   preparationTime;
		int ID_numb;
		BIGNUM  *NODEID; 
		Pastry_Node* Node;
		//~ Pastry_Node* Node;
		UTIL *util; 
		int nodes;
		TIME clock1;
		
		// default constructor
		Pastry_node_Simple() noexcept{
			preparationTime  = TIME("00:00:00");
			state.estado=0;
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			clock1=TIME("00:00:00");
		}
		 //~ Pastry_node_Simple(int _ID_numb, BIGNUM  * _id) noexcept{
		 Pastry_node_Simple(int _ID_numb,  int _nodes, BIGNUM  * _id, Pastry_Node* _Node) noexcept{
			preparationTime  = TIME("00:00:00");
			state.estado=0;
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			ID_numb			= _ID_numb;
			state.Node = _Node;
			nodes = _nodes;	
			Node = _Node;			//Corregir luego. Es porque necesito acceder al nodeEntry desde el Cliente.
			NODEID = _Node->nodeID;
			state.OutputUp = 0;
			state.OutputDown = 0;	
			//~ state.leafset= new Leafset(_id, 16);
			//~ cout << "Pastry node nr: " << ID_numb << ". Leafset->nodeID: " <<  BN_bn2dec((Node->leafset)->nodeID) << "::::::::::::::::::::::::::"<<endl;
			vector<NodeEntry*> result;
			result=(Node->leafset)->listAllNodes();
			//~ cout << "Size Leafset Original: " << result.size()<< endl;
			//~ for (int j=0;j<result.size();j++){
				//~ cout << "Leafset ["<<j<<"] - PastryProtocol "<< ID_numb<<": "<< BN_bn2dec(result[j]->nodeID) << endl;
			//~ }
			state. Processing_time=0;
			clock1=TIME("00:00:00");
			util            = new UTIL();
			if (debug){
				cout << "Pastry ID_num: " << ID_numb << ". NODEID: " << BN_bn2dec(NODEID)<< ". Node->ip: "<< Node->ip;
				cout <<  ". Node->nodeID: "<< BN_bn2dec(Node->nodeID) << ". Node->ne->getIP(): " << Node->ne->getIP() << ". Node->ne->getNodeID(): " << BN_bn2dec(Node->ne->getNodeID())<< endl;
			}
			
			
		}
		
		// state definition
		struct state_type{
			int estado;
			vector<PastryMessage> Msg_queue;
			vector<TLCMessage> Msg_list_up;
			vector<PastryMessage> Msg_list_down;
			vector<ServerMessage> Msg_list_DB;
			TIME next_internal;
			bool send_data;
			int OutputUp;
			int OutputDown;
			Leafset *leafset;
			Pastry_Node* Node;
			double Processing_time;
			
		}; 
		state_type state;
		
		//ports definition
		using input_ports=std::tuple<typename Pastry_node_Simple_defs::DataIn>;
		//~ using output_ports=std::tuple<typename Pastry_node_Simple_defs::DataOutUp,typename Pastry_node_Simple_defs::DataOutDown,typename Pastry_node_Simple_defs::DataOutDB,typename Pastry_node_Simple_defs::doneOut>;
		using output_ports=std::tuple<typename Pastry_node_Simple_defs::DataOutUp,typename Pastry_node_Simple_defs::DataOutDown,typename Pastry_node_Simple_defs::DataOutDB>;
		//====================================================================================================================================
		//internal transition
		//====================================================================================================================================
		void internal_transition() {
			assert(state.estado!=0);
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
					if (debug){std::cout << "Pastry_node_Simple"<< ID_numb << " - Cambio de estado 1 a 2." << std::endl; }
					state.Msg_list_down.clear();
					state.Msg_list_up.clear();
					state.Msg_list_DB.clear();
					if (state.Msg_queue.size()!=0){
						if (debug){std::cout << "Pastry_node_Simple"<< ID_numb << " - Voy a estado 1." << std::endl; }
						state.estado=1;
						state.next_internal=preparationTime;
						//~ clock1 = clock1 + state.next_internal;
					}else{
						if (debug){std::cout << "Pastry_node_Simple"<< ID_numb << " - Voy a estado 0." << std::endl; }
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
			//std::cout << "Pastry_node_Simple"<< ID_numb << " - Transicion externa. Estado: "<< state.estado << std::endl; 
			int flag_dataIn=0;
			int aux_source,aux_dest;
			if (debug_clock){cout << "Pastry_node_Simple "<< ID_numb << " - ET - Clock1: " << clock1<< ". Elapsed Time: " << e  << ". Next_interal previo: " << state.next_internal << endl;}
			clock1+=e;
			if (debug_clock){cout << "Pastry_node_Simple "<< ID_numb << " - ET - Nuevo Clock1: " << clock1 << endl;}
			for( auto &x : get_messages<typename Pastry_node_Simple_defs::DataIn>(mbs)){
				//Si llega un mensaje quiere decir que estoy en el estado 3. Acá puedo poner un assert para que, si no es el estado 3, me de un error.
				//~ assert (state.estado!= 0 || state.estado!= 3);		
				if (x.dest_Transport==ID_numb){
					TLCMessage temp =x.getData();
					//~ cout << "ET "<< ID_numb <<" - temp->type" << temp.type << endl;
					//~ cout << "ET "<< ID_numb <<" - temp->src->getIP():" << temp.src->getIP() << endl;
					//~ cout << "ET "<< ID_numb <<" - temp->src->getNodeID():" << BN_bn2dec(temp.src->getNodeID()) << endl;
					//~ cout << "ET "<< ID_numb <<" - temp->destNode->getIP():" << temp.destNode->getIP() << endl;
					//~ cout << "ET "<< ID_numb <<" - temp->destNode->getNodeID():" << BN_bn2dec(temp.destNode->getNodeID()) << endl;
					//~ cout << "ET "<< ID_numb <<" - temp->dest:" << BN_bn2dec(temp.dest) << endl;
					state.Processing_time = process_msg(x);
					flag_dataIn=1;
				}
				
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
						//clock1 = clock1 + state.next_internal;		//El clock ya lo acumulé antes
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
				if (debug){std::cout << "Pastry_node_Simple "<< ID_numb << " - Output function. Send data to DB - TIME_clock1: "<< clock1<< std::endl;}
				get_messages<typename Pastry_node_Simple_defs::DataOutDB>(bags).push_back(out_aux_DB);
			}	
			if(state.estado==0)
			{
				//
			}
			else if (state.estado==1){
				if (state.Msg_list_up.size() !=0){
					TLCMessage out_aux;
					for (int i=0; i< state.Msg_list_up.size(); i++){					
						out_aux = state.Msg_list_up[i];
						//~ std::cout << "Pastry_node_Simple"<< ID_numb << " - Output function. Tag: "<< out_aux.tag << ". Enviar dato a server: "<< out_aux.dest << std::endl;
						if (debug){std::cout << "Pastry_node_Simple "<< ID_numb << " - Output function - TIME_clock1: "<< clock1<< std::endl;}
						get_messages<typename Pastry_node_Simple_defs::DataOutUp>(bags).push_back(out_aux);
					}	
				}
				if (state.Msg_list_down.size() !=0){
					PastryMessage out_aux;
					for (int i=0; i< state.Msg_list_down.size(); i++){					
						out_aux = state.Msg_list_down[i];
						TIME timeAux=clock1;
						double auxDouble;
						//~ auxDouble = convert_ND_to_double(clock1);
						if (debug){std::cout << "~"<<out_aux.id_tarea<<"-Pastry_node_Simple "<< ID_numb << " - Output function. Source: "<< out_aux.src_Transport << ". Enviar dato a Peer: "<< out_aux.dest_Transport << " - TIME_clock1: "<< clock1<< std::endl;}
						assert(out_aux.src_Transport != out_aux.dest_Transport);
						get_messages<typename Pastry_node_Simple_defs::DataOutDown>(bags).push_back(out_aux);
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

		friend std::ostringstream& operator<<(std::ostringstream& os, const typename Pastry_node_Simple<TIME>::state_type& i) {
			//~ os << "ackNum: " << i.ackNum; 
			 os << "Estado: " << i.estado << ". next_internal: " << i.next_internal; 
		return os;
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
		// Process Msg
		//====================================================================================================================================
		// ALTERNATIVA CON 1 COLA, PERO CON MENSAJES PASTRY ORIGINALES.		
		double process_msg(PastryMessage m)
		{	
			if (debug){cout << "======================================================================" << endl;
			std::cout << "Pastry_node_Simple " << ID_numb << " -  Process MSG. MSG Type:" << m.getType() << " . id_tarea: "<< m.id_tarea << std::endl;
			//~ cout << "~"<<m.id_tarea<<"-Pastry_node_Simple"<< ID_numb << " - Process Message. Msg Type:" << m.getType() << " . MSG Source: "<< m.src_Transport <<  " . MSG Dest: "<< m.dest_Transport << endl;
			cout << "======================================================================" << endl;}		
			double processing_time=0;
			if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Type: "<< m.getType()<< " - Process MSG. MSG Transport Src: " << m.src_Transport<< " - MSG Transport Dest: " << m.dest_Transport<< std::endl;}
			if ( m.src_Transport != m.dest_Transport){		//Control para determinar si el mensaje viene de la capa superior o de la red.
				m.setType(PASTRY_RECEIVE);
				if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Mensaje de la capa inferior. Cambiar mensaje a PASTRY_RECEIVE."<< std::endl;	}
			}
			//~ cout << "==========================================================================================================" << endl;
			//~ std::cout << "Pastry_node_Simple POST CAMBIO " << ID_numb << " -  Process MSG. MSG Type:" << m.getType() << " . id_tarea: "<< m.id_tarea << std::endl;
			switch (m.getType())
			{			
				case PASTRY_SENDDIRECT:
				{	
					//~ std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Pastry PASTRY_SENDDIRECT."<< std::endl;
					TLCMessage temp =m.getData();
					//~ printPath(temp);
					if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Pastry PASTRY_SENDDIRECT."<< std::endl;}
					int src_Transport = Node->ne->getIP(); // m->src;
					int dest_Transport = m.getDestEntry()->getIP();
					m.dest = Node->ne -> getNodeID();
					m.src_Transport=src_Transport;			//Reemplazo de campos de MessageT
					m.dest_Transport=dest_Transport;		//Reemplazo de campos de MessageT
					if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - src_Transport: " << src_Transport << ". dest_Transport: "<< dest_Transport << std::endl;}
					state.Msg_list_down.push_back(m);
					break;
				}
		
				case PASTRY_SENDDHT:
				{
					//~ std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple" << ID_numb << ". Pastry SEND_DHT."<< std::endl;
					TLCMessage temp =m.getData();
					//~ printPath(temp);
					int src_Transport = Node->ne->getIP();
					NodeEntry* nextStep = route(m.dest);
					if(BN_cmp(NODEID, nextStep->getNodeID())!=0)
					{
						int dest_Transport = nextStep->getIP();
						m.setDestEntry(nextStep);	
						m.src_Transport=src_Transport;			//Reemplazo de campos de MessageT
						m.dest_Transport=dest_Transport;		//Reemplazo de campos de MessageT
						if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Send MSG through SEND_DHT. Dest-node: " << dest_Transport << std::endl;}
						state.Msg_list_down.push_back(m);
					}
					else 
					{
						m.finalDestination=1;
						if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Pastry SEND_DHT. This node is the final Destination - Forward to upper layer " << std::endl;}
						//~ std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple" << ID_numb << ". Pastry receive."<< std::endl;
						TLCMessage temp =m.getData();
						//~ printPath(temp);
						if (0){cout << "TLC msg "<< ID_numb <<" - temp->type" << temp.type << endl;
						cout << "TLC msg "<< ID_numb <<" - temp->src->getIP():" << temp.src->getIP() << endl;
						cout << "TLC msg "<< ID_numb <<" - temp->src->getNodeID():" << BN_bn2dec(temp.src->getNodeID()) << endl;
						cout << "temp->query->INFO:"<< endl;
						temp.query->printQuery();}
						if(m.finalDestination)
						{temp.setFinalDest();}
						state.Msg_list_up.push_back(temp);					
						if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Pastry RECEIVE - Mensaje TLC enviado a la upper layer en la Output Function" << std::endl;}
					}
					break;
				}
		
				case PASTRY_RECEIVE: 
				{	
					/* Send message to the upperLayer through the function deliver */
					if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Pastry RECEIVE. Call Route " << std::endl;}
					NodeEntry* nextStep = route(m.dest);
					if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Compare - NODEID: " << BN_bn2dec(NODEID) << ", nextStep->getNodeID()"<< BN_bn2dec(nextStep->getNodeID())<<  std::endl;}
					if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Compare - IP: " << state.Node->ip << ", nextStep->getIP()"<< nextStep->getIP() << std::endl;}
					if(BN_cmp(NODEID, nextStep->getNodeID())==0){	
						if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Pastry RECEIVE. This node is the final Destination - Forward to upper layer " << std::endl;}
						m.finalDestination = true;	
					}
					else
					{
						if (debug){std::cout << "~"<<m.id_tarea<<"-Pastry_node_Simple " << ID_numb << " - Pastry RECEIVE - Not Final Destination. Route to: " <<  nextStep->getIP() << std::endl;}
						m.finalDestination = false;	
					}
					TLCMessage temp =m.getData();
					//~ printPath(temp);
					if (debug){cout << "temp->type" << temp.type << endl;
					cout << "temp->src->getIP():" << temp.src->getIP() << endl;
					cout << "temp->src->getNodeID():" << BN_bn2dec(temp.src->getNodeID()) << endl;
					cout << "temp->query->INFO:"<< endl;
					cout << "temp->dest:" << BN_bn2dec(temp.dest) << endl;
					cout << "FINAL DESTINATION? "<< m.finalDestination << endl;}
					if(m.finalDestination)
					{
						temp.setFinalDest();
					}
					else
					{
						temp.final_destination=false;
					}
					state.Msg_list_up.push_back(temp);
					break;
				}
				
				default:
					fflush(stdout);
			}
			return processing_time;		
		}
		
		
		//====================================================================================================================================
		NodeEntry* route(BIGNUM* dest)
		{	
			//~ std::cout << "Pastry_node_Simple" << ID_numb << ". Enter Route"<< std::endl;
			NodeEntry* nextHop = NULL;
			NodeEntry* nextHopI = NULL;
			NodeEntry* nextHopLS= NULL;
			NodeEntry * nextHopRT = NULL;
			nextHopI = state.Node->ne;
			nextHopLS = state.Node->leafset->checkLeafset(dest);
			nextHopRT = state.Node->rt->checkRoutingTable(dest, state.Node->ne);
			//IM THE RESPONSIBLE
			if( BN_cmp( nextHopI->getNodeID(),dest) == 0 )
			{
				//~ std::cout << "Pastry_node_Simple" << ID_numb << ". I'm Responsible"<< std::endl;
				return nextHopI;
			}
			//NO RT INFO -> CHECK LEAFSET
			if(nextHopRT == NULL){
				//~ std::cout << "Pastry_node_Simple" << ID_numb << ". Check Leafset"<< std::endl;
				nextHop = nextHopLS;
			}
			//CLOSEST AMONG RT AND LS
			if(nextHopRT != NULL && nextHopLS != NULL){
				//~ std::cout << "Pastry_node_Simple" << ID_numb << ". RT or Leafset"<< std::endl;
				nextHop =  checkClosest(dest, nextHopRT, nextHopLS);
			}
			//CLOSEST AMONG ME AND OTHERS OPTIONS
			nextHop = checkClosest(dest, nextHopI, nextHop);
			if (0){std::cout << "Pastry_node_Simple " << ID_numb << " - Closest - NODEID:"<< BN_bn2dec(nextHop->getNodeID()) << ". NODE IP:" << nextHop->getIP() << std::endl;}
			return nextHop;
		}

		//====================================================================================================================================
		NodeEntry* checkClosest(BIGNUM* dest, NodeEntry *a, NodeEntry *b)
		{
		   BIGNUM* d1= state.Node->leafset->absMinDist(dest,a->getNodeID());
		   BIGNUM* d2= state.Node->leafset->absMinDist(dest,b->getNodeID());

		   if(BN_cmp(d1,d2)>0)
		   {
			  BN_free(d1);
			  BN_free(d2);
			  return b;    
		   }
		   else
		   {
			  BN_free(d1);
			  BN_free(d2);
			  return a;
		   }
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


#endif // __Pastry_node_Simple_HPP__
