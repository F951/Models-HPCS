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
//~ #include "util.hpp"
#include <random>
#include <chrono>
//~ #include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"
//~ #include "../data_structures/TLCmessage.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct ClientSimple_defs{
	struct DataOut : public out_port<ServerMessage> { };
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
		int sentQueries;
		int msgID;
		int param_peer;
		int totalQueries;
		
		// default constructor
		ClientSimple() noexcept{
			preparationTime  = TIME("00:00:00:001");
			state.clock1=TIME("00:00:00");
			state.estado=0;
			state.flag_output=0;
			state.next_internal    = std::numeric_limits<TIME>::infinity();
		}
		 ClientSimple(int _ID_numb, int _nodes, int _param_peer, int _totalQueries) noexcept{
			preparationTime  = TIME("00:00:00:001");
			state.clock1=TIME("00:00:00");
			state.estado=0;
			state.flag_output=0;
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			ID_numb			= _ID_numb;
			nodes = _nodes;	
			param_peer = _param_peer;	
			totalQueries = _totalQueries;	
		}
		
		// state definition
		struct state_type{
			int estado;
			list<ServerMessage> Msg_list;
			TIME next_internal;
			TIME clock1;
			int flag_output;
		}; 
		state_type state;
		
		//ports definition
		using input_ports=std::tuple<typename ClientSimple_defs::DataIn>;
		using output_ports=std::tuple<typename ClientSimple_defs::DataOut>;
		//====================================================================================================================================
		//internal transition
		//====================================================================================================================================
		void internal_transition() {
			assert(state.estado!=0);
			state.clock1+=state.next_internal;
			switch (state.estado)
			{
				case 0:
				{
					//~ state.clock1 += state.next_internal;
					if (0){std::cout << "ClientSimple"<< ID_numb << " - INTERNAL TRANSITION CASO 0 - TIME: " << state.clock1 << " preparationTime "<< preparationTime <<std::endl; }
					//No se debe llegar hasta acá por el assert
					break;
				}
				case 1:
				{
					if (debug){std::cout << "ClientSimple"<< ID_numb << " - Estado 1 - Internal transition." << std::endl; }
					state.Msg_list.pop_front();

					if (state.Msg_list.size()!=0){
						if (debug){std::cout << "ClientSimple"<< ID_numb << " - Voy a estado 1." << std::endl; }
						state.estado=1;
						//~ preparationTime = convertir_tiempo( (ID_numb + state.Msg_list.front().id_tarea)*0.0000001 );
						preparationTime = convertir_tiempo( ((ID_numb/nodes) + 0.1*(state.Msg_list.front().id_tarea%(totalQueries/2)/(double)totalQueries) )*10 );
						//~ preparationTime=get_random_time_exp(3,0.1);
						//~ preparationTime=convertir_tiempo(0.5);
						//~ preparationTime=convertir_tiempo(param_peer);
						state.next_internal=preparationTime;
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
			int flag_dataIn=0;
			int aux_source,aux_dest;
			state.clock1+=e;
			//~ if (1){std::cout << "ClientSimple"<< ID_numb << " - External Transition - TIME: " << state.clock1 << std::endl; }
			for( auto &x : get_messages<typename ClientSimple_defs::DataIn>(mbs)){
				if (x.dest==ID_numb){
					process_msg(x);		
					flag_dataIn=1;
					//~ state.Msg_list.push_back(x_done);				
				}				
			}

			if (flag_dataIn==1){
				switch (state.estado)
				{
					case 0:
					{
						state.estado=1;
						//preparationTime = convertir_tiempo( (ID_numb + state.Msg_list.front().id_tarea)*0.0000001 );
						preparationTime = convertir_tiempo( ((ID_numb/nodes) + 0.1*(state.Msg_list.front().id_tarea%(totalQueries/2)/(double)totalQueries) )*10 );
						//~ preparationTime=get_random_time_exp(3,0.1);
						//~ preparationTime=convertir_tiempo(0.5);
						//~ preparationTime=convertir_tiempo(param_peer);
						state.next_internal=preparationTime;
						//~ state.clock1+=e;	//Ya lo sumo arriba, al principio de la ET
						if (0){std::cout << "ClientSimple"<< ID_numb << " - ET - Programo un nuevo delay - TIME: " << state.clock1 <<". DELAY: "<< preparationTime<< " prox ev: " << state.clock1 + state.next_internal << std::endl; }
						break;
					}
					case 1:
					{
						state.estado=1;
						if (preparationTime >= e){
							preparationTime = preparationTime - e;
							state.next_internal=preparationTime;
						}
						if (0){std::cout << "ClientSimple"<< ID_numb << " - ET - completo el delay anterior - Actual: " << state.clock1 <<". DELAY: "<< state.next_internal << " prox ev: " << state.clock1 + state.next_internal << std::endl; }
						break;
					}
				}				
			}else{
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
			external_transition(TIME(), move(mbs));
			
		}
		
		//====================================================================================================================================
		// output function
		//====================================================================================================================================
		typename make_message_bags<output_ports>::type output() const {
			typename make_message_bags<output_ports>::type bags;
			if(state.estado==0)
			{
				//Do nothing
			}
			else if (state.estado==1){
				ServerMessage out_aux;
				out_aux = state.Msg_list.front();
				if (debug){std::cout << "ClientSimple_node"<< ID_numb << " - OUTPUT. ID destino: " << out_aux.dest << " - source: " << out_aux.source <<" - TIME: " << state.clock1 << " - NEXT_INT: "<< state.next_internal<< " - ACTUAL: "<< state.clock1 + state.next_internal<< std::endl; }
				get_messages<typename ClientSimple_defs::DataOut>(bags).push_back(out_aux);
			}

			return bags;
		}
		
		//====================================================================================================================================
		// time_advance function
		//====================================================================================================================================
		TIME time_advance() const {  
			if (debug){std::cout << "ClientSimple"<< ID_numb << " - TIMEADVANCE ########## - TIME: " << state.clock1 <<". preparationTime: "<< preparationTime<<". next_internal: "<< state.next_internal<< " prox ev: " << state.clock1 + state.next_internal << std::endl; }
			return state.next_internal;
		  //~ TIME next_internal;
		  //~ next_internal = preparationTime;
		  //~ return next_internal;
		}
		
		//====================================================================================================================================
		// Process Msg
		//====================================================================================================================================
		double process_msg(ServerMessage temp)
		{
			if (debug){cout << "~"<<temp.id_tarea<<"-======================================================================" << endl;
			cout << "~"<<temp.id_tarea<<"-ClientSimple" << ID_numb << ". Process Message. Message TAG: " << temp.tag << endl;
			cout << "~"<<temp.id_tarea<<"-======================================================================" << endl;}
			assert(temp.tag==0);
			if (debug){cout << "~"<<temp.id_tarea<<"-ClientSimple" << ID_numb << " (responsible) wants object from server. Send object request to Server." << endl;}
			ServerMessage wseQuery;
			wseQuery.query=temp.query;
			wseQuery.latencia=temp.latencia;
			wseQuery.query_TLC=temp.query_TLC;
			wseQuery.key=temp.key;
			wseQuery.source=ID_numb;
			wseQuery.tag=1;
			wseQuery.dest=temp.source;
			wseQuery.id_tarea=temp.id_tarea;
			wseQuery.size=temp.size;
			state.Msg_list.push_back(wseQuery);
			//~ return processing_time;
			return 0;			
		}
		
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
		
		NDTime convertir_tiempo(double _Tin){
			int hours = (int) _Tin/3600;
			int mins = (int) (_Tin - hours*3600) / 60;
			int secs = (int) (_Tin - hours*3600 - mins*60);
			int msecs =  1000.0*(_Tin - hours*3600 - mins*60 - secs);
			std::string Shours = std::to_string(hours);
			std::string Smins = std::to_string(mins);
			std::string Ssecs = std::to_string(secs);
			std::string Smsecs = std::to_string(msecs);
			std::string TotalTime = Shours+":"+Smins+":"+Ssecs+":"+Smsecs;
			return TotalTime;
		}
		
		NDTime get_random_time_exp(double par, double multiplicador){
			//------------------------------------------------------------------------------
			int seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::default_random_engine generator (seed);
			std::exponential_distribution<double> distribution(par);
			double delay = multiplicador*distribution(generator);
			NDTime ND_delay  = convertir_tiempo(delay);
			//~ if (debug){std::cout << "ND_delay: " << ND_delay<< ". Delay: " << delay << std::endl;}
			//------------------------------------------------------------------------------
			return ND_delay;
		}
		
		friend std::ostringstream& operator<<(std::ostringstream& os, const typename ClientSimple<TIME>::state_type& i) {
			//~ os << "ackNum: " << i.ackNum; 
			 os << "Estado: " << i.estado << ". next_internal: " << i.next_internal; 
		return os;
		}
	};     


#endif // __ClientSimple_HPP__
