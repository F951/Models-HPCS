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
#include <random>
#include <chrono>
//~ #include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Subnet_ISP_defs{
    struct out : public out_port<ServerMessage> {};
    struct outDB : public out_port<ServerMessage> {};
    struct in : public in_port<ServerMessage> {};
};

template<typename TIME> class Subnet_ISP{
public:
    // ports definition
    using input_ports=tuple<typename Subnet_ISP_defs::in>;
    using output_ports=tuple<typename Subnet_ISP_defs::out,typename Subnet_ISP_defs::outDB>;
    // state definition
    struct state_type{
        bool transmitting;
        ServerMessage packet;
        int index;
        //vector<ServerMessage> Msg_list;
        list<ServerMessage> Msg_list;
        TIME clock1;
        TIME next_internal;
        double latencia_acc;
        int flag_output;
    }; 
    int debug=0;
    int debug_clock=0;
    TIME   preparationTime;
    int nodes;
    int nro_servers;
    int param_isp;
    
    state_type state;    
    // default constructor
    Subnet_ISP() {
        state.transmitting = false;
        state.next_internal= numeric_limits<TIME>::infinity();
        state.index        = 0;
        preparationTime  = TIME("00:00:00:000");
		state.clock1=TIME("00:00:00");
        state.Msg_list.clear();
        state.latencia_acc=0;
        state.flag_output=0;
    }     
    Subnet_ISP(int _nodes, int _nro_servers, int _param_isp) {
        state.transmitting = false;
        state.next_internal= numeric_limits<TIME>::infinity();
        state.index        = 0;
        preparationTime  = TIME("00:00:00:000");
		state.clock1=TIME("00:00:00");
        state.Msg_list.clear();
        nodes = _nodes;
        nro_servers = _nro_servers;
        param_isp = _param_isp;
        state.latencia_acc=0;
        state.flag_output=0;
    }     
    
    //========================================================================================================================================
    // internal transition
    //========================================================================================================================================
	void internal_transition() {
		assert (state.transmitting == true);
		//~ if (1){std::cout << "ISP_Net -  INTERNAL TRANSITION - TIME: " << state.clock1 << " preparationTime "<< preparationTime <<std::endl; }
		state.clock1+=state.next_internal;
		state.Msg_list.pop_front();
		if ((int)state.Msg_list.size()==0){
			state.transmitting=false;  		//Estado = Passive (0)
			state.next_internal= numeric_limits<TIME>::infinity();
		}else{
			state.transmitting=true;  		//Estado = Active (1)
			preparationTime=convertir_tiempo( state.Msg_list.front().size );		//Proximo HOLD, size del 1er mensaje
			//~ preparationTime=get_random_time_exp(3,0.1);
			//~ preparationTime=convertir_tiempo(param_isp);
			//~ if (1){cout<<"=== ISP_Net - IT - time: "<< state.clock1 <<  ". Proximo Hold: " << preparationTime <<  endl;}
			state.next_internal = preparationTime;
			state.latencia_acc=convert_ND_to_double(preparationTime);
		}
	
    }
    
    
    //========================================================================================================================================
    // external transition
    //========================================================================================================================================
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
        vector<ServerMessage> bag_port_in;
        bag_port_in = get_messages<typename Subnet_ISP_defs::in>(mbs);
        state.clock1+=e;
        //~ if (debug){cout << "ISP_Net - ET "<< endl;}
        if (debug){std::cout << "ISP_Net - EXTERNAL TRANSITION - TIME: " << state.clock1 << " preparationTime "<< preparationTime<< " - elapsed: "<< e <<std::endl; }	
        for (int i=0; i<bag_port_in.size(); i++){
			state.index ++;
			//~ if (debug){std::cout << "Mensaje en Subnet_ISP. TaskID: "<< bag_port_in[i].id_tarea << ". Source: " << bag_port_in[i].src_ISP << ". Dest: " << bag_port_in[i].dest_ISP << std::endl; }
			state.Msg_list.push_back(bag_port_in[i]);                     
		}
		
		if (state.transmitting==false){
			preparationTime=convertir_tiempo( state.Msg_list.front().size );		//Proximo HOLD, size del 1er mensaje
			//~ preparationTime=get_random_time_exp(3,0.1);
			//~ preparationTime=convertir_tiempo(0.5);
			//~ preparationTime=convertir_tiempo(param_isp);
			if (debug){std::cout << "ISP_Net - ET - Programo un nuevo delay - TIME: " << state.clock1 <<". DELAY: "<< preparationTime<< " prox ev: " << state.clock1 + state.next_internal << std::endl; }
			state.next_internal=preparationTime;
			state.latencia_acc=convert_ND_to_double(preparationTime);
		}else{
			if (preparationTime > e){
				if (debug){std::cout << "ISP_Net - ET - completo el delay anterior - Actual: " << state.clock1 <<". DELAY: "<< state.next_internal << " prox ev: " << state.clock1 + state.next_internal << std::endl; }
				//~ state.next_internal=preparationTime - e;
				preparationTime = preparationTime - e;
				state.next_internal=preparationTime;
			}
			//~ else{
				//~ state.next_internal=0;
			//~ }
		}
		state.transmitting = true;	
		

    }
    
    //========================================================================================================================================
    // confluence transition
    //========================================================================================================================================
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
        external_transition(TIME(), move(mbs));
        internal_transition();
        
    }
    
    //========================================================================================================================================
    // output function
    //========================================================================================================================================
    typename make_message_bags<output_ports>::type output() const {
		typename make_message_bags<output_ports>::type bags;
				ServerMessage MSG;
				MSG = state.Msg_list.front();
				if (MSG.source>=nodes)
				{
					if (debug){cout << "ISP_Net - OUTPUT - Mensaje de server "<< MSG.source <<" a peer "<< MSG.dest <<" . - TIME: " << state.clock1 << " - NEXT_INT: "<< state.next_internal<< " - ACTUAL: "<< state.clock1 + state.next_internal<< std::endl; }
				}else{
					if (debug){cout << "ISP_Net - OUTPUT - Mensaje de peer "<< MSG.source <<" a server "<< MSG.dest <<" . - TIME: " << state.clock1 << " - NEXT_INT: "<< state.next_internal<< " - ACTUAL: "<< state.clock1 + state.next_internal<< std::endl; }
				}
				get_messages<typename Subnet_ISP_defs::out>(bags).push_back(MSG);
				ServerMessage MSG_DB;
				MSG_DB.source=-2;	
				MSG_DB.dest=nodes+nro_servers;	
				MSG_DB.tag=3;	
				MSG_DB.latencia=state.latencia_acc;	
				if (debug){cout << "ISP_Net - Output - MSG a DB - TIME: "<< state.clock1<< endl;}
				get_messages<typename Subnet_ISP_defs::outDB>(bags).push_back(MSG_DB);
				return bags;
    }
    
    //========================================================================================================================================
    // time_advance function
    //========================================================================================================================================
    TIME time_advance() const {
		if (debug){std::cout << "ISP_NET - TIMEADVANCE ########## - TIME: " << state.clock1 <<". preparationTime: "<< preparationTime<<". next_internal: "<< state.next_internal<< " prox ev: " << state.clock1 + state.next_internal << std::endl; }
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
			if (debug){std::cout << "ND_delay: " << ND_delay<< ". Delay: " << delay << std::endl;}
			//------------------------------------------------------------------------------
			return ND_delay;
		}
		
    friend ostringstream& operator<<(ostringstream& os, const typename Subnet_ISP<TIME>::state_type& i) {
        os << "index: " << i.index << " & transmitting: " << i.transmitting; 
        return os;
    }
};    
#endif // _Subnet_ISP_HPP__
