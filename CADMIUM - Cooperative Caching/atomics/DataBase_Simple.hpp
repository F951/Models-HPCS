/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* DataBase_Simple:
* Cadmium implementation of CD++ DataBase_Simple atomic model
*/

#ifndef __DataBase_Simple_HPP__
#define __DataBase_Simple_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>

#include "../data_structures/message.hpp"
#include "../data_structures/ServerMessage.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

using namespace cadmium;
using namespace std;



	

//Port definition
    struct DataBase_Simple_defs{
        struct DataOut : public out_port<ServerMessage> { };
        //~ struct doneOut : public out_port<bool> { };
        struct DataIn : public in_port<ServerMessage> { };
        struct packetsExpected : public in_port<int> { };
    };
   
    template<typename TIME>
    class DataBase_Simple{
        public:
            //Parameters to be overwriten when instantiating the atomic model
            int debug=0;
            TIME   preparationTime;
            int ID_numb;
            int queries_expected;
            vector<int> answers_received;
            vector<int> answers_expected;
            //~ int flag_DataIn=0;
            ofstream archivo_salida;
			TIME clock1;
			
		
            // default constructor
            DataBase_Simple() noexcept{
				preparationTime  = TIME("00:00:00");
				state.estado=0;
				state.next_internal    = std::numeric_limits<TIME>::infinity();
				clock1=TIME("00:00:00");
				state.t_inicio=TIME("00:00:00");
				state.TotalTasks=0;
				state.global_hits=0;	
				state.local_hits=0;	
				state.misses=0;		
				state.hops=0;			
				state.latencia=0;		
				state.queries_in=0;	
				state.replicas=0;
				state.msgs_recieved=0;
				state.terminadas=0;
				state.queries_WSE=0;
            }
            
             DataBase_Simple(int _ID_numb, int _queries_expected) noexcept{
				preparationTime  = TIME("00:00:00");
				state.estado=0;
				state.next_internal    = std::numeric_limits<TIME>::infinity();
				ID_numb			= _ID_numb;
				clock1=TIME("00:00:00");
				state.t_inicio=TIME("00:00:00");
				state.flag_done=0;
				state.TotalTasks=0;
				state.global_hits=0;	
				state.local_hits=0;	
				state.misses=0;		
				state.hops=0;			
				state.latencia=0;		
				state.queries_in=0;	
				state.replicas=0;	
				state.terminadas=0;
				state.queries_WSE=0;
				state.t_observacion=0.00001;	
				state.msgs_recieved=0;
				queries_expected = _queries_expected;
				archivo_salida.open ("Salida_stats.csv",ios_base::app);
				for (int i=0;i<ID_numb-1;i++){
					state.load.push_back(0);	
				}
				for (int i=0;i<queries_expected;i++){
					answers_expected.push_back(i);	
				}
				cout << "Peers"<< ";" << "nQueriesOut"<< ";" << "time"<< ";"  << "terminadas" <<  ";" << "queries_in"<<  ";"  <<  "queries_WSE" << ";" << "hitLocal"<< ";" << "hitGlobal"<< ";"  << "misses"<< ";"  << "avHops"<<  ";"  << "nReplicas" << ";"<< "gini" << ";"  << "avLat"<< endl;
				//~ cout << state.global_hits << ";" << state.local_hits << ";" << state.misses << ";" << state.avHops << ";" << state.avLat << ";" << state.queries_in << ";" << state.replicas << ";" << state.terminadas<< ";" << state.queries_WSE << ";" << convert_ND_to_double(clock1) << gini << enl;
            }
            // state definition
            struct state_type{
				int estado;
				vector<ServerMessage> Msg_queue;
				vector<ServerMessage> Msg_list;
				TIME next_internal;
				bool send_data;
				TIME t_inicio;
				int TotalTasks;
				int global_hits;
				int local_hits;
				int misses;
				int hops;
				int overloaded;
				vector<int> load;
				int replicas;
				int terminadas;
				int queries_WSE;
				int nFin;
				int msgID;
				int sentQueries;
				int queries_in;
				double latencia;
				double t_observacion;
				int msgs_recieved;
				int flag_done;
				vector<int> vector_finalizacion_peers;
            }; 
            state_type state;
            
            //ports definition
            using input_ports=std::tuple<typename DataBase_Simple_defs::DataIn,typename DataBase_Simple_defs::packetsExpected>;
            //~ using output_ports=std::tuple<typename DataBase_Simple_defs::DataOut,typename DataBase_Simple_defs::doneOut>;
            using output_ports=std::tuple<typename DataBase_Simple_defs::DataOut>;
			
			
			
            //========================================================================================================================================
            //internal transition
            //========================================================================================================================================
            void internal_transition() {
				//std::cout << "Transicion interna en DataBase_Simple: " << ID_numb <<  std::endl; 
				//state.flag_done=0;
				assert(state.estado!=0);
				clock1 =  clock1 + state.next_internal;
				if ( state.estado==0 ){
					
				}
				else if ( state.estado==1 ){	
					if (debug){std::cout << "DataBase_Simple"<< ID_numb << " - Cambio de estado 1 a 2." << std::endl; }
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
					
				}
            }

            //========================================================================================================================================
            // external transition
            //========================================================================================================================================
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
				
				//~ if (debug){cout << "DataBase_Simple "<< ID_numb << " - ET - Clock1: " << clock1<< ". Elapsed Time: " << e  << ". Next_interal previo: " << state.next_internal << endl;}
				clock1+=e;
				//~ if (debug){cout << "DataBase_Simple "<< ID_numb << " - ET - Nuevo Clock1: " << clock1 << endl;}
				int flag_DataIn=0;
				for( auto &x : get_messages<typename DataBase_Simple_defs::DataIn>(mbs)){
					if (x.dest==ID_numb){
						if (x.tag == -1){
							state.TotalTasks =  x.id_tarea;
							if (debug){std::cout << "DataBase_Simple"<< ID_numb << " - TotalTasks: " << state.TotalTasks << " ====================================================================" <<  std::endl;}
						}
						else if (x.tag == 3){
							//~ if (debug){cout <<"TAG 3 ================================== DATABASE MSG IN: " << x.stats_TLC->global_hits << ";" << x.stats_TLC->local_hits << ";" << x.stats_TLC->misses << ";" << x.stats_TLC->hops << ";" << x.stats_TLC->latencia << ";" << x.stats_TLC->queries_in << ";" << x.stats_TLC->replicas << endl;		}
							state.global_hits	+= x.stats_TLC->global_hits;	
							state.local_hits	+= x.stats_TLC->local_hits;		
							state.misses		+= x.stats_TLC->misses;			
							state.hops			+= x.stats_TLC->hops;			
							state.latencia		+= x.stats_TLC->latencia;		
							state.queries_in	+= x.stats_TLC->queries_in;		
							state.replicas		+= x.stats_TLC->replicas;		
							state.terminadas	+= x.stats_TLC->terminadas;		
							state.queries_WSE	+= x.stats_TLC->queries_WSE;		
							state.load[x.source]+=x.stats_TLC->load;
							if (x.stats_TLC->terminadas!=0){
								for (int i=0;i<x.stats_TLC->terminadas;i++){
									state.vector_finalizacion_peers.push_back(x.source);
									answers_received.push_back(x.id_tarea);
									assert(x.stats_TLC->terminadas <=1);
								}
								//~ diferencias_en_vectores(answers_expected,answers_received);
							}
							if (debug){cout <<"TAG = " << x.tag << ".Stats from peer: " << x.source << ". id_tarea: " << x.id_tarea << endl;}
						}else {
							//~ state.msgs_recieved++;
							cout << "MENSAJE DISTINTO DE TAG 3 EN LA DATABASE :::::::::::::::::::===============::::::::::::::::::===================:::::::::::::::::===================" << endl;
						}
						
						//~ }
						
						state.Msg_list.push_back(x);
						flag_DataIn=1;		
					}	
												
				}
				if (flag_DataIn==1){
					//~ if (1){cout << convert_ND_to_double(clock1) << ";" << ID_numb - 1 << ";" << queries_expected << ";" << state.terminadas << ";" << state.queries_in << ";" << state.queries_WSE << ";"  << state.local_hits << ";" << state.global_hits << ";" << state.misses << ";" << avHops <<  ";" << state.replicas << ";"<< gini << ";" << avLat << endl;}
					if ( convert_ND_to_double(clock1 - state.t_inicio ) > state.t_observacion){
					//~ if ( 1){
						state.t_inicio = clock1;
						double gini=getGini((ID_numb-1), state.load);
						double avHops=0;
						double avLat=0;
						if(state.queries_in >0){
							avHops = (double)state.hops/state.queries_in;
							avLat=(double)state.latencia/state.queries_in;
						}
						if (debug){cout <<"========================================================== DATABASE STATE: " << ID_numb - 1 << ";" << queries_expected << ";" <<state.global_hits << ";" << state.local_hits << ";" << state.misses << ";" << avHops << ";" << avLat << ";" << state.queries_in << ";" << state.replicas << ";" << state.terminadas<< ";" << state.queries_WSE << ";" << convert_ND_to_double(clock1) << ";"<< gini << endl;}
						//Queries_WSE es lo mismo que misses.						
						
						//if (1){cout << ID_numb - 1 << ";" << queries_expected << ";" <<state.global_hits << ";" << state.local_hits << ";" << state.misses << ";" << avHops << ";" << avLat << ";" << state.queries_in << ";" << state.replicas << ";" << state.terminadas<< ";" << state.queries_WSE << ";" << convert_ND_to_double(clock1) << ";"<< gini << endl;}
						if (debug){cout << convert_ND_to_double(clock1) << ";" << ID_numb - 1 << ";" << queries_expected << ";" << state.terminadas << ";" << state.queries_in << ";" << state.queries_WSE << ";"  << state.local_hits << ";" << state.global_hits << ";" << state.misses << ";" << avHops <<  ";" << state.replicas << ";"<< gini << ";" << avLat << endl;}
						if (debug){print_vec(state.load,"state.load");}
						//~ print_vec(state.vector_finalizacion_peers,"vector_finalizacion_peers");
						//reset_statistics();
					}
					 if (queries_expected==state.terminadas){
						double gini=getGini((ID_numb-1), state.load);
						double avHops=0;
						double avLat=0;
						if(state.queries_in >0){
							avHops = (double)state.hops/state.queries_in;
							avLat=(double)state.latencia/state.queries_in;
						}
						cout << "Latencia: " << state.latencia << ". avLat: " << avLat << endl;
						if (debug){print_vec(state.load,"state.load");}
						//~ if (1){cout << state.global_hits << ";" << state.local_hits << ";" << state.misses << ";" << avHops << ";" << avLat << ";" << state.queries_in << ";" << state.replicas << ";" << state.terminadas<< ";" << state.queries_WSE << ";" << convert_ND_to_double(clock1) << ";"<< gini << endl;}
						if (1){cout << convert_ND_to_double(clock1) << ";" << ID_numb - 1 << ";" << queries_expected << ";" << state.terminadas << ";" << state.queries_in << ";" << state.queries_WSE << ";"  << state.local_hits << ";" << state.global_hits << ";" << state.misses << ";" << avHops <<  ";" << state.replicas << ";"<< gini << ";" << avLat << endl;}
						if (1){archivo_salida << "time"<< ";"  << "Peers"<< ";" << "nQueriesOut"<< ";" << "terminadas" <<  ";" << "queries_in"<<  ";"  <<  "queries_WSE" << ";" << "hitLocal"<< ";" << "hitGlobal"<< ";"  << "misses"<< ";"  << "avHops"<<  ";"  << "nReplicas" << ";"<< "gini" << ";"  << "avLat"<< endl;}
						if (1){archivo_salida << convert_ND_to_double(clock1) << ";" << ID_numb - 1 << ";" << queries_expected << ";" << state.terminadas << ";" << state.queries_in << ";" << state.queries_WSE << ";"  << state.local_hits << ";" << state.global_hits << ";" << state.misses << ";" << avHops <<  ";" << state.replicas << ";"<< gini << ";" << avLat << endl;}
					}
					double avLat=0;
					avLat=(double)state.latencia/state.queries_in;
					//~ cout << "Latencia: " << state.latencia << ". avLat: " << avLat << ". queries_in:"<< state.queries_in << endl;
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
					
            }

            //========================================================================================================================================
            // confluence transition
            //========================================================================================================================================
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                external_transition(TIME(), std::move(mbs));
                internal_transition();
            }

            //========================================================================================================================================
            // output function
            //========================================================================================================================================
            typename make_message_bags<output_ports>::type output() const {
				typename make_message_bags<output_ports>::type bags;
				const NDTime Time_aux=clock1 + state.next_internal;
				//std::cout << "DataBase_Simple"<< ID_numb << " - Output function. Estado: "<< state.estado << std::endl; 
				if(state.estado==0)
				{
					//No hay salida
				}
				else if (state.estado==1){
					//No hay salida
				}
				
				return bags;
            }

            // time_advance function
            TIME time_advance() const {  
				return state.next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename DataBase_Simple<TIME>::state_type& i) {
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
            
            double getGini(int np, vector<int> & load)
			{
				vector<int> load2=load;
				double l=0;
				double gini=0;
				double mean=0;  
				for(int i=0; i< np ; i++){
				   l=l+load2[i];
				}
				mean=l/np;
				sort(load2.begin(),load2.end() );
				for (int i=0; i < np; i++){
				   int j= i + 1;				//Esto no se puede reemplazar en la fórmula??
				   gini = gini + (( 2 * j - ( np - 1 )) * load2[i] );
				}
				if (mean>0)
					gini= gini / (np*np*mean);
				return gini;
			}
			
			void reset_statistics(){
				//Reiniciar estadísticas.
				state.TotalTasks=0;				
				state.global_hits=0;	
				state.local_hits=0;	
				state.misses=0;		
				state.hops=0;			
				state.latencia=0;		
				state.queries_in=0;	
				state.replicas=0;
				state.terminadas=0;
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
			
			void diferencias_en_vectores(vector<int> &vec1, vector<int>& vec2){
				vector <int> diferencias;
				vector<int>::iterator it_vector1,it_vector2;
				for (it_vector1=vec1.begin(); it_vector1!=vec1.end(); ++it_vector1)
				{
					it_vector2 = find(vec2.begin(), vec2.end(), *it_vector1);
					if(it_vector2 == vec2.end()){
						diferencias.push_back(*it_vector1);
					}
				}
				print_vec(diferencias,"diferencias");	
			}

            
        };     
  

#endif // __DataBase_Simple_HPP__
