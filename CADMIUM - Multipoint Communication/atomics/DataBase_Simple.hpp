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

//~ #include "../data_structures/message.hpp"
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
            //~ int flag_DataIn=0;
            ofstream archivo_salida;
			int nodes;
			int nro_servers;
			int param_server;
			int param_isp	;
			int param_peer	;
			
            // default constructor
            DataBase_Simple() noexcept{
				preparationTime  = TIME("00:00:00");
				state.estado=0;
				state.next_internal    = std::numeric_limits<TIME>::infinity();
				state.clock1=TIME("00:00:00");
				state.t_inicio=TIME("00:00:00");
				state.latencia=0;		
				state.queries_in=0;	
				state.msgs_recieved=0;
				state.terminadas=0;
            }
            
             DataBase_Simple(int _ID_numb, int _queries_expected, int _nodes, int _nro_servers, int _param_server,int _param_isp,int _param_peer) noexcept{
				preparationTime  = TIME("00:00:00");
				state.estado=0;
				state.next_internal    = std::numeric_limits<TIME>::infinity();
				ID_numb			= _ID_numb;
				nodes			= _nodes;
				nro_servers			= _nro_servers;
				param_server			= _param_server;
				param_isp			= _param_isp;
				param_peer			= _param_peer;
				state.clock1=TIME("00:00:00");
				state.t_inicio=TIME("00:00:00");
				state.flag_done=0;
				state.latencia=0;		
				state.TotalTasks=0;		
				state.queries_in=0;	
				state.terminadas=0;
				state.t_observacion=0.00001;	
				state.msgs_recieved=0;
				state.latencia_isp = 0;
				queries_expected = _queries_expected;
				archivo_salida.open ("Salida_stats.csv",ios_base::app);
				//~ cout << "Peers"<< ";" << "nQueriesOut"<< ";" << "time"<< ";"  << "terminadas" <<  ";" << "queries_in"<<  ";"  <<  "queries_WSE" << ";" << "hitLocal"<< ";" << "hitGlobal"<< ";"  << "misses"<< ";"  << "avHops"<<  ";"  << "nReplicas" << ";"<< "gini" << ";"  << "avLat"<< endl;
            }
            // state definition
            struct state_type{
				int estado;
				vector<ServerMessage> Msg_list;
				TIME next_internal;
				bool send_data;
				TIME clock1;
				TIME t_inicio;
				int terminadas;	
				int queries_in;
				int TotalTasks;
				double latencia;
				double t_observacion;
				int msgs_recieved;
				int flag_done;
				double latencia_isp;
				vector<int> vector_finalizacion_peers;
            }; 
            state_type state;
            
            //ports definition
            using input_ports=std::tuple<typename DataBase_Simple_defs::DataIn,typename DataBase_Simple_defs::packetsExpected>;
            using output_ports=std::tuple<typename DataBase_Simple_defs::DataOut>;
			
			
			
            //========================================================================================================================================
            //internal transition
            //========================================================================================================================================
            void internal_transition() {
				assert(state.estado!=0);
				if ( state.estado==0 ){
					//No se debe llegar a este punto
				}
				else if ( state.estado==1 ){	//Publicar DataOut 
					state.Msg_list.clear();
					if (state.Msg_list.size()!=0){
						state.estado=1;
						state.next_internal=preparationTime;
						state.clock1 = state.clock1 + state.next_internal;
					}else{
						state.estado=0;
						state.next_internal=std::numeric_limits<TIME>::infinity();	//Cuánto tiempo voy a estar en el estado 0
					}
				}
            }

            //========================================================================================================================================
            // external transition
            //========================================================================================================================================
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
				
				//~ if (debug){cout << "DataBase_Simple "<< ID_numb << " - ET - Clock1: " << state.clock1<< ". Elapsed Time: " << e  << ". Next_interal previo: " << state.next_internal << endl;}
				state.clock1+=e;
				//~ if (debug){cout << "DataBase_Simple "<< ID_numb << " - ET - Nuevo Clock1: " << state.clock1 << endl;}
				int flag_DataIn=0;
				for( auto &x : get_messages<typename DataBase_Simple_defs::DataIn>(mbs)){
					
					if (x.dest==ID_numb){
						//~ if (1){cout <<"DATABASE : TAG = " << x.tag << ". DEST = " << x.dest << ".Stats from: " << x.source << ". id_tarea: " << x.id_tarea << ". latencia: " << x.latencia <<". latencia_isp: " << state.latencia_isp << " TIME: "<< state.clock1<< endl;}
						if (x.source==-2){
							assert(x.tag==3);
							if (debug){cout <<"DATABASE 1: TAG = " << x.tag << ".Stats from ISP. id_tarea: " << x.id_tarea << ". latencia: " << x.latencia <<". latencia_isp: " << state.latencia_isp << " TIME: "<< state.clock1 <<endl;}
							state.latencia_isp+=x.latencia;
						}else if (x.source==-1){
							//Generador. No acumulo nada porque las esperadas vienen de afuera,.
						}else{
							if (debug){cout <<"========================= DATABASE 2: TAG = " << x.tag << ".Stats from peer: " << x.source << ". id_tarea: " << x.id_tarea << ". latencia: " << x.latencia <<". latencia_isp: " << state.latencia_isp << " TIME: "<< state.clock1<< endl;}
							state.TotalTasks =  x.id_tarea;
							state.latencia		+= x.latencia;
							state.terminadas++;	
							
							state.vector_finalizacion_peers.push_back(x.source);							
							state.Msg_list.push_back(x);
									
						}
						flag_DataIn=1;
					}	
												
				}
				if (flag_DataIn==1){
					if ( convert_ND_to_double(state.clock1 - state.t_inicio ) > state.t_observacion){
						state.t_inicio = state.clock1;
						if (1){cout << "TIME" << ";" << "D1" << ";" << "D2" << ";" << "D3" << ";" << "servers" << ";" << "nodes" << ";" << "queries_expected" << ";" << "terminadas" << ";" <<  "latencia" <<";"<< "latencia_isp" << endl;}
						if (1){cout << convert_ND_to_double(state.clock1) << ";" << param_server<< ";" << param_isp<<";" << param_peer<<";" << nro_servers <<  ";" << nodes << ";" << queries_expected << ";" << state.terminadas << ";" <<  state.latencia <<";"<< state.latencia_isp << endl;}
						//~ print_vec(state.load,"state.load");
						
					}
					 if (queries_expected==state.terminadas){
						if (1){cout << "TIME" << ";" << "D1" << ";" << "D2" << ";" << "D3" << ";" << "servers" << ";" << "nodes" << ";" << "queries_expected" << ";" << "terminadas" << ";" <<  "latencia" <<";"<< "latencia_isp" << endl;}
						if (1){cout << convert_ND_to_double(state.clock1) << ";" << param_server<< ";" << param_isp<<";" << param_peer<<";" << nro_servers <<  ";" << nodes << ";" << queries_expected << ";" << state.terminadas << ";" <<  state.latencia <<";"<< state.latencia_isp << endl;}
						//~ if (1){archivo_salida << "time"<< ";"  << "Peers"<< ";" << "nQueriesOut"<< ";" << "terminadas" <<  ";" << "queries_in"<<  ";"  <<  "queries_WSE" << ";" << "hitLocal"<< ";" << "hitGlobal"<< ";"  << "misses"<< ";"  << "avHops"<<  ";"  << "nReplicas" << ";"<< "gini" << ";"  << "latencia"<< ";"  << "latencia_isp"<< endl;}
						//~ if (1){archivo_salida << convert_ND_to_double(state.clock1) << ";" << ID_numb - 1 << ";" << queries_expected << ";" << state.terminadas << ";" << state.queries_in << ";" << state.queries_WSE << ";"  << state.local_hits << ";" << state.global_hits << ";" << state.misses << ";" << avHops <<  ";" << state.replicas << ";"<< gini << ";" << state.latencia << ";" << state.latencia_isp << endl;}
					}
					
					switch (state.estado)
					{
						case 0:
						{
							state.estado=1;
							state.next_internal=preparationTime;
							state.clock1 = state.clock1 + state.next_internal;
							break;
						}
						case 1:
						{
							state.estado=1;
							state.next_internal=preparationTime - e;
							break;
						}
					}
				}
					
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
				//std::cout << "DataBase_Simple"<< ID_numb << " - Output function. Estado: "<< state.estado << std::endl; 
				if(state.estado==0)
				{
					
				}
				else if (state.estado==1){
					
				}
				return bags;
            }
			//========================================================================================================================================
            // time_advance function
            //========================================================================================================================================
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
