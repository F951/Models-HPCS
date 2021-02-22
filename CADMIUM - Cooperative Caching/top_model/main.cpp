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
#include "../data_structures/PastryMessage.hpp"
#include "../data_structures/TLCmessage.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/subnet_ISP.hpp"
#include "../atomics/subnet_Transport.hpp"
#include "../atomics/generator.hpp"
#include "../atomics/generator_string.hpp"
#include "../atomics/server_Simple.hpp"
#include "../atomics/ClientSimple.hpp"
#include "../atomics/Pastry_node_Simple.hpp"
#include "../atomics/p2pLayer/Pastry_Node.h"
#include "../atomics/TLC_node_Simple.hpp"
//~ #include "../atomics/queue.hpp"
//~ #include "../atomics/queuePastry.hpp"
//~ #include "../atomics/queueServer.hpp"
//~ #include "../atomics/queueTLC.hpp"
#include "../atomics/DataBase_Simple.hpp"
#include "../atomics/p2pLayer/StateBuilder.h"


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
//~ struct inp_1 : public in_port<ServerMessage>{};
//~ struct inp_2 : public in_port<ServerMessage>{};
struct in_coupledR_Up : public in_port<ServerMessage>{};			//Coupled Peer
struct out_coupledR_Up : public out_port<ServerMessage>{};			//Coupled Peer
struct in_coupledR_Down : public in_port<PastryMessage>{};			//Coupled Peer
struct out_coupledR_Down : public out_port<PastryMessage>{};		//Coupled Peer	
//~ struct in_coupledS : public in_port<ServerMessage>{};
//~ struct out1_coupledS : public out_port<ServerMessage>{};
//~ struct out2_coupledS : public out_port<ServerMessage>{};
//~ struct outDB1_coupledPeer : public out_port<ServerMessage>{};
//~ struct outDB2_coupledPeer : public out_port<ServerMessage>{};
//~ struct outDB3_coupledPeer : public out_port<ServerMessage>{};
struct outDB_coupledPeer : public out_port<ServerMessage>{};
//~ struct in1_coupledDB : public in_port<ServerMessage>{};
//~ struct in2_coupledDB : public in_port<int>{};
//~ struct out_coupledDB : public out_port<ServerMessage>{};
/***** Define output ports for coupled model *****/
struct outp_ack : public out_port<int>{};
struct outp_1 : public out_port<ServerMessage>{};
struct outp_2 : public out_port<ServerMessage>{};
struct outp_pack : public out_port<int>{};

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


void show_RTs_y_Leafsets(int nodes, vector<int> &indices, vector<Pastry_Node*> &PastryNetwork, StateBuilder* sb){
	//~ for (int i=0;i<nodes;i++){
		//~ cout <<"Indices[" << i << "]="<< indices[i] << endl;
	//~ }
	//~ for (int i=0;i<nodes;i++){
		//~ cout <<"NODO " << i << " a la salida del SB - IP: "<< PastryNetwork[i]->ne->getIP() << " - ID BIGNUM: "<< BN_bn2dec(PastryNetwork[i]->ne->getNodeID()) << endl;
	//~ }
	
	//~ for (int i=0;i<nodes;i++){
		//~ PastryNetwork[i]=(sb->Node_vector[i]);
		//~ cout <<"NODO " << i << " - IP NE: "<< PastryNetwork[i]->ne->getIP() << " - ID BIGNUM NE: "<< BN_bn2dec(PastryNetwork[i]->ne->getNodeID()) << endl;
		//~ cout <<"NODO " << i << " - IP: "<< PastryNetwork[i]->ip << " - ID BIGNUM: "<< BN_bn2dec(PastryNetwork[i]->nodeID) << endl;
		//~ cout <<"AUX " << i << " - IP: "<< PastryNetwork_aux[i]->ne->getIP() << " - ID BIGNUM: "<< BN_bn2dec(PastryNetwork_aux[i]->ne->getNodeID()) << endl;
		//~ cout <<"SB VECTOR " << i << " - IP: "<< (sb->Node_vector[i])->ne->getIP() << " - ID BIGNUM: "<< BN_bn2dec((sb->Node_vector[i])->ne->getNodeID()) << endl;
		//~ for (int j=0;j<40;j++){
			//~ for (int k=0;k<16;k++){
				//~ if ( (( (PastryNetwork[i])->rt->get(j,k))==NULL ) && ( ((sb->Node_vector[i])->rt->get(j,k))==NULL )){
					//~ cout << "RT ["<<j<<"]["<< k<<"] - PastryNetwork "<<i<<": NULL. Pastry_Node modificado: NULL" << endl;
				//~ }else if ((PastryNetwork[i]->rt->get(j,k))==NULL){
					//~ cout << "RT ["<<j<<"]["<< k<<"] - PastryNetwork "<<i<<": NULL. Pastry_Node modificado: "<< BN_bn2dec(((sb->Node_vector[i])->rt->get(j,k))->nodeID) << endl;
				//~ }else if (((sb->Node_vector[i])->rt->get(j,k))==NULL){
					//~ cout << "RT ["<<j<<"]["<< k<<"] - PastryNetwork "<<i<<": "<< BN_bn2dec((PastryNetwork[i]->rt->get(j,k))->nodeID) << ". Pastry_Node modificado: NULL" << endl;
				//~ }else{
					//~ cout << "RT ["<<j<<"]["<< k<<"] - PastryNetwork "<<i<<": "<< BN_bn2dec((PastryNetwork[i]->rt->get(j,k))->nodeID) << ". Pastry_Node modificado: "<< BN_bn2dec(((sb->Node_vector[i])->rt->get(j,k))->nodeID) << endl;
				//~ }
			//~ }
		//~ }
		//~ cout <<"===================================================================================================================="<< endl;
		//~ cout <<"===================================================================================================================="<< endl;
	//~ }
}

int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1; 
    }
    
    int debug = 1;
    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_str, TIME, const char* >("input_reader" , move(i_input));

    /****** Generator atomic model instantiation *******************/
    //~ int nodes=1000;
    int nodes=std::stoi(argv[2]);
    int totalQueries=std::stoi(argv[3]);
    int ID_numb=-1;	
    int totalQ_aux=totalQueries;
    //shared_ptr<dynamic::modeling::model> Generator = dynamic::translate::make_dynamic_atomic_model<Generator, TIME,int,int>("Generator1",std::move(nodes));
    shared_ptr<dynamic::modeling::model> generator = dynamic::translate::make_dynamic_atomic_model<GeneratorString, TIME,int,int,int>("Generator1",std::move(ID_numb),std::move(nodes),std::move(totalQ_aux));
    /****** Server & queue atomic model instantiation *******************/
    ID_numb=nodes;	//Los nodos van de 0 a (nodes-1). Nodes es el ID del servidor.
    shared_ptr<dynamic::modeling::model> server = dynamic::translate::make_dynamic_atomic_model<Server_Simple, TIME,int,int>("server",std::move(ID_numb),std::move(nodes));
    //~ shared_ptr<dynamic::modeling::model> queue_server = dynamic::translate::make_dynamic_atomic_model<QueueServer,TIME,int>("queueServer"+std::to_string(ID_numb),std::move(ID_numb));
    /****** DataBase & queueatomic models instantiation *******************/
    totalQ_aux=totalQueries;
    ID_numb=nodes+1;	//Nodes es el ID del servidor. Nodes+1 es el número de la DB. Se usa para no tener que modificar el modelo atómico de queue, que siempre tiene ID_numb
    shared_ptr<dynamic::modeling::model> database = dynamic::translate::make_dynamic_atomic_model<DataBase_Simple,TIME,int,int>("DataBase",std::move(ID_numb),std::move(totalQ_aux));
    //~ shared_ptr<dynamic::modeling::model> queue_db = dynamic::translate::make_dynamic_atomic_model<QueueServer,TIME,int>("queueDB",std::move(ID_numb));	
    
    /****** Subnet atomic models instantiation *******************/
    shared_ptr<dynamic::modeling::model> ISP = dynamic::translate::make_dynamic_atomic_model<Subnet_ISP, TIME>("ISP");
    shared_ptr<dynamic::modeling::model> Red_p2p = dynamic::translate::make_dynamic_atomic_model<Subnet_Transport, TIME>("Red_p2p");
	
	/****** Pastry Network Initialization *******************/
	vector<Pastry_Node*> PastryNetwork;
	Hash* h ;
	h = new Hash();
	//~ Hash HG;
	//~ std::stringstream str;		//Para debug del Hash
	//~ fflush(stdout);
	srand(1);
   
	//~ ofstream nodeIDsFile;
	//~ nodeIDsFile.open("nodeIDsFile.txt");
	
	ifstream nodeIDsInput;
	nodeIDsInput.open("../auxiliar/nodeIDsFile.txt");
	
	//~ ofstream nodeIDsFile2;
	//~ nodeIDsFile2.open("nodeIDsFile2.txt");
	string nodeID_string;
	BIGNUM * nodeid;
	for (int i=0;i<nodes;i++){
		//~ BIGNUM* nodeid = h->CreateRandomKey();
		//Fixed Key (debug) ----------------------------------------------------------------
		fflush(stdout);
		nodeID_string.clear();
		getline(nodeIDsInput,nodeID_string);
		string aux3=nodeID_string;
		const char* aux2 = (char*)(aux3).c_str();
		BIGNUM *key = BN_new();
		BN_hex2bn(&key,aux2);
		if (debug) {cout << "Peer: " << i  << ". Hash: " << BN_bn2hex(key) << endl;}
		//~ nodeIDsFile2 << BN_bn2hex(key) << endl;
		fflush(stdout);
		nodeid=key;
		PastryNetwork.push_back(new Pastry_Node(nodeid,i));
		PastryNetwork[i]->nodeID=nodeid;
		PastryNetwork[i]->ip=i;
		
	}
	
	vector<Pastry_Node*> PastryNetwork_aux;
	for (int i=0;i<nodes;i++){
		PastryNetwork_aux.push_back(PastryNetwork[i]);
	}
	//~ for (int i=0;i<nodes;i++){
		//~ cout <<"NODO " << i << " - IP: "<< PastryNetwork[i]->ne->getIP() << " - ID BIGNUM: "<< BN_bn2dec(PastryNestwork[i]->ne->getNodeID()) << endl;
	//~ }
	int leafset_size=16;
	StateBuilder* sb = new StateBuilder(PastryNetwork, leafset_size);
	sb->execute();
	
	//~ cout <<"===================================================================================================================="<< endl;
	//~ cout <<"===================================================================================================================="<< endl;
	int indices[nodes];
	for (int i=0;i<nodes;i++){
		PastryNetwork[i]=(sb->Node_vector[i]);
	}
	for (int i=0;i<nodes;i++){
		for (int j=0;j<nodes;j++){
			if (PastryNetwork[j]->ip==i){
				indices[i]=j;
			}
		}
	}
	//show_RTs_y_Leafsets();
	/***********************************************************/
	
	/****** Layer atomic model instantiation *******************/
    vector<shared_ptr<dynamic::modeling::model>> clientsSimple;
    vector<shared_ptr<dynamic::modeling::model>> Pastry_peers;
    vector<shared_ptr<dynamic::modeling::model>> Tlc_peers;
	//~ Hash* h ;
    for (int i=0; i<nodes; i++){
		ID_numb=i;
		int aux_nodes=nodes;
		NodeEntry* ne_aux = PastryNetwork[indices[i]]->ne;
		//~ clients.push_back(dynamic::translate::make_dynamic_atomic_model<Client,TIME,int,int,NodeEntry*>("Client"+std::to_string(i),std::move(ID_numb),std::move(aux_nodes),std::move(ne_aux)));
		clientsSimple.push_back(dynamic::translate::make_dynamic_atomic_model<ClientSimple,TIME,int,int,NodeEntry*>("ClientSimple"+std::to_string(i),std::move(ID_numb),std::move(aux_nodes),std::move(ne_aux)));
		//~ NodeEntry* ne_aux = PastryNetwork[indices[i]]->Node->ne;
		ne_aux = PastryNetwork[indices[i]]->ne;
		vector<NodeEntry*> leafset_aux;
		leafset_aux=(PastryNetwork[indices[i]]->leafset)->listAllNodes();
		if (debug) {cout << "Size Leafset Original: " << leafset_aux.size()<< endl;}
		if (debug) {cout << "indice"<< indices[i] << endl;}
		if (debug) {
			for (int j=0;j<leafset_aux.size();j++){
				cout << "Leafset ["<<j<<"] - PastryProtocol "<< ID_numb<<": "<< BN_bn2dec(leafset_aux[j]->nodeID) << endl;
			}
		}
		aux_nodes=nodes;
		Tlc_peers.push_back(dynamic::translate::make_dynamic_atomic_model<TLC_node_Simple,TIME,int,int,NodeEntry*,vector<NodeEntry*>>("TlcPeer"+std::to_string(i),std::move(ID_numb),std::move(aux_nodes),std::move(ne_aux),std::move(leafset_aux)));
		h = new Hash();
		BIGNUM* nodeid = h->CreateRandomKey();			//NO USADO
		//~ std::cout << "COUPLED_PEERS: "<< i<< ". Key: "<< UTIL::myBN2HEX2(nodeid) <<  std::endl;
		Pastry_Node* Node_aux=PastryNetwork[indices[i]];
		aux_nodes=nodes;
		Pastry_peers.push_back(dynamic::translate::make_dynamic_atomic_model<Pastry_node_Simple,TIME,int,int,BIGNUM*,Pastry_Node*>("PastryPeer"+std::to_string(i),std::move(ID_numb),std::move(aux_nodes),std::move(nodeid),std::move(Node_aux)));
	}
	
	/******* PEER COUPLED MODEL 1 ********/
    // FOR DE MODELOS
    vector<shared_ptr<dynamic::modeling::coupled<TIME>>> COUPLED_PEERS;
    for (int i=0; i<nodes; i++){	
		dynamic::modeling::Ports iports_Coupled_Peer = {typeid(in_coupledR_Up),typeid(in_coupledR_Down)};
		dynamic::modeling::Ports oports_Coupled_Peer = {typeid(out_coupledR_Up),typeid(out_coupledR_Down),typeid(outDB_coupledPeer)};
		dynamic::modeling::Models submodels_Coupled_Peer = { clientsSimple[i],Tlc_peers[i],Pastry_peers[i]};
		dynamic::modeling::EICs eics_Coupled_Peer = {
			dynamic::translate::make_EIC<in_coupledR_Up, ClientSimple_defs::DataIn>("ClientSimple"+std::to_string(i)),
			dynamic::translate::make_EIC<in_coupledR_Down, Pastry_node_Simple_defs::DataIn>("PastryPeer"+std::to_string(i)),
		};
		dynamic::modeling::EOCs eocs_Coupled_Peer = {
			dynamic::translate::make_EOC<ClientSimple_defs::DataOutUp,out_coupledR_Up>("ClientSimple"+std::to_string(i)),
			dynamic::translate::make_EOC<Pastry_node_Simple_defs::DataOutDown,out_coupledR_Down>("PastryPeer"+std::to_string(i)),
			dynamic::translate::make_EOC<ClientSimple_defs::DataOutDB,outDB_coupledPeer>("ClientSimple"+std::to_string(i)),
			dynamic::translate::make_EOC<TLC_node_Simple_defs::DataOutDB,outDB_coupledPeer>("TlcPeer"+std::to_string(i)),
			dynamic::translate::make_EOC<Pastry_node_Simple_defs::DataOutDB,outDB_coupledPeer>("PastryPeer"+std::to_string(i)),
		};
		dynamic::modeling::ICs ics_Coupled_Peer = {
			//Client - TLC
			dynamic::translate::make_IC<ClientSimple_defs::DataOutDown, TLC_node_Simple_defs::DataIn>("ClientSimple"+std::to_string(i),"TlcPeer"+std::to_string(i)),
			dynamic::translate::make_IC<TLC_node_Simple_defs::DataOutUp, ClientSimple_defs::DataIn>("TlcPeer"+std::to_string(i),"ClientSimple"+std::to_string(i)),
			//TLC - Pastry
			dynamic::translate::make_IC<TLC_node_Simple_defs::DataOutDown, Pastry_node_Simple_defs::DataIn>("TlcPeer"+std::to_string(i),"PastryPeer"+std::to_string(i)),
			dynamic::translate::make_IC<Pastry_node_Simple_defs::DataOutUp, TLC_node_Simple_defs::DataIn>("PastryPeer"+std::to_string(i),"TlcPeer"+std::to_string(i)),
			
		};
		
		COUPLED_PEERS.push_back(make_shared<dynamic::modeling::coupled<TIME>>(
			"Coupled_Peer"+std::to_string(i), submodels_Coupled_Peer, iports_Coupled_Peer, oports_Coupled_Peer, eics_Coupled_Peer, eocs_Coupled_Peer, ics_Coupled_Peer
		));
	}
    
	
	
    /*******ABP SIMULATOR COUPLED MODEL********/
    dynamic::modeling::Ports iports_ABP = {typeid(inp_control)};
    dynamic::modeling::Ports oports_ABP = {typeid(outp_ack),typeid(outp_pack)};
    dynamic::modeling::Models submodels_ABP;
    for (int i=0; i<nodes; i++){
		submodels_ABP.push_back(COUPLED_PEERS[i]);
	}
	submodels_ABP.push_back(generator);
	submodels_ABP.push_back(ISP);
	submodels_ABP.push_back(Red_p2p);
	submodels_ABP.push_back(server);
	submodels_ABP.push_back(database);

    dynamic::modeling::EICs eics_ABP = {
        cadmium::dynamic::translate::make_EIC<inp_control, GeneratorString_defs::controlIn>("Generator1")
    };
    dynamic::modeling::EOCs eocs_ABP = {
        dynamic::translate::make_EOC<GeneratorString_defs::packetSentOut,outp_pack>("Generator1"),
        dynamic::translate::make_EOC<GeneratorString_defs::ackReceivedOut,outp_ack>("Generator1")
    };
    dynamic::modeling::ICs ics_ABP;
	ics_ABP.push_back( dynamic::translate::make_IC<GeneratorString_defs::dataOut,Subnet_ISP_defs::in>("Generator1","ISP"));
	ics_ABP.push_back( dynamic::translate::make_IC<GeneratorString_defs::dataOut, DataBase_Simple_defs::DataIn>("Generator1","DataBase"));
	ics_ABP.push_back( dynamic::translate::make_IC<Subnet_ISP_defs::out, Server_Simple_defs::DataIn>("ISP","server"));
	ics_ABP.push_back( dynamic::translate::make_IC< Server_Simple_defs::DataOut,Subnet_ISP_defs::in>("server","ISP"));
	ics_ABP.push_back( dynamic::translate::make_IC< Server_Simple_defs::DataOutDB,DataBase_Simple_defs::DataIn>("server","DataBase"));
	for (int i=0; i<nodes; i++){
		ics_ABP.push_back(	dynamic::translate::make_IC<out_coupledR_Up, Subnet_ISP_defs::in>("Coupled_Peer"+std::to_string(i),"ISP"));
		ics_ABP.push_back( dynamic::translate::make_IC<Subnet_ISP_defs::out, in_coupledR_Up>("ISP","Coupled_Peer"+std::to_string(i))	);
		ics_ABP.push_back(	dynamic::translate::make_IC<out_coupledR_Down, Subnet_Transport_defs::in>("Coupled_Peer"+std::to_string(i),"Red_p2p"));
		ics_ABP.push_back( dynamic::translate::make_IC<Subnet_Transport_defs::out, in_coupledR_Down>("Red_p2p","Coupled_Peer"+std::to_string(i))	);
		ics_ABP.push_back( dynamic::translate::make_IC< outDB_coupledPeer,DataBase_Simple_defs::DataIn>("Coupled_Peer"+std::to_string(i),"DataBase"));
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
