#ifndef _CLIENTTLCWSE_H
#define _CLIENTTLCWSE_H

#include "../glob.h"
#include "../wse/ClientWSE.h"
#include "../wse/WSE.h"
#include "../wse/MessageWSE.h"
#include "Query.h"
#include "Entry.h"
#include "TlcMessage.h"
#include "../isp/ISP.h"

class TlcProtocol;


class ClientTlcWse: public ClientWSE
{
public:

//   list<MessageWSE *> queue_in;
	handle<WSE> *wse;
	handle<ClientWSE> *I_obj;
	handle<TlcProtocol> *tlc;
	Hash* h;
	double rate_trabajo;
	//~ rng<double>* work_stime;
	rng<double> *work_time1,*work_time2;
	int habilidad=0;

	BIGNUM* pid;
	int totalQueries;
	handle<Observer> *observer;
	handle<ISP> *isp;
	
	//Fin simulacion
	int *ends, NP;
	double cte_atencion;
	bool end_cliente=0;
	
	map<int, Rtas_Peers*> en_proceso_local;
	int consultas_totales = 0;
	int contador_miss_en_proceso=0;
	int enviadas=0;	
	int H;
	int flag_terminar;

   ~ClientTlcWse ( )
   {
      //free (wse);
      //free(tlc);
   }

   ClientTlcWse ( const string& name, handle<WSE> *_wse , double _client_wse_cte, int _habilidad,
					BIGNUM* _pid,
					handle<Observer> *_obs,
					int _tQueries,
					int *_ends,
					int _NP	)
	            : ClientWSE ( name )
   {
		wse=_wse;
		//rate_trabajo=_client_wse_cte;
		//~ work_time = new rngUniform( "Work Time", 0.1,20 );
		work_time1 = new rngNormal( "Normal", 1.0, 0.1 );
		work_time2 = new rngExp( "NegExp 2", 0.6 );
		work_time1->reset();
		work_time2->reset();
		habilidad = _habilidad;
		NP   = _NP;   
		pid  = _pid;
		observer = _obs;
		totalQueries = _tQueries;
		ends = _ends;  
		flag_terminar=0;
		//~ escritura_a_archivo::GetStream() << "Habilidad peer: " << habilidad << endl;
   }

   void inner_body(void);

   void set_tlc(handle<TlcProtocol> *t);
   
   void set_isp(handle<ISP>* _isp){
		isp=_isp;
   }

   /*Implemented to make the connection with WSE */
   void set_ClientWSE_obj(handle<ClientTlcWse> *t)
   {
      I_obj = (handle<ClientWSE>*)t;
      
   }

   handle<ClientWSE>* get_ClientWSE_obj()
   {
      return I_obj;
   }

   void sendWSEQuery(Query *q);

   long int getTTL(BIGNUM *);
   long int getTimeIncremental(long int);
			
	void printLista(std::list<int>& ,const char* , int);
	
	void mostrar_tareas_en_proceso_local();
	void mostrar_tareas_en_proceso();
	
	void print_rta(Rtas_Peers*& rta);
	void acumular_rtas(Rtas_Peers* rta, int id_peer,int indice);

	int calcular_consenso(int indice, 
							int peers_reenvio, 
							map<int, Rtas_Peers*> &en_proceso_local,
							double umbral_votacion, 
							int H,
							MessageWSE* &temp
							);
	void diferencias_en_listas(list<int> &lista1, list<int> &lista2, int indice);
	
	
	void terminar(  ){
		flag_terminar=1;		
	}
	
};



#endif
