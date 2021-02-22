/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* Server_Simple:
* Cadmium implementation of CD++ Server_Simple atomic model
*/

#ifndef __SERVER_HPP__
#define __SERVER_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <sstream>
#include "../auxiliar/Hash.h"
#include "../auxiliar/Util.h"

#include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"
#include "applicationLayer/Lru.h"		//Memorias cache

using namespace cadmium;
using namespace std;


void imprimir_msg_prueba(int a){
		cout << "==============================================================" << endl;
		cout << "Server_Simple, mensaje de prueba para simular función. Estado: "<< a << endl;
		cout << "==============================================================" << endl;
	}
	
//Port definition
    struct Server_Simple_defs{
        struct DataOut : public out_port<ServerMessage> { };
        struct DataOutDB : public out_port<ServerMessage> { };
        //~ struct doneOut : public out_port<bool> { };
        struct DataIn : public in_port<ServerMessage> { };
    };
   
    template<typename TIME>
    class Server_Simple{
        public:
            //Parameters to be overwriten when instantiating the atomic model
            int debug=0;
            int Peer_Selection=0;
            TIME   preparationTime;
            int ID_numb;
            int nodes;
			TIME clock1;
				
            // default constructor
            Server_Simple() noexcept{
				preparationTime  = TIME("00:00:00");
				state.estado=0;
				state.next_internal    = std::numeric_limits<TIME>::infinity();
            }
            
             Server_Simple(int _ID_numb, int _nodes) noexcept{
				preparationTime  = TIME("00:00:00");
				//~ state.active     = true;
				//~ state.send_data=false;
				state.estado=0;
				state.next_internal    = std::numeric_limits<TIME>::infinity();
				ID_numb			= _ID_numb;
				nodes			= _nodes;
            }
            // state definition
            struct state_type{
				vector<ServerMessage> Msg_queue;
				int estado;
				vector<ServerMessage> Msg_list;
				TIME next_internal;
				bool send_data;
				vector<int> vector_peers_consultados;
            }; 
            state_type state;
            
            //ports definition
            using input_ports=std::tuple<typename Server_Simple_defs::DataIn>;
            //~ using output_ports=std::tuple<typename Server_Simple_defs::DataOut,typename Server_Simple_defs::DataOutDB,typename Server_Simple_defs::doneOut>;
            using output_ports=std::tuple<typename Server_Simple_defs::DataOut,typename Server_Simple_defs::DataOutDB>;

			//========================================================================================================================================
            //internal transition
			//========================================================================================================================================        
            void internal_transition() {
				assert(state.estado!=0);
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
						if (debug){std::cout << "Server_Simple"<< ID_numb << " - Cambio de estado 1 a 2." << std::endl; }
						state.Msg_list.clear();
						if (state.Msg_queue.size()!=0){
							if (debug){std::cout << "Pastry_node_Simple"<< ID_numb << " - Voy a estado 1." << std::endl; }
							state.estado=1;
							state.next_internal=preparationTime;
							//~ clock1 = clock1 + state.next_internal;
						}else{
							if (debug){std::cout << "Pastry_node_Simple"<< ID_numb << " - Voy a estado 0." << std::endl; }
							state.estado=0;
							state.next_internal=std::numeric_limits<TIME>::infinity();	//Cuánto tiempo voy a estar en el estado 0
						}
						break;
					}
				}
            }

			//========================================================================================================================================
            // external transition
			//========================================================================================================================================            
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
				//std::cout << "Server_Simple"<< ID_numb << " - Transicion externa. Estado: "<< state.estado << std::endl; 
				clock1 += e;
				int flag_dataIn=0;
				int aux_source,aux_dest;
				if (debug){std::cout << "Server_Simple"<< ID_numb << " - External Transition" << std::endl; }
				for( auto &x : get_messages<typename Server_Simple_defs::DataIn>(mbs)){
					if (x.dest==ID_numb){
						assert(x.tag!=0);
						assert(x.source!=-1); 
						if (x.source == -1) { 		//Generador/Sender // Elegir destino del mensaje							
							x.version=0;
							x.source= ID_numb;
							x.tag=0; 
							x.dest=0;
							//if (Peer_Selection==0){
							//	x.dest=rand() % nodes; 
							//}else{
							//	rand_val(1);
							//	x.dest = getZipf(1.0, nodes-1);
							//}
							//~ x.dest=state.vector_peers_consultados.size();
							assert(x.dest!=nodes);
							if (debug){std::cout << "Server_Simple"<< ID_numb << " - Mensaje desde el sender. Enviar mensaje a peer " << x.dest << " con tag 0" << std::endl; }
							if (debug){cout << "=============================================================================================================================" << endl;
							std::cout << "Objeto en server - id_tarea: "<< x.id_tarea<< ". Key: "<< BN_bn2dec(x.query) << ". Destino: "<< x.dest << std::endl;
							cout << "=============================================================================================================================" << endl;}
							state.vector_peers_consultados.push_back(x.dest);
							//~ print_vec(state.vector_peers_consultados,"vector_peers_consultados");
						}
						else
						{
							if (debug){std::cout << "Server_Simple"<< ID_numb << " - Mensaje desde el peer: " << x.source << " con tag: " << x.tag << std::endl; }
							if (x.tag==1){
								//Me están pidiendo un objeto. Configurar el destino y enviar el mensaje con el objeto.
								aux_source=x.dest;
								aux_dest=x.source;
								x.source=ID_numb;
								x.dest=aux_dest;
								x.tag=2;
							}
							else if (x.tag==2)
							{
								if (debug){std::cout << "Server_Simple"<< ID_numb << " - Mensaje con tag2. Envío a DB y sigo con otro trabajo." << std::endl; }
								x.dest=nodes+1;
								x.source=ID_numb;
								x.tag=0;
							}
						}
						state.Msg_list.push_back(x);	
						flag_dataIn=1;
					}
					
				}
				
				if (flag_dataIn==1){
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
							break;
						}
					}
				}
				else
				{
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

				
			

			//========================================================================================================================================
            // confluence transition
			//========================================================================================================================================
			void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                internal_transition();
                external_transition(TIME(), std::move(mbs));
            }
			
			//========================================================================================================================================
            // output function
            //========================================================================================================================================
            typename make_message_bags<output_ports>::type output() const {
				if (debug){std::cout << "Server_Simple"<< ID_numb << " - Output function. Estado: "<< state.estado << ". Size lista de mensajes: " << state.Msg_list.size() << std::endl; }
				typename make_message_bags<output_ports>::type bags;
				const NDTime Time_aux=clock1 + state.next_internal;
				
				if(state.estado==0)	{
					//No hay salidas.
				}
				else if (state.estado==1){
					assert(state.Msg_list.size() !=0);
					if (debug){std::cout << "Server_Simple"<< ID_numb << " - Output function.  Publico el DataOut y espero un poco en la TI para publicar el DoneOut."  << std::endl;}
					ServerMessage out_aux;
					for (int i=0; i< state.Msg_list.size(); i++){					
						out_aux = state.Msg_list[i];
						if (debug){std::cout << "Server_Simple"<< ID_numb << " - Output function. ID destino: " << out_aux.dest << std::endl;}
						if ( out_aux.dest == (nodes+1) ) {
							if (debug){std::cout << "Server_Simple"<< ID_numb << " - Output function. Enviar a DB" << std::endl;}
							get_messages<typename Server_Simple_defs::DataOutDB>(bags).push_back(out_aux);
						}else{
							if (debug){std::cout << "Server_Simple"<< ID_numb << " - Output function. Enviar a Peer:" << out_aux.dest << std::endl;}
							if (debug){cout<<"WSE - CASE PEER - Send object to peer " << out_aux.dest << ". Time: " << Time_aux <<endl;}
							get_messages<typename Server_Simple_defs::DataOut>(bags).push_back(out_aux);
						}
					}
				}
				
				return bags;
				if (debug){std::cout << "Server_Simple"<< ID_numb << " - Exit output function."<< endl;}
            }
			//========================================================================================================================================
            // time_advance function
            //========================================================================================================================================
            TIME time_advance() const {  
				return state.next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Server_Simple<TIME>::state_type& i) {
                //~ os << "ackNum: " << i.ackNum; 
                 os << "Estado: " << i.estado << ". next_internal: " << i.next_internal; 
            return os;
            }
            
            void print_vec(vector<int> &vector_in, const char* nombre){
				//cout.open ("cout.txt",ios_base::app);
				cout << "#Vector: " << nombre << endl;
				for (int k=0;k<(int)(vector_in.size());k++){
					cout << " " << vector_in[k];
				}cout << "\n";
			}
			
						
			//int getZipf (int alpha, int n)
			//{
			//	static bool first = true;      // Static first time flag
			//	static double c = 0;          // Normalization constant
			//	double z;                     // Uniform random number (0 < z < 1)
			//	double sum_prob;              // Sum of probabilities
			//	double zipf_value=0;            // Computed exponential value to be returne
			//	int i;                        // Loop counter
			//	// Compute normalization constant on first call only
			//	if (first == true){
			//		for (i=1; i <= n; i++)
			//		c = c + (1.0 / pow((double) i, alpha));
			//		c = 1.0 / c;
			//		first = false;
			//		//cout << "PRIMERO" << endl;
			//	}
			//	// Pull a uniform random number (0 < z < 1)
			//	do
			//	{
			//		z = (((double) rand())/(RAND_MAX+1.0));
			//		// cout << "Z VALUE " << z << endl;
			//	}
			//	while ((z == 0) || (z == 1));
			//	
			//	// Map z to the value
			//	sum_prob = 0;
			//	for (i=1; i<=n; i++){
			//		//      cout <<"Z " << z << endl;
			//		//     cout << "SUM PROB: " << sum_prob << endl;
			//		//      cout << "C " << c <<endl;
			//		//      cout << "ALPHA " << alpha << endl;
			//		sum_prob = sum_prob + (c / pow((double) i, alpha));
			//		if (sum_prob >= z){
			//			zipf_value = i;
			//			break;
			//		}
			//	}
			//	//   cout << "ZIPF VALUE " << zipf_value << endl;
			//	// Assert that zipf_value is between 1 and N
			//	ASSERT((zipf_value >=1) && (zipf_value <= n));
			//	return zipf_value ;
			//}
			//
			//
			//double rand_val(int seed)
			//{ 
			//	const long a =16807;// Multiplier
			//	const long m = 2147483647;// Modulus
			//	const long q = 127773;// m div a
			//	const long r = 2836;// m mod a
			//	static long x; // Random int value
			//	long x_div_q; // x divided by q
			//	long x_mod_q; // x modulo q
			//	long x_new; // New x value
			//	// Set the seed if argument is non-zero and then return zero
			//	if (seed > 0){
			//		x = seed;
			//		return(0.0);
			//	}
			//	// // RNG using integer arithmetic
			//	x_div_q = x / q;
			//	x_mod_q = x % q;
			//	x_new = (a * x_mod_q) - (r * x_div_q);
			//	if (x_new > 0)
			//		x = x_new;
			//	else
			//		x = x_new + m;
			//	// // Return a random value between 0.0 and 1.0
			//	return((double) x / m);
			//}
			
        };     
  

#endif // __SERVER_HPP__
