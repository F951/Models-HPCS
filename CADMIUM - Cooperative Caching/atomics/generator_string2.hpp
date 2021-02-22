/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* GeneratorString_defs:
* Cadmium implementation of CD++ GeneratorString_defs atomic model
*/

#ifndef __GeneratorString_HPP__
#define __GeneratorString_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <chrono>
#include "util.hpp"
#include "../auxiliar/Hash.h"
#include "../glob.h"
#include "../auxiliar/Util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

#include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct GeneratorString_defs{
	struct packetSentOut : public out_port<int> { };
    struct ackReceivedOut : public out_port<int> {};
	struct dataOut : public out_port<ServerMessage> { };
	//~ struct dataOutDB : public out_port<ServerMessage> { };
	//~ struct doneOut : public out_port<bool> { };
	struct DataIn : public in_port<ServerMessage> { };
	struct controlIn : public in_port<std::string> { };	//Added
    
};

template<typename TIME> class GeneratorString{
	public:
		
		  //Parameters to be overwriten when instantiating the atomic model
        int debug=0;
        int debug_clock=0;
        TIME   preparationTime;
        TIME   sendTime;
        int ID_numb;
        int nodes;
		char *ptr;
		UTIL *util = new UTIL(); 
		int externalTransitionCounter;
		int Peer_Selection=1;
		int flag_first_query;
		TIME clock1;
        // default constructor
        GeneratorString() noexcept{
			clock1 = TIME("00:00:00");
			preparationTime  = TIME("00:00:00");
			sendTime          = TIME("00:00:00");
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			state.model_active     = false;
			state.flag_db=0;
			state.ID_tarea=1;
			state.totalQueries=0;
            state.currentQueries=0;
            externalTransitionCounter=0;
            flag_first_query=0;
            state.estado=0;
        }
        
        GeneratorString(int _ID_numb, int _nodes,int _totalQueries) noexcept{
			preparationTime  = TIME("00:00:00");
			//~ sendTime          = TIME("00:00:00");
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			//~ state.model_active     = false;
			ID_numb		= _ID_numb;
			nodes			= _nodes;
			//~ state.flag_db=0;
			state.ID_tarea=1;
			state.totalQueries=_totalQueries;
            state.currentQueries=0;
            externalTransitionCounter=0;
            flag_first_query=0;
            state.estado=0;
		}
        
        // state definition
        struct state_type{
			vector<ServerMessage> Msg_list;
            int packetNum;
            int totalPacketNum;
            bool sending;
            bool model_active;
            int flag_db;
            TIME next_internal;
            int ID_tarea;
            int totalQueries;
            int currentQueries;
            int estado;
        }; 
        state_type state;
        
        //============================================================================================================
        
		//ports definition
		using input_ports=std::tuple<typename GeneratorString_defs::controlIn, typename GeneratorString_defs::DataIn>;
        //~ using output_ports=std::tuple<typename GeneratorString_defs::dataOut>;
		using output_ports=std::tuple<typename GeneratorString_defs::packetSentOut, typename GeneratorString_defs::ackReceivedOut, typename GeneratorString_defs::dataOut>;
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
					if (debug){std::cout << "GeneratorString_defs"<< ID_numb << " - Estado 1 - Internal transition." << std::endl; }
					//~ state.Msg_list_down.clear();
					//~ state.Msg_list_up.clear();
					//~ state.Msg_list_DB.clear();
					state.Msg_list.clear();
					if (state.Msg_list.size()!=0){
						if (debug){std::cout << "GeneratorString_defs"<< ID_numb << " - Voy a estado 1." << std::endl; }
						state.estado=1;
						state.next_internal=preparationTime;
						//~ clock1 = clock1 + state.next_internal;
					}else{
						if (debug){std::cout << "GeneratorString_defs"<< ID_numb << " - Voy a estado 0." << std::endl; }
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
			clock1+=e;
			if (debug_clock){cout << "GeneratorString_defs "<< ID_numb << " - ET - Nuevo Clock1: " << clock1 << endl;}
			for( auto &x : get_messages<typename GeneratorString_defs::controlIn>(mbs)){
				if(externalTransitionCounter < state.totalQueries){
					externalTransitionCounter++;
					if (debug){cout << "externalTransitionCounter: "<< externalTransitionCounter << endl;}
					std:string line;
					line = x;
					//~ cout << "GENERATOR STRING - LINE: " << line << endl;
					ServerMessage msg;
					Hash* h;
					std::string terms;
					vector<string> tokens;
					
					tokens.clear();
					util->Tokenize( line, tokens, ";");
					int destination = std::stoi(tokens[0]);
					msg.dest=destination;					
					if (1){cout << "chosen: " <<msg.dest << endl;}
					assert(msg.dest!=nodes);
					
					terms = tokens[3];
					tokens.clear();
					util->Tokenize( terms, tokens, "~");
					ptr = util->obtain_terms( tokens );
					BIGNUM* hashValue = h->GenerateKey(ptr); 
					if (debug){std::cout << "PTR generator: " << ptr << " - Hash: " << BN_bn2dec(hashValue)<<   " - CurrentQueries: " << state.currentQueries<<   std::endl;}
					
					msg.query =hashValue;
					msg.key=ptr; 
					msg.version=0;
					msg.source= ID_numb;
					msg.tag=0; 
					msg.id_tarea=state.currentQueries; 
					msg.version=0;
					msg.source= ID_numb;
					msg.tag=0; 
					
					if (debug){cout << "=============================================================================================================================" << endl;
					std::cout << "Objeto en generator - id_tarea: "<< msg.id_tarea<< ". Key: "<< BN_bn2dec(msg.query) << ". Destino: "<< msg.dest << std::endl;
					cout << "=============================================================================================================================" << endl;}
					state.Msg_list.push_back(msg);
					//~ msg.tag=-1; 	//Envío el mismo mensaje a la DB para llevar registro de las tareas totales.
					//~ state.Msg_list.push_back(msg);
                    state.totalPacketNum = 1;
                    if (state.totalPacketNum > 0 && (state.currentQueries <= state.totalQueries)){
						state.currentQueries++;
					}
				}
				else
				{
					//Don't add the message to the queue
				}
				
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
			//~ ServerMessage out_aux_DB;
			//~ for (int i=0; i< state.Msg_list_DB.size(); i++){					
				//~ out_aux_DB = state.Msg_list_DB[i];
				//~ if (debug){std::cout << "GeneratorString_defs_node"<< ID_numb << " - Output function. Send data to DB - Time_aux: "<< Time_aux<< std::endl;}
				//~ get_messages<typename GeneratorString_defs::dataOutDB>(bags).push_back(out_aux_DB);
			//~ }	
			if(state.estado==0)
			{
				//Do nothing
			}
			else if (state.estado==1){
				ServerMessage out;
                for (int i=0; i< state.Msg_list.size(); i++){					
					out = state.Msg_list[i];
					out.source=-1;
					if (out.tag==-1){
						out.dest=nodes+1;
					}
					//~ cout << "Generator send query to peer: " << out.dest << ". Time: " << Time_aux << endl;     
					get_messages<typename GeneratorString_defs::dataOut>(bags).push_back(out);
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
		
		friend std::ostringstream& operator<<(std::ostringstream& os, const typename GeneratorString<TIME>::state_type& i) {
			//~ os << "ackNum: " << i.ackNum; 
			 os << "Estado: " << i.estado << ". next_internal: " << i.next_internal; 
		return os;
		}
	};     


#endif // __GeneratorString_HPP__
