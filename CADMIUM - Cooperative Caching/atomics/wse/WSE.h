#ifndef WSE_H
#define WSE_H

#include "../glob.h"
#include "Rtas_peers.h"
#include "MessageWSE.h"
#include "LruA.h"

#include "../auxiliar/Util.h"
#include "../auxiliar/Hash.h"
#include "../Observer.h"

#include <iostream>
#include "../generador/Mje_Gen_WSE.h"
#include <ctime>




//#define WSECACHESIZE 8000

class TlcProtocol;
class ClientTlcWse;
class BD;

class Observer;
class ISP;

class WSE: public process
{

private:
  
   // ID_PAGE , (TTL, VERSION)
//   map<BIGNUM*, pair<long int,int> > answers;
   
   //~ int *totalQueries, sentQueries, lastStepQueries; 
	vector<handle<TlcProtocol> *> Peers;
	vector<handle<ClientTlcWse> *> clientes_wse_Peers;


public:
	handle<BD> *bd;
	list<Mje_Gen_WSE*> queue_in_Gen; 	//Msjes del Generador
	list<MessageWSE*> queue_in;			//Msjes de los peers
	
	handle<WSE> *puntero_propio;
	handle<Observer> *observer;
	handle<ISP> *isp;
	bool busy;
	LRUA *ANSWERS;
	int cacheSize=0;
	map<int, Rtas_Peers*> en_proceso;//Lista numerada de tareas, con la ID de imagen, el TTL, y un contador para ver cuándo respondieron todos los peers

	int end_WSE;
	int *ends;
	int Nuser;
	Hash* h;
	int H;
	double rate_reenvio;
	int totalQueries;
	multimap<int,int> mapa_prioridad;
	list<int> lista_reenvio;
	double parametro_TTL;
	int NP;
	// vector <double> utilizacion;
	double umbral_votacion;
	int enviadas;
	
	double promedio_ponderado_en_proceso;
	double promedio_ponderado_queue_in;
	double start_virtual_t_wse_gen;
	double stop_virtual_t_wse_gen;
	double virtual_t_wse_gen;
	double start_virtual_t_wse_peers;
	double stop_virtual_t_wse_peers;
	double virtual_t_wse_peers;
	double t_com_wse_bd;
	double perc_consenso;
	long double cpu_time_Wse;
	int consultas_totales;
	int consultas_enviadas_desde_peers=0;	//Es para comparar con consultas_totales, a ver si se están enviando bien y se reciben mal (error de ceros).
	int consultas_enviadas_desde_tlc=0;	//Es para comparar con consultas_totales, a ver si se están enviando bien y se reciben mal (error de ceros).
	int gen_stop = 0;
	double control_TTLs_anterior=0;
	int debug_repeticion_responsables=0;
	vector <int> SolvedTasks;
	
   ~WSE()
   {
    //  answers.clear();
      delete ANSWERS;
   }

   WSE ( const string& name,handle<BD> *_bd,  int _NP,int *_ends,int _H, double _parametro_TTL, int _totalQueries, double _umbral_votacion): process ( name )  
   {
		busy=false;
		int cacheSize = WSECACHESIZE; 
		ANSWERS = new LRUA(&cacheSize); 
		bd=_bd; 
		h = new Hash();
		NP              = _NP;
		ends            = _ends;
		H		= _H;
		end_WSE			=0;
		parametro_TTL = _parametro_TTL;
		totalQueries 	= _totalQueries;
		umbral_votacion = _umbral_votacion/100.0;
		promedio_ponderado_en_proceso=0;
		promedio_ponderado_queue_in=0;
		virtual_t_wse_gen = 0;
		virtual_t_wse_peers = 0;
		t_com_wse_bd = 0;
		perc_consenso=0;
		cpu_time_Wse=0;
        consultas_totales = 0;
		enviadas=0;
		
   }

   /*Implemented to make the connection with WSE */
   void set_WSE_obj(handle<WSE> *t)
   {
      puntero_propio = t;
   }
   
   void set_handle_obs(handle<Observer> *_obs)
   {
	   observer=_obs;
   }
   
	void set_handle_isp(handle<ISP> *_isp)
	{
		isp=_isp;
	}
	
   //~ handle<ClientWSE>* get_WSE_obj()
   //~ {
      //~ return I_obj;
   //~ }


   void inner_body ( void );

   void add_request ( MessageWSE* bn )
   {
      queue_in.push_back(bn);
   }

   bool get_busy()
   {
      return busy;
   }

   void print()
   {
      escritura_a_archivo::GetStream() << "WSE" << endl;
   }

   long int getTTL(BIGNUM*);
   //int getVersion(BIGNUM*);
   int getVersion(string, BIGNUM*);
   long int randomTTL();
   int calcular_votacion(int , int ,map<int, Rtas_Peers*> &, vector <int> &, vector <int> &,vector <int> &, vector <double> &, vector <double> &,
						int , int H, int NP, multimap<int,int> &, int *, vector<handle<TlcProtocol> *>&, 	vector<handle<ClientTlcWse> *> &,
						handle<BD> *bd,	vector<int> &,
						vector<int> &,
						vector<double> &
						);	
	void encontrar_maximos(vector<double>&, vector<int>&, int H);		
	void encontrar_minimos(vector<double>&, vector<int>&, int H);			
	void print_vec(vector<int> &, const char* ,int );
	void print_vec_double(vector<double> &, const char* );
					
  void add_peer(handle<TlcProtocol> * p)
 {
    Peers.push_back(p);
 }
   void add_peer(handle<ClientTlcWse> * p)
 {
    clientes_wse_Peers.push_back(p);
 }
 
    void free_WSE()
   {
      Peers.clear();
      queue_in.clear();
      queue_in_Gen.clear();
      //~ delete util;
      //~ delete h;
      //~ delete arrival_time;
      //~ delete SelectSource;
      
      //free(observer);
      //free(wse);
   }
 
 	void end_replicas()
	{
		end_WSE=1;
	}
	
	//Mjes del Generador
	void add_request_Gen( Mje_Gen_WSE *_m)
	{
		queue_in_Gen.push_back(_m);
	}
	
	//Esta función es muy práctica para depurar el funcionamiento del multimapa, pero no funciona en el cluster. Sólo me sirve en esta notebook.
	//void printMultimap(const std::multimap<int, int>& multiMap)
	//{
	//	escritura_a_archivo::GetStream() << "==============MultiMap:\n";
	//	for (const auto& pair : multiMap)
	//	{
	//		escritura_a_archivo::GetStream() << pair.first << ":" << pair.second << "\n";
	//	}
	//	escritura_a_archivo::GetStream() << "==============\n";
	//}
	
	void printLista(std::list<int>& lista,const char* nombre)
	{
		std::list<int>::iterator it_lista;
		escritura_a_archivo::GetStream() << "==============Lista: "<< nombre << "\n";
		for (it_lista=lista.begin(); it_lista!=lista.end(); ++it_lista)
		{
			escritura_a_archivo::GetStream() << ' ' << *it_lista;
			escritura_a_archivo::GetStream() << ' ' << *it_lista;
		} 
		escritura_a_archivo::GetStream() << "\n==============\n";
	}
	
	void actualizar_frecuencias(vector <int> &vector_frecuencias, multimap<int,int> &mymultimap, list<int> &peers_a_actualizar)
	{
		//~ std::clock_t c_start;//------------------------------------------------------------------------------------------------------------
		//~ std::clock_t c_end;
		//~ long double time_elapsed_ms;
		//~ int SAVE_CPUtime_STATS=1;
		//~ ofstream archivo_actualizar_frecuencias;
		//~ if (SAVE_CPUtime_STATS) {
			//~ archivo_actualizar_frecuencias.open ("stats_CPUtime/archivo_actualizar_frecuencias.txt",ios_base::app);
		//~ }
		//~ c_start = std::clock();//------------------------------------------------------------------------------------------------------------
		
		//iterador para la lista.
		list<int>::iterator it_opciones;
		//~ int clave;
		int id_peer;
		for (it_opciones=peers_a_actualizar.begin();it_opciones!=peers_a_actualizar.end();it_opciones++)
		{
			id_peer = *it_opciones;
			if (0) {escritura_a_archivo::GetStream() << id_peer << std::endl;}
		}
		for (it_opciones=peers_a_actualizar.begin();it_opciones!=peers_a_actualizar.end();it_opciones++)
		{
			id_peer = *it_opciones;
			if (0) {escritura_a_archivo::GetStream() << "Actualizar frecuencia de peer:"<< id_peer << std::endl;}
			//~ clave = vector_frecuencias[id_peer]; 
			vector_frecuencias[id_peer] +=1;
		}
		//~ c_end = std::clock();//------------------------------------------------------------------------------------
		//~ time_elapsed_ms = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
		//~ if (SAVE_CPUtime_STATS) { archivo_actualizar_frecuencias << time_elapsed_ms << ";" << H << endl;}//--------------------------------
		
		//Al finalizar, tengo todas las entradas actualizadas en el arreglo y en el mapa, y la lista queda vacía.		
		/*** peers_a_actualizar.clear();***/
		//~ std::escritura_a_archivo::GetStream() << "Presione Enter:"<< std::endl;
		//~ std::cin.get();
	}	
	
void print_rta(Rtas_Peers*& rta);	

void terminar_tarea(map<int, Rtas_Peers*> &, int , handle<BD> * & );

void finalizacion();

void control_TTLs();

void revisar_vencimientos();

void actualizar_puntajes(vector <int> &, vector <double> &,	vector <double> &,
						vector <int> &,	vector <int> &,
						vector<handle<TlcProtocol> *> &
						);

int reenvio_tarea( int , int , 	map<int, Rtas_Peers*> &,
					vector <double> &,	int ,	int *,
					vector<handle<TlcProtocol> *> &
					);
					
void termino_tarea( int , map<int, Rtas_Peers*> &, int,  handle<BD> *	);

void mostrar_tareas_en_proceso(int );

void mostrar_tareas_en_proceso_cout(int indice);

void acumular_rtas(Rtas_Peers* rta, int id_peer,int indice);

int calcular_consenso(int indice, 
							int peers_reenvio, 
							map<int, Rtas_Peers*> &en_proceso,
							double umbral_votacion, 
							int H,
							MessageWSE* &temp
							);

int seleccion_peer_agregacion(vector<double> &);

int select_random(vector<int> &);

};



#endif 
