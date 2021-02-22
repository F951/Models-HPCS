/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* Subnet_ISP:
* Cadmium implementation of CD++ Subnet_ISP atomic model
*/

#ifndef _Subnet_ISP_HPP__
#define _Subnet_ISP_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <vector>

//~ #include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Subnet_ISP_defs{
    struct out : public out_port<ServerMessage> {};
    struct in : public in_port<ServerMessage> {};
};

template<typename TIME> class Subnet_ISP{
    public:
    // ports definition
    using input_ports=tuple<typename Subnet_ISP_defs::in>;
    using output_ports=tuple<typename Subnet_ISP_defs::out>;
    // state definition
    struct state_type{
        bool transmitting;
        ServerMessage packet;
        int index;
        vector<ServerMessage> Msg_list;
        TIME clock1;
        
    }; 
    state_type state; 
    int debug=0;   
    int debug_clock=0;   
    TIME   preparationTime;
    
    // default constructor
    Subnet_ISP() {
        state.transmitting = false;
        state.index        = 0;
        preparationTime=TIME("00:00:00");        
        state.Msg_list.clear();
    }     
    // internal transition
    void internal_transition() {
		//std::cout << "Internal Transition en Subnet_ISP. Eliminar mensajes de la lista de pendientes. Size lista: "<< state.Msg_list.size()  << std::endl; 
        state.transmitting = false;  
		state.Msg_list.clear();
		state.clock1 = state.clock1 + preparationTime;
		if (debug_clock){cout << "ISP Net - clock IT: " << state.clock1<< endl;}
		//std::cout << "Size lista: "<< state.Msg_list.size()  << std::endl; 		//Esto me servía para controlar que los mensajes se eliminan correctamente.
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
        vector<ServerMessage> bag_port_in;
        bag_port_in = get_messages<typename Subnet_ISP_defs::in>(mbs);
        //~ if(bag_port_in.size()>1) assert(false && "One message at a time");            
        state.transmitting = false;
		if (debug_clock){cout << "ISP Net - ET - Clock1: " << state.clock1<< ". Elapsed Time: " << e  << ". Next_interal previo: " << preparationTime << endl;}
		state.clock1+=e;
		if (debug_clock){cout << "ISP Net - ET - Nuevo Clock1: " << state.clock1 << endl;}
		//~ preparationTime=TIME("00:00:00:001");
		preparationTime=TIME("00:00:00:00");
		if (debug_clock){cout << "ISP Net - clock ET: " << state.clock1<< endl;}
		if(debug){std::cout << "Mensaje en Subnet_ISP."  << std::endl; }
        for (int i=0; i<bag_port_in.size(); i++){
			state.index ++;
			if(debug){std::cout << "Mensaje en Subnet_ISP. Source: " << bag_port_in[i].source << ". Dest: " << bag_port_in[i].dest << ". id_tarea: " << bag_port_in[i].id_tarea << std::endl; }
			//~ if ((double)rand() / (double) RAND_MAX  < 0.95){                
			state.transmitting = true;  
			state.Msg_list.push_back(bag_port_in[i]);
			//~ }else{
				
			//~ }                              
		}
    }
    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }
    // output function
    typename make_message_bags<output_ports>::type output() const {
		typename make_message_bags<output_ports>::type bags;
		if(debug){std::cout << "Subnet_ISP. Función de salida " << std::endl; }		
		vector<ServerMessage> bag_port_out;         
		ServerMessage MSG;
		for (int i=0; i<state.Msg_list.size(); i++){
			MSG = state.Msg_list[i];
			bag_port_out.push_back(MSG);
		}
		get_messages<typename Subnet_ISP_defs::out>(bags) = bag_port_out; 
		return bags;
    }
    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
        if (state.transmitting) {            
            //~ next_internal = TIME("00:00:03:000");
			next_internal = preparationTime;
        }else {
            next_internal = numeric_limits<TIME>::infinity();
        }    
        return next_internal;
    }

    friend ostringstream& operator<<(ostringstream& os, const typename Subnet_ISP<TIME>::state_type& i) {
        os << "index: " << i.index << " & transmitting: " << i.transmitting; 
        return os;
    }
};    
#endif // _Subnet_ISP_HPP__
