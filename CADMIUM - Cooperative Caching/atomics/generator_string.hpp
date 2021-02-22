/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* GeneratorString:
* Cadmium implementation of CD++ GeneratorString atomic model
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
    struct controlIn : public in_port<std::string> { };
    //~ struct ackIn : public in_port<Message_t> { };
    struct DataIn : public in_port<Message_t> { };
};

template<typename TIME> class GeneratorString{
    public:
        //Parameters to be overwriten when instantiating the atomic model
        int debug=0;
        TIME   preparationTime;
        TIME   sendTime;
        int ID_numb;
        int nodes;
		char *ptr;
		UTIL *util = new UTIL(); 
		int externalTransitionCounter;
		int Peer_Selection=1;
		int flag_first_query;
        // default constructor
        GeneratorString() noexcept{
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
        }
        
        GeneratorString(int _ID_numb, int _nodes,int _totalQueries) noexcept{
			preparationTime  = TIME("00:00:00");
			sendTime          = TIME("00:00:00");
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			state.model_active     = false;
			ID_numb		= _ID_numb;
			nodes			= _nodes;
			state.flag_db=0;
			state.ID_tarea=1;
			state.totalQueries=_totalQueries;
            state.currentQueries=0;
            externalTransitionCounter=0;
            flag_first_query=0;
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
        }; 
        state_type state;
        
        // ports definition
        using input_ports=std::tuple<typename GeneratorString_defs::controlIn, typename GeneratorString_defs::DataIn>;
        using output_ports=std::tuple<typename GeneratorString_defs::packetSentOut, typename GeneratorString_defs::ackReceivedOut, typename GeneratorString_defs::dataOut>;

        // internal transition
        void internal_transition() {
			state.flag_db=0;
            if (state.model_active){
				if (!state.sending){
					if (state.packetNum <= state.totalPacketNum){		//Si quedan por enviar, envío, si no, model_active=false (o sea, passivate() ).
						//~ state.packetNum ++;							//Lo aumento al salir del E2.
						state.model_active = true; 					
						state.sending = true;							//Cambio del E1 al E2
						if (debug){std::cout << "GeneratorString - IT - E1 -> E2" << std::endl;}
						state.next_internal = TIME("00:00:00");
					} else {
						state.Msg_list.clear();
						if (debug){std::cout << "Passivate GeneratorString" << std::endl;}
						state.model_active = false;						//Cambio del E1 al E0
						state.sending = false;
						state.next_internal = std::numeric_limits<TIME>::infinity();
					}
				} else{
					if (state.packetNum <= state.totalPacketNum){		//Si quedan por enviar, envío, si no, model_active=false (o sea, passivate() ).
						if (debug){std::cout << "GeneratorString - IT - E2 -> E1" << std::endl;}
						state.packetNum ++;
						//~ state.ID_tarea++;
						state.model_active = true; 
						state.sending = false;							//Cambio del E2 al E1
						state.next_internal = TIME("00:00:00");
						if (debug){std::cout << "next_internal: " << state.next_internal << std::endl;}
						
					} else {
						state.Msg_list.clear();
						if (debug){std::cout << "GeneratorString - IT - E2 -> E0" << std::endl;}
						if (debug){std::cout << "Passivate GeneratorString" << std::endl;}
						state.model_active = false;						//Cambio del E2 al E0
						state.sending = false;
						state.next_internal = std::numeric_limits<TIME>::infinity();
					}
				}
            } else{
					if (debug){std::cout << "GeneratorString - IT - E0 -> E1" << std::endl;}
                    state.model_active = true;							//Cambio del E0 al E1
                    state.sending = false;
                    state.next_internal = preparationTime;   			//Tiempo en el E1
                    if (debug){std::cout << "next_internal: " << state.next_internal << std::endl;}
            }   
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            for(const auto &x : get_messages<typename GeneratorString_defs::controlIn>(mbs)){
                if(externalTransitionCounter < state.totalQueries){
					externalTransitionCounter++;
					if (debug){cout << "externalTransitionCounter: "<< externalTransitionCounter << endl;}
					std:string line;
					line = x;
					//~ cout << "GENERATOR STRING - LINE: " << line << endl;
					//~ state.packetNum++;
					state.packetNum=1;
					
					ServerMessage msg;
					Hash* h;
					std::string terms;
					vector<string> tokens;
					tokens.clear();
					util->Tokenize( line, tokens, ";");
					int destination = std::stoi(tokens[0]);
					terms = tokens[3];
					tokens.clear();
					util->Tokenize( terms, tokens, "~");
					ptr = util->obtain_terms( tokens );
					//~ ptr="353535";
					BIGNUM* hashValue = h->GenerateKey(ptr); 
					if (debug){std::cout << "PTR generator: " << ptr << " - Hash: " << BN_bn2dec(hashValue)<<   " - CurrentQueries: " << state.currentQueries<<   std::endl;}
					msg.query =hashValue;
					msg.key=ptr; 
					msg.version=0;
					msg.source= ID_numb;
					msg.tag=0; 
					msg.id_tarea=state.currentQueries; 
					
					//-----------------------------------------------------------------------------------------------
					msg.version=0;
					msg.source= ID_numb;
					msg.tag=0; 
					
					msg.dest=(state.currentQueries)%nodes;  //Debug
					msg.dest=destination;
					//~ msg.dest=0;  //Debug
					if (1){cout << "chosen: " <<msg.dest << endl;}
					//Peer_Selection=1;
					//if (Peer_Selection==0){		//Full random
					//	msg.dest=rand() % nodes; 
					//}else{						//Zipf
					//	rand_val(1);
					//	msg.dest = getZipf(1.0, nodes-1);
					//}
					assert(msg.dest!=nodes);
					//-----------------------------------------------------------------------------------------------
					if (debug){cout << "=============================================================================================================================" << endl;
					std::cout << "Objeto en generator - id_tarea: "<< msg.id_tarea<< ". Key: "<< BN_bn2dec(msg.query) << ". Destino: "<< msg.dest << std::endl;
					cout << "=============================================================================================================================" << endl;}
					state.Msg_list.push_back(msg);
					msg.tag=-1; 	//Envío el mismo mensaje a la DB para llevar registro de las tareas totales.
					state.Msg_list.push_back(msg);
                    state.totalPacketNum = 1;
                    if (state.totalPacketNum > 0 && (state.currentQueries <= state.totalQueries)){
						state.currentQueries++;
                        //~ state.packetNum = 1;
                        state.sending = true;
                        state.model_active = true;
                        state.next_internal = preparationTime;
                        state.flag_db=1;
                    }else{
                        if(state.next_internal != std::numeric_limits<TIME>::infinity()){
                            state.next_internal = state.next_internal - e;
                        }
                    }
                }else{
					state.next_internal = state.next_internal - e;
					//~ std::cout << "=== GeneratorString - Still have pending work - Ignored Control Input ===" << std::endl;
					if (debug){std::cout << "=== GeneratorString - Input number exceeded ===" << std::endl;}
				}
            }
        }

        // confluence transition
        void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
            internal_transition();
            external_transition(TIME(), std::move(mbs));
        }

        // output function
        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bags;
            ServerMessage out;
            if (state.sending){
                for (int i=0; i< state.Msg_list.size(); i++){					
					out = state.Msg_list[i];
					//~ out.id_tarea = state.currentQueries;
					out.source=-1;
					if (out.tag==-1){
						out.dest=nodes+1;
					}
					get_messages<typename GeneratorString_defs::dataOut>(bags).push_back(out);
					
				}
            }else {
				
			}
            return bags;
        }

        // time_advance function
        TIME time_advance() const {  
             return state.next_internal;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename GeneratorString<TIME>::state_type& i) {
            os << "packetNum: " << i.packetNum << " & totalPacketNum: " << i.totalPacketNum; 
        return os;
        }
        
		int getZipf (int alpha, int n)
		{
			srand(1);
			static bool first = true;      // Static first time flag
			static double c = 0;          // Normalization constant
			double z;                     // Uniform random number (0 < z < 1)
			double sum_prob;              // Sum of probabilities
			double zipf_value=0;            // Computed exponential value to be returne
			int i;                        // Loop counter
			// Compute normalization constant on first call only
			if (first == true){
				for (i=1; i <= n; i++)
				c = c + (1.0 / pow((double) i, alpha));
				c = 1.0 / c;
				first = false;
				//cout << "PRIMERO" << endl;
			}
			// Pull a uniform random number (0 < z < 1)
			do
			{
				z = (((double) rand())/(RAND_MAX+1.0));
				// cout << "Z VALUE " << z << endl;
			}
			while ((z == 0) || (z == 1));
			
			// Map z to the value
			sum_prob = 0;
			for (i=1; i<=n; i++){
				//      cout <<"Z " << z << endl;
				//     cout << "SUM PROB: " << sum_prob << endl;
				//      cout << "C " << c <<endl;
				//      cout << "ALPHA " << alpha << endl;
				sum_prob = sum_prob + (c / pow((double) i, alpha));
				if (sum_prob >= z){
					zipf_value = i;
					break;
				}
			}
			//   cout << "ZIPF VALUE " << zipf_value << endl;
			// Assert that zipf_value is between 1 and N
			ASSERT((zipf_value >=1) && (zipf_value <= n));
			return zipf_value ;
		}
		
		double rand_val(int seed)
		{ 
			srand(1);
			const long a =16807;// Multiplier
			const long m = 2147483647;// Modulus
			const long q = 127773;// m div a
			const long r = 2836;// m mod a
			static long x; // Random int value
			long x_div_q; // x divided by q
			long x_mod_q; // x modulo q
			long x_new; // New x value
			// Set the seed if argument is non-zero and then return zero
			if (seed > 0){
				x = seed;
				return(0.0);
			}
			// // RNG using integer arithmetic
			x_div_q = x / q;
			x_mod_q = x % q;
			x_new = (a * x_mod_q) - (r * x_div_q);
			if (x_new > 0)
				x = x_new;
			else
				x = x_new + m;
			// // Return a random value between 0.0 and 1.0
			return((double) x / m);
		}
	
        
        
        //NDTime convertir_tiempo(double _Tin){
		//	int hours = (int) _Tin/3600;
		//	int mins = (int) (_Tin - hours*3600) / 60;
		//	int secs = (int) (_Tin - hours*3600 - mins*60);
		//	int msecs =  1000.0*(_Tin - hours*3600 - mins*60 - secs);
		//	std::string Shours = std::to_string(hours);
		//	std::string Smins = std::to_string(mins);
		//	std::string Ssecs = std::to_string(secs);
		//	std::string Smsecs = std::to_string(msecs);
		//	std::string TotalTime = Shours+":"+Smins+":"+Ssecs+":"+Smsecs;
		//	//std::cout << "_Tin: " << _Tin << std::endl;
		//	//std::cout << "Shours: " << Shours << std::endl;
		//	//std::cout << "Smins: " << Smins << std::endl;
		//	//std::cout << "Ssecs: " << Ssecs << std::endl;
		//	//std::cout << "Smsecs: " << Smsecs << std::endl;
		//	//std::cout << "TotalTime: " << TotalTime << std::endl;
		//	return TotalTime;
		//}
        
};     
#endif // __GeneratorString_HPP__
