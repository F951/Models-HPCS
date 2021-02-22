//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Messages structures
#include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"
//~ #include "../data_structures/PastryMessage.hpp"
//~ #include "../data_structures/TLCmessage.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
//~ #include "../atomics/subnet.hpp"
#include "../atomics/subnet_ISP.hpp"
//~ #include "../atomics/subnet_Transport.hpp"
//~ #include "../atomics/generator.hpp"
#include "../atomics/generator_string.hpp"
//~ #include "../atomics/server.hpp"
#include "../atomics/server_Simple.hpp"
//#include "../atomics/processor.hpp"
//~ #include "../atomics/Client.hpp"
#include "../atomics/ClientSimple.hpp"
//~ #include "../atomics/Pastry_node_Simple.hpp"
//~ #include "../atomics/p2pLayer/Pastry_Node.h"
//~ #include "../atomics/TLC_node_Simple.hpp"
//~ #include "../atomics/queue.hpp"
//~ #include "../atomics/queuePastry.hpp"
//~ #include "../atomics/queueServer.hpp"
//~ #include "../atomics/queueTLC.hpp"
//~ #include "../atomics/DataBase.hpp"
#include "../atomics/DataBase_Simple.hpp"
//~ #include "../atomics/p2pLayer/StateBuilder.h"


//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>

//Bignum
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "../auxiliar/Hash.h"
#include "../glob.h"
#include "../auxiliar/Util.h"
#include <ctime>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct inp_control : public in_port<std::string>{};
struct inp_net1 : public in_port<ServerMessage>{};
struct inp_net2 : public in_port<ServerMessage>{};


/***** Define output ports for coupled model *****/
struct outp_ack : public out_port<int>{};
//~ struct outp_1 : public out_port<ServerMessage>{};
//~ struct outp_2 : public out_port<ServerMessage>{};
struct outp_pack : public out_port<int>{};

/************Acoplado*************/
//~ struct outp_net1 : public out_port<ServerMessage>{};
//~ struct outp_net2 : public out_port<ServerMessage>{};
//~ struct outp_net_DB1 : public out_port<ServerMessage>{};
//~ struct outp_net_DB2 : public out_port<ServerMessage>{};

/****** Input Reader atomic model declaration *******************/
//~ template<typename T>
//~ class InputReader_str : public iestream_input<int,T> {
//~ public:
    //~ InputReader_str() = default;
    //~ InputReader_str(const char* file_path) : iestream_input<int,T>(file_path) {}
//~ };
template<typename T>
class InputReader_str : public iestream_input<std::string,T> {
public:
    InputReader_str() = default;
    InputReader_str(const char* file_path) : iestream_input<std::string,T>(file_path) {}
};
/***************************************************************/
void imprimir_inicio(vector<int> & prueba){
	for (int i=0;i<20;i++){
		prueba.push_back(i);
	}
	cout << "VECTOR LISTO" << endl;
}



int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1; 
    }
    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_str, TIME, const char* >("input_reader" , move(i_input));

    /****** Generator atomic model instantiation *******************/
    int nodes=std::stoi(argv[2]);
    int totalQueries=std::stoi(argv[3]);
    int nro_servers=std::stoi(argv[4]);
    int par_server=std::stoi(argv[5]);
    int par_isp=std::stoi(argv[6]);
    int par_peer=std::stoi(argv[7]);
    int ID_numb=-1;	
    int totalQ_aux=totalQueries;
    shared_ptr<dynamic::modeling::model> generator = dynamic::translate::make_dynamic_atomic_model<GeneratorString, TIME,int,int,int,int>("Generator1",std::move(ID_numb),std::move(nodes),std::move(nro_servers),std::move(totalQ_aux));
    
    /****** Server & queue atomic model instantiation *******************/
    vector<shared_ptr<dynamic::modeling::model>> servers;
	for (int i=0; i<nro_servers; i++){
		ID_numb=nodes+i;
		//~ ID_numb=i;
		//~ ID_numb=nodes;	//Los nodos van de 0 a (nodes-1). Nodes es el ID del servidor.
		servers.push_back(dynamic::translate::make_dynamic_atomic_model<Server_Simple, TIME,int,int,int,int,int>("server"+std::to_string(i),std::move(ID_numb),std::move(nodes),std::move(nro_servers),std::move(par_server),std::move(totalQueries)));
	}
    /****** DataBase atomic model instantiation *******************/
    totalQ_aux=totalQueries;
    ID_numb=nodes+nro_servers;	
    cout <<"Database ID: "<< ID_numb << endl;
    shared_ptr<dynamic::modeling::model> database = dynamic::translate::make_dynamic_atomic_model<DataBase_Simple,TIME,int,int,int,int,int>("DataBase",std::move(ID_numb),std::move(totalQ_aux),std::move(nodes),std::move(nro_servers),std::move(par_server),std::move(par_isp),std::move(par_peer));
        
    /****** Network atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> ISP1 = dynamic::translate::make_dynamic_atomic_model<Subnet_ISP, TIME,int,int,int>("ISP1", std::move(nodes), std::move(nro_servers),std::move(par_isp));
    shared_ptr<dynamic::modeling::model> ISP2 = dynamic::translate::make_dynamic_atomic_model<Subnet_ISP, TIME,int,int,int>("ISP2", std::move(nodes), std::move(nro_servers),std::move(par_isp));
    
    /****** Clients atomic model instantiation *******************/
    vector<shared_ptr<dynamic::modeling::model>> clients;
    for (int i=0; i<nodes; i++){
		ID_numb=i;
		int aux_nodes=nodes;
		clients.push_back(dynamic::translate::make_dynamic_atomic_model<ClientSimple,TIME,int,int,int,int>("ClientSimple"+std::to_string(i),std::move(ID_numb),std::move(aux_nodes),std::move(par_peer),std::move(totalQueries)));
	}

	/*******NETWORKS COUPLED MODEL********/
    //~ dynamic::modeling::Ports iports_Network = {typeid(inp_net1),typeid(inp_net2)};
    //~ dynamic::modeling::Ports oports_Network = {typeid(outp_net1),typeid(outp_net2),typeid(outp_net_DB1),typeid(outp_net_DB2)};
    //~ dynamic::modeling::Models submodels_Network = {ISP1, ISP2};
    //~ dynamic::modeling::EICs eics_Network = {
        //~ dynamic::translate::make_EIC<inp_net1, Subnet_ISP_defs::in>("ISP1"),
         //~ dynamic::translate::make_EIC<inp_net2, Subnet_ISP_defs::in>("ISP2")
    //~ };
    //~ dynamic::modeling::EOCs eocs_Network = {
        //~ dynamic::translate::make_EOC<Subnet_ISP_defs::out,outp_net1>("ISP1"),
        //~ dynamic::translate::make_EOC<Subnet_ISP_defs::out,outp_net2>("ISP2"),
        //~ dynamic::translate::make_EOC<Subnet_ISP_defs::outDB,outp_net_DB1>("ISP1"),
        //~ dynamic::translate::make_EOC<Subnet_ISP_defs::outDB,outp_net_DB2>("ISP2")
    //~ };
    //~ dynamic::modeling::ICs ics_Network = {};
    //~ shared_ptr<dynamic::modeling::coupled<TIME>> NETWORK;
    //~ NETWORK = make_shared<dynamic::modeling::coupled<TIME>>(
        //~ "Network", submodels_Network, iports_Network, oports_Network, eics_Network, eocs_Network, ics_Network 
    //~ );

	
	
    /*******ABP SIMULATOR COUPLED MODEL********/
    dynamic::modeling::Ports iports_ABP = {typeid(inp_control)};
    dynamic::modeling::Ports oports_ABP = {typeid(outp_ack),typeid(outp_pack)};
    dynamic::modeling::Models submodels_ABP;
    for (int i=0; i<nodes; i++){
		submodels_ABP.push_back(clients[i]);
	}
    for (int i=0; i<nro_servers; i++){
		submodels_ABP.push_back(servers[i]);
	}
	submodels_ABP.push_back(generator);
	submodels_ABP.push_back(database);
	/*********Atomico*********/
	submodels_ABP.push_back(ISP1);
	submodels_ABP.push_back(ISP2);
	/*********Acoplado*********/
	//~ submodels_ABP.push_back(NETWORK);
	

    dynamic::modeling::EICs eics_ABP = {
        cadmium::dynamic::translate::make_EIC<inp_control, GeneratorString_defs::controlIn>("Generator1")
    };
    dynamic::modeling::EOCs eocs_ABP = {
        dynamic::translate::make_EOC<GeneratorString_defs::packetSentOut,outp_pack>("Generator1"),
        dynamic::translate::make_EOC<GeneratorString_defs::ackReceivedOut,outp_ack>("Generator1")
    };
    dynamic::modeling::ICs ics_ABP;
    
	//ics_ABP.push_back( dynamic::translate::make_IC<GeneratorString_defs::dataOut,Subnet_ISP_defs::in>("Generator1","ISP"));
	ics_ABP.push_back( dynamic::translate::make_IC<GeneratorString_defs::dataOut, DataBase_Simple_defs::DataIn>("Generator1","DataBase"));
	/*********Atomico*********/
	ics_ABP.push_back( dynamic::translate::make_IC< Subnet_ISP_defs::outDB,DataBase_Simple_defs::DataIn>("ISP1","DataBase"));
	ics_ABP.push_back( dynamic::translate::make_IC< Subnet_ISP_defs::outDB,DataBase_Simple_defs::DataIn>("ISP2","DataBase"));
	/*********Acoplado*********/
	//~ ics_ABP.push_back( dynamic::translate::make_IC< outp_net_DB1,DataBase_Simple_defs::DataIn>("Network","DataBase"));
	//~ ics_ABP.push_back( dynamic::translate::make_IC< outp_net_DB2,DataBase_Simple_defs::DataIn>("Network","DataBase"));

	for (int i=0; i<nro_servers; i++){
		ics_ABP.push_back( dynamic::translate::make_IC<GeneratorString_defs::dataOut,Server_Simple_defs::DataIn>("Generator1","server"+std::to_string(i)));
		/*********Atomico*********/
		ics_ABP.push_back( dynamic::translate::make_IC<Subnet_ISP_defs::out, Server_Simple_defs::DataIn>("ISP1","server"+std::to_string(i)));
		ics_ABP.push_back( dynamic::translate::make_IC< Server_Simple_defs::DataOut,Subnet_ISP_defs::in>("server"+std::to_string(i),"ISP2"));
		/*********Acoplado*********/
		//~ ics_ABP.push_back( dynamic::translate::make_IC<outp_net1, Server_Simple_defs::DataIn>("Network","server"+std::to_string(i)));
		//~ ics_ABP.push_back( dynamic::translate::make_IC< Server_Simple_defs::DataOut,inp_net2>("server"+std::to_string(i),"Network"));
		
		ics_ABP.push_back( dynamic::translate::make_IC< Server_Simple_defs::DataOutDB,DataBase_Simple_defs::DataIn>("server"+std::to_string(i),"DataBase"));
	}
	for (int i=0; i<nodes; i++){
		/*********Atomico*********/
		ics_ABP.push_back(	dynamic::translate::make_IC<ClientSimple_defs::DataOut, Subnet_ISP_defs::in>("ClientSimple"+std::to_string(i),"ISP1"));
		ics_ABP.push_back( dynamic::translate::make_IC<Subnet_ISP_defs::out, ClientSimple_defs::DataIn>("ISP2","ClientSimple"+std::to_string(i))	);
		/*********Acoplado*********/
		//~ ics_ABP.push_back(	dynamic::translate::make_IC<ClientSimple_defs::DataOut, inp_net1>("ClientSimple"+std::to_string(i),"Network"));
		//~ ics_ABP.push_back( dynamic::translate::make_IC<outp_net2, ClientSimple_defs::DataIn>("Network","ClientSimple"+std::to_string(i))	);
	}	
	
    shared_ptr<dynamic::modeling::coupled<TIME>> ABP;
    ABP = make_shared<dynamic::modeling::coupled<TIME>>(
        "ABP", submodels_ABP, iports_ABP, oports_ABP, eics_ABP, eocs_ABP, ics_ABP 
    );


    /*******TOP COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(outp_pack),typeid(outp_ack)};
    dynamic::modeling::Models submodels_TOP = {input_reader, ABP};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<outp_pack,outp_pack>("ABP"),
        dynamic::translate::make_EOC<outp_ack,outp_ack>("ABP")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<std::string>::out, inp_control>("input_reader","ABP")
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/ABP_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/ABP_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    //~ using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    //~ using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    //~ using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;
    using logger_top=logger::multilogger<log_messages, global_time_mes>;

    /************** Runner call ************************/ 
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until_passivate();
    return 0;
}
