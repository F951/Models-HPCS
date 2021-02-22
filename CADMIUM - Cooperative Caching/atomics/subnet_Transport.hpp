/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* Subnet_Transport:
* Cadmium implementation of CD++ Subnet_Transport atomic model
*/

#ifndef _Subnet_Transport_HPP__
#define _Subnet_Transport_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <vector>

#include "../data_structures/message.hpp"
#include "../data_structures/PastryMessage.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Subnet_Transport_defs{
    struct out : public out_port<PastryMessage> {};
    struct in : public in_port<PastryMessage> {};
};

template<typename TIME> class Subnet_Transport{
public:
    // ports definition
    using input_ports=tuple<typename Subnet_Transport_defs::in>;
    using output_ports=tuple<typename Subnet_Transport_defs::out>;
    // state definition
    struct state_type{
        bool transmitting;
        PastryMessage packet;
        int index;
        //vector<PastryMessage> Msg_list;
        list<PastryMessage> Msg_list;
        TIME clock1;
        TIME next_internal;
    }; 
    int debug=0;
    int debug_clock=0;
    TIME   preparationTime;
    TIME   preparationTime_INIT;
    state_type state;    
    // default constructor
    Subnet_Transport() {
        state.transmitting = false;
        state.next_internal= numeric_limits<TIME>::infinity();
        state.index        = 0;
        preparationTime_INIT=TIME("00:00:00:000:010");
        preparationTime=TIME("00:00:00:000:010");
        state.clock1=TIME("00:00:00");
        state.Msg_list.clear();
    }     
    
    //========================================================================================================================================
    // internal transition
    //========================================================================================================================================
	void internal_transition() {
		if (debug){std::cout << "Internal Transition en Subnet_Transport. Eliminar mensajes de la lista de pendientes. Size lista: "<< state.Msg_list.size()  << std::endl; }
        assert (state.transmitting == true);
        state.Msg_list.pop_front();
        preparationTime=preparationTime_INIT;
        state.clock1 = state.clock1 + preparationTime;
        if ((int)state.Msg_list.size()==0){
			state.transmitting=false;  		//Estado = Passive (0)
			state.next_internal= numeric_limits<TIME>::infinity();
		}else{
			state.transmitting=true;  		//Estado = Active (1)
			state.next_internal = preparationTime;
		}

		if (debug){cout << "Transport Net - clock IT: " << state.clock1<< endl;}
    }
    
    //========================================================================================================================================
    // external transition
    //========================================================================================================================================
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
        vector<PastryMessage> bag_port_in;
        bag_port_in = get_messages<typename Subnet_Transport_defs::in>(mbs);
		state.clock1+=e;
		if (debug_clock){cout << "Transport Net - ET - Clock1: " << state.clock1<< ". Elapsed Time: " << e  << ". Next_interal previo: " << preparationTime << endl;}
		for (int i=0; i<bag_port_in.size(); i++){
			state.index ++;
			state.Msg_list.push_back(bag_port_in[i]);                     
		}
		if (state.transmitting==false){
			state.next_internal=preparationTime;
		}else{
			if (preparationTime > e){
				//~ state.next_internal=preparationTime - e;
				preparationTime = preparationTime - e;
				state.next_internal=preparationTime;
			}
		}
		state.transmitting = true;	
		


    }
    
    //========================================================================================================================================
    // confluence transition
    //========================================================================================================================================
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(TIME(), move(mbs));
    }
    
    //========================================================================================================================================
    // output function
    //========================================================================================================================================
    typename make_message_bags<output_ports>::type output() const {
		typename make_message_bags<output_ports>::type bags;
		vector<PastryMessage> bag_port_out;
		PastryMessage MSG;
		MSG = state.Msg_list.front();
		if (debug){cout << "Transport Net - Output Function - state.Msg_list.size():" << state.Msg_list.size() << ". State.next_internal: "<< state.next_internal<< endl;}
		get_messages<typename Subnet_Transport_defs::out>(bags).push_back(MSG);
		return bags;
    }
    
    //========================================================================================================================================
    // time_advance function
    //========================================================================================================================================
    TIME time_advance() const {
        TIME next_internal;
        next_internal=state.next_internal;
        return next_internal;
    }
    
    //========================================================================================================================================
    //Otras funciones
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

    friend ostringstream& operator<<(ostringstream& os, const typename Subnet_Transport<TIME>::state_type& i) {
        os << "index: " << i.index << " & transmitting: " << i.transmitting; 
        return os;
    }
};    
#endif // _Subnet_Transport_HPP__
