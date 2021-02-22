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
//~ #include "../auxiliar/Hash.h"
//~ #include "../auxiliar/Util.h"
#include <random>
#include <chrono>
//~ #include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"
//~ #include "applicationLayer/Lru.h"		//Memorias cache

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
            int nro_servers;
            int param_server;
            int totalQueries;
			int secuencia[20]={1,2,1,3,3,2,1,3,2,3,2};
			//~ int n =0;
            // default constructor
            Server_Simple() noexcept{
				preparationTime  = TIME("00:00:00:001");
				state.clock1=TIME("00:00:00");
				state.estado=0;
				state.next_internal    = std::numeric_limits<TIME>::infinity();
            }
            
             Server_Simple(int _ID_numb, int _nodes, int _nro_servers, int _param_server, int _totalQueries) noexcept{
				preparationTime  = TIME("00:00:00:001");
				state.clock1=TIME("00:00:00");
				state.Msg_list.clear();
				state.Msg_list_DB.clear();
				state.estado=0;
				state.flag_output=0;
				state.next_internal    = std::numeric_limits<TIME>::infinity();
				ID_numb			= _ID_numb;
				nodes			= _nodes;
				nro_servers	= _nro_servers;
				param_server	= _param_server;
				totalQueries	= _totalQueries;
				srand(1);
				state.count=0;
				
            }
            // state definition
            struct state_type{
				//~ vector<ServerMessage> Msg_queue;
				int estado;
				//~ vector<ServerMessage> Msg_list;
				list<ServerMessage> Msg_list;
				list<ServerMessage> Msg_list_DB;
				TIME next_internal;
				bool send_data;
				int flag_output;
				TIME clock1;
				vector<int> vector_peers_consultados;
				int count;
            }; 
            state_type state;
            
            //ports definition
            using input_ports=std::tuple<typename Server_Simple_defs::DataIn>;
            using output_ports=std::tuple<typename Server_Simple_defs::DataOut,typename Server_Simple_defs::DataOutDB>;

			//========================================================================================================================================
            //internal transition
			//========================================================================================================================================        
            void internal_transition() {
				assert(state.estado!=0);
				state.clock1+=state.next_internal;
				if (debug){std::cout << "Server_Simple"<< ID_numb << " -  INTERNAL TRANSITION - TIME: " << state.clock1 << " preparationTime "<< preparationTime <<std::endl; }
				switch (state.estado)
				{
					
					case 0:
					{
						//No se debe llegar hasta acá por el assert
						break;
					}
					case 1:
					{
						if (state.Msg_list_DB.size() !=0){
							state.Msg_list_DB.pop_front();
						}
						if ((state.Msg_list.size() !=0)){
							state.Msg_list.pop_front();
						}
						if (state.Msg_list.size()!=0){
							//~ if (debug){std::cout << "Server_Simple"<< ID_numb << " - Voy a estado 1." << std::endl; }
							state.estado=1;
							//preparationTime = convertir_tiempo( (ID_numb + state.Msg_list.front().id_tarea)*0.0000001 );
							preparationTime = convertir_tiempo( (((ID_numb-nodes)/nodes) + 0.1*(state.Msg_list.front().id_tarea%(totalQueries/2)/(double)totalQueries) )*10 );
							//~ preparationTime=get_random_time_exp(3,0.1);
							//~ preparationTime=convertir_tiempo(param_server);
							state.next_internal=preparationTime;
							if (debug){std::cout << "Server_Simple"<< ID_numb << " -  INTERNAL TRANSITION - Mensajes pendientes - TIME: " << state.clock1 << " preparationTime "<< preparationTime <<std::endl; }
							//~ state.clock1 = state.clock1 + state.next_internal;
						}else{
							//~ if (debug){std::cout << "Server_Simple"<< ID_numb << " - Voy a estado 0." << std::endl; }
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
				state.clock1+=e;
				state.flag_output=0;
				int flag_dataIn=0;
				int aux_source,aux_dest;
				if (debug){std::cout << "Server_Simple"<< ID_numb << " - External Transition - TIME: " << state.clock1 << std::endl; }
				for( auto &x : get_messages<typename Server_Simple_defs::DataIn>(mbs)){
					if (x.dest==ID_numb){
						if (x.source == -1) { 		//Generador/Sender // Elegir destino del mensaje							
							x.version=0;
							x.source= ID_numb;
							x.tag=0; 					
							
							x.dest=(x.id_tarea + ID_numb)%nodes;
							//~ x.dest=rand()% nodes; 
							//~ x.dest=secuencia[state.count]; state.count++;
							
							assert(x.dest!=nodes);
							if (debug){std::cout << "Server_Simple"<< ID_numb << " ET - Mensaje desde el sender. Enviar mensaje a peer " << x.dest << " con tag 0 - TIME: " << state.clock1 << std::endl; }
							if (1){cout << "=============================================================================================================================" << endl;
							std::cout << "Objeto en server - id_tarea: "<< x.id_tarea<< ". Key: "<< BN_bn2dec(x.query) << ". Destino: "<< x.dest << std::endl;
							cout << "=============================================================================================================================" << endl;}
							state.vector_peers_consultados.push_back(x.dest);
							state.Msg_list.push_back(x);	
						}
						else
						{
							if (debug){std::cout << "Server_Simple"<< ID_numb << " ET - Mensaje desde el peer: " << x.source << " con tag: " << x.tag << " latencia: " << x.latencia << " - TIME: " << state.clock1 << std::endl; }
							if (x.tag==1)	//Enviar a la DB
							{		
								x.dest=nodes+nro_servers;	//Database ID
								x.source=ID_numb;
								x.tag=0;
								state.Msg_list.push_back(x);	
							}
						}
						
						flag_dataIn=1;
						//~ state.Msg_list.push_back(x);
					}					
				}
				
				if (flag_dataIn==1){
					switch (state.estado)
					{
						case 0:
						{
							state.estado=1;
							//~ state.clock1+= e;		//Esto lo hago al principio de la ET.
							//~ preparationTime = convertir_tiempo( (ID_numb + state.Msg_list.front().id_tarea)*0.0000001 );
							preparationTime = convertir_tiempo( (((ID_numb-nodes)/nodes) + 0.1*(state.Msg_list.front().id_tarea%(totalQueries/2)/(double)totalQueries) )*10 );
							//~ preparationTime=get_random_time_exp(3,0.1);
							//~ preparationTime=convertir_tiempo(param_server);
							state.next_internal=preparationTime;
							if (debug){std::cout << "Server_Simple"<< ID_numb << " - ET - Programo un nuevo delay - TIME: " << state.clock1 <<". DELAY: "<< preparationTime<< " prox ev: " << state.clock1 + state.next_internal << std::endl; }
							break;
						}
						case 1:
						{
							state.estado=1;
							//~ state.next_internal=preparationTime - e;
							preparationTime = preparationTime - e;
							state.next_internal=preparationTime;
							if (debug){std::cout << "Server_Simple"<< ID_numb << " - ET - completo el delay anterior - Actual: " << state.clock1 <<". DELAY: "<< state.next_internal << " prox ev: " << state.clock1 + state.next_internal << std::endl; }
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
				if (debug){std::cout << "Server_Simple"<< ID_numb << " - CONFLUENT ==============================================================." << endl;}
				external_transition(TIME(), move(mbs));
				internal_transition();
				
            }
			
			//========================================================================================================================================
            // output function
            //========================================================================================================================================
            typename make_message_bags<output_ports>::type output() const {
				typename make_message_bags<output_ports>::type bags;

				if(state.estado==0)	{
					//No hay salidas.
				}
				else if (state.estado==1){
					ServerMessage out_aux;
					if (state.Msg_list.size() !=0){
						out_aux = state.Msg_list.front();
						if (debug){std::cout << "Server_Simple"<< ID_numb << " - OUTPUT. ID destino: " << out_aux.dest << " - source: " << out_aux.source <<" - TIME: " << state.clock1 << " - NEXT_INT: "<< state.next_internal<< " - ACTUAL: "<< state.clock1 + state.next_internal<< std::endl; }
						const NDTime Time_aux=state.clock1 + state.next_internal;
						if(out_aux.dest!=(nodes+nro_servers)){
							out_aux.latencia=convert_ND_to_double(Time_aux);
							assert ((out_aux.dest>=0) && out_aux.dest<=(nodes+nro_servers));
							get_messages<typename Server_Simple_defs::DataOut>(bags).push_back(out_aux);
						}else{
							out_aux.latencia=convert_ND_to_double(Time_aux) - out_aux.latencia;
							if (debug){std::cout << "Server_Simple"<< ID_numb << " - OUTPUT. Enviar a DB - source: " << out_aux.source <<" - TIME: " << state.clock1 << " - NEXT_INT: "<< state.next_internal<< " - ACTUAL: "<< state.clock1 + state.next_internal<< std::endl; }
							assert ( (out_aux.dest>=0) && out_aux.dest<=(nodes+nro_servers) );
							get_messages<typename Server_Simple_defs::DataOutDB>(bags).push_back(out_aux);
						}
						
					}

				}
				return bags;
            }
            
			//========================================================================================================================================
            // time_advance function
            //========================================================================================================================================
            TIME time_advance() const {  
				if (debug){std::cout << "Server_Simple"<< ID_numb << " - TIMEADVANCE ########## - TIME: " << state.clock1 <<". preparationTime: "<< preparationTime<<". next_internal: "<< state.next_internal<< " prox ev: " << state.clock1 + state.next_internal << std::endl; }
				//~ if (state.flag_output==1){
					//~ return state.next_internal;
				//~ }else{
					//~ return TIME("00:00:00");
				//~ }
				//~ state.clock1=state.next_internal;
				return state.next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Server_Simple<TIME>::state_type& i) {
                //~ os << "ackNum: " << i.ackNum; 
                 os << "Estado: " << i.estado << ". next_internal: " << i.next_internal; 
            return os;
            }
            
            void print_vec(vector<int> &vector_in, const char* nombre){
				cout << "#Vector: " << nombre << endl;
				for (int k=0;k<(int)(vector_in.size());k++){
					cout << " " << vector_in[k];
				}cout << "\n";
			}
			
						
			int getZipf (int alpha, int n)
			{
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
				}
				// Pull a uniform random number (0 < z < 1)
				do
				{
					z = (((double) rand())/(RAND_MAX+1.0));					
				}
				while ((z == 0) || (z == 1));
				
				// Map z to the value
				sum_prob = 0;
				for (i=1; i<=n; i++){
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
			
			//====================================================================================================================================
			double convert_ND_to_double  (const TIME clock_in) const {
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
				//~ if (debug){std::cout << "ND_delay: " << ND_delay<< ". Delay: " << delay << std::endl;}
				//------------------------------------------------------------------------------
				return ND_delay;
			}
		
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
			
        };     
  

#endif // __SERVER_HPP__
