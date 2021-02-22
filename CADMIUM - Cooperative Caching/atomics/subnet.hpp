/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* Subnet:
* Cadmium implementation of CD++ Subnet atomic model
*/

#ifndef _SUBNET_HPP__
#define _SUBNET_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <vector>

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Subnet_defs{
    struct out : public out_port<Message_t> {};
    struct in : public in_port<Message_t> {};
};

template<typename TIME> class Subnet{
    public:
    // ports definition
    using input_ports=tuple<typename Subnet_defs::in>;
    using output_ports=tuple<typename Subnet_defs::out>;
    // state definition
    struct state_type{
        bool transmitting;
        Message_t packet;
        int index;
        vector<Message_t> Msg_list;
    }; 
    state_type state;    
    // default constructor
    Subnet() {
        state.transmitting = false;
        state.index        = 0;
        state.Msg_list.clear();
    }     
    // internal transition
    void internal_transition() {
		//std::cout << "Internal Transition en Subnet. Eliminar mensajes de la lista de pendientes. Size lista: "<< state.Msg_list.size()  << std::endl; 
        state.transmitting = false;  
		state.Msg_list.clear();
		//std::cout << "Size lista: "<< state.Msg_list.size()  << std::endl; 		//Esto me servía para controlar que los mensajes se eliminan correctamente.
    }
    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
        vector<Message_t> bag_port_in;
        bag_port_in = get_messages<typename Subnet_defs::in>(mbs);
        //~ if(bag_port_in.size()>1) assert(false && "One message at a time");            
        state.transmitting = false;

        for (int i=0; i<bag_port_in.size(); i++){
			state.index ++;
			std::cout << "Mensaje en Subnet. Source: " << bag_port_in[i].source << ". Dest: " << bag_port_in[i].dest << ". PacketNum: " << bag_port_in[i].packet << std::endl; 
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
		vector<Message_t> bag_port_out;         
		Message_t MSG;
		for (int i=0; i<state.Msg_list.size(); i++){
			MSG = state.Msg_list[i];
			bag_port_out.push_back(MSG);
		}
		get_messages<typename Subnet_defs::out>(bags) = bag_port_out; 
		return bags;
    }
    // time_advance function
    TIME time_advance() const {
        TIME next_internal;
        if (state.transmitting) {            
            next_internal = TIME("00:00:03:000");
        }else {
            next_internal = numeric_limits<TIME>::infinity();
        }    
        return next_internal;
    }

    friend ostringstream& operator<<(ostringstream& os, const typename Subnet<TIME>::state_type& i) {
        os << "index: " << i.index << " & transmitting: " << i.transmitting; 
        return os;
    }
};    
#endif // _SUBNET_HPP__
