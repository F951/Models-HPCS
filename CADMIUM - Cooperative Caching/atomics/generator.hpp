/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* Generator:
* Cadmium implementation of CD++ Generator atomic model
*/

#ifndef __Generator_HPP__
#define __Generator_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <chrono>
#include "util.hpp"

#include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Generator_defs{
    struct packetSentOut : public out_port<int> { };
    struct ackReceivedOut : public out_port<int> {};
    struct dataOut : public out_port<ServerMessage> { };
    struct controlIn : public in_port<int> { };
    //~ struct ackIn : public in_port<Message_t> { };
    struct DataIn : public in_port<Message_t> { };
};

template<typename TIME> class Generator{
    public:
        //Parameters to be overwriten when instantiating the atomic model
        int debug=0;
        TIME   preparationTime;
        TIME   sendTime;
        int ID_numb;
        int nodes;
        // default constructor
        Generator() noexcept{
			preparationTime  = TIME("00:00:10");
			sendTime          = TIME("00:00:05");
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			state.model_active     = false;
			state.flag_db=0;
			state.ID_tarea=1;
        }
        
        Generator(int _ID_numb, int _nodes) noexcept{
			preparationTime  = TIME("00:00:10");
			sendTime          = TIME("00:00:05");
			state.next_internal    = std::numeric_limits<TIME>::infinity();
			state.model_active     = false;
			ID_numb		= _ID_numb;
			nodes			= _nodes;
			state.flag_db=0;
			state.ID_tarea=1;
        }
        
        // state definition
        struct state_type{
            int packetNum;
            int totalPacketNum;
            bool sending;
            bool model_active;
            int flag_db;
            TIME next_internal;
            int ID_tarea;
        }; 
        state_type state;
        
        // ports definition
        using input_ports=std::tuple<typename Generator_defs::controlIn, typename Generator_defs::DataIn>;
        using output_ports=std::tuple<typename Generator_defs::packetSentOut, typename Generator_defs::ackReceivedOut, typename Generator_defs::dataOut>;

        // internal transition
        void internal_transition() {
			state.flag_db=0;
            if (state.model_active){
				if (!state.sending){
					if (state.packetNum <= state.totalPacketNum){		//Si quedan por enviar, envío, si no, model_active=false (o sea, passivate() ).
						//~ state.packetNum ++;							//Lo aumento al salir del E2.
						state.model_active = true; 					
						state.sending = true;							//Cambio del E1 al E2
						//std::cout << "Generator - IT - E1 -> E2" << std::endl;
						//------------------------------------------------------------------------------
						int seed = std::chrono::system_clock::now().time_since_epoch().count();
						std::default_random_engine generator (seed);
						std::exponential_distribution<double> distribution(1);
						double delay = 100*distribution(generator);
						//convertir_tiempo(delay);
						sendTime = convertir_tiempo(delay);									/*** Tiempo random***/
						if (debug){std::cout << "Send time: " << sendTime<< ". Delay: " << delay << std::endl;}
						state.next_internal = sendTime;   				//Tiempo en el E2
						//------------------------------------------------------------------------------
						
					} else {
						std::cout << "Passivate Generator" << std::endl;
						state.model_active = false;						//Cambio del E1 al E0
						state.sending = false;
						//std::cout << "Generator - IT - E1 -> E0" << std::endl;
						state.next_internal = std::numeric_limits<TIME>::infinity();
					}
				} else{
					if (state.packetNum <= state.totalPacketNum){		//Si quedan por enviar, envío, si no, model_active=false (o sea, passivate() ).
						state.packetNum ++;
						//~ state.ID_tarea++;
						//Random Seed-------------------------------------------------
						int seed = std::chrono::system_clock::now().time_since_epoch().count();
						std::default_random_engine generator (seed);
						std::binomial_distribution<int> distribution1(100,0.5);
						state.ID_tarea = distribution1(generator);
						state.model_active = true; 
						state.sending = false;							//Cambio del E2 al E1
						//std::cout << "Generator - IT - E2 -> E1" << std::endl;
						//------------------------------------------------------------------------------
						//~ int seed = std::chrono::system_clock::now().time_since_epoch().count();
						//~ std::default_random_engine generator (seed);
						std::exponential_distribution<double> distribution(1);
						double delay = 100*distribution(generator);
						//convertir_tiempo(delay);
						sendTime = convertir_tiempo(delay);									/*** Tiempo random***/
						if (debug){std::cout << "Send time: " << sendTime<< ". Delay: " << delay << std::endl;}
						state.next_internal = sendTime;   		//Tiempo en el E1
						//------------------------------------------------------------------------------
					} else {
						//std::cout << "Generator - IT - E2 -> E0" << std::endl;
						std::cout << "Passivate Generator" << std::endl;
						state.model_active = false;						//Cambio del E2 al E0
						state.sending = false;
						state.next_internal = std::numeric_limits<TIME>::infinity();
					}
				}
            } else{
					//std::cout << "Generator - IT - E0 -> E1" << std::endl;
                    state.model_active = true;							//Cambio del E0 al E1
                    state.sending = false;
                    state.next_internal = preparationTime;   			//Tiempo en el E1
            }   
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            for(const auto &x : get_messages<typename Generator_defs::controlIn>(mbs)){
                if(state.model_active == false){
                    state.totalPacketNum = x;
                    if (state.totalPacketNum > 0){
                        state.packetNum = 1;
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
					std::cout << "=== Generator - Still have pending work - Ignored Control Input ===" << std::endl;
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
                out.id_tarea = state.ID_tarea;
                //~ out.bit = 0;
                out.source=-1;
                out.dest=nodes;
                get_messages<typename Generator_defs::dataOut>(bags).push_back(out);
                //~ get_messages<typename Generator_defs::packetSentOut>(bags).push_back(state.totalPacketNum);
                //std::cout << "=== Generator - Send output message - Packet Nr: " <<  state.packetNum << std::endl;
                if ( state.flag_db==1) {
					out.id_tarea = state.ID_tarea;
					//~ out.bit = 0;
					out.source=-1;
					out.dest=nodes;
					get_messages<typename Generator_defs::packetSentOut>(bags).push_back(state.totalPacketNum);
				}    
            }else {
				
			}
            return bags;
        }

        // time_advance function
        TIME time_advance() const {  
             return state.next_internal;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Generator<TIME>::state_type& i) {
            os << "packetNum: " << i.packetNum << " & totalPacketNum: " << i.totalPacketNum; 
        return os;
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
#endif // __Generator_HPP__
