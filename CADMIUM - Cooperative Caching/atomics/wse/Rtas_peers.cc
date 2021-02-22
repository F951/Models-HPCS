#include "Rtas_Peers.h"

//~ Rtas_Peers::Rtas_Peers(int _id_tarea,int _id_img, double _ttl, int _esp, 
//~ int _prioridad_tarea, list<int>& _opciones_originales, double _tiempo_inicio, double _tiempo_generacion)
//~ {
	//~ id_tarea			= _id_tarea;
	//~ id_img 				= _id_img;
	//~ TTL_tarea    		= _ttl;
	//~ rtas_esperadas 		= _esp;
	//~ rtas_recibidas 		= 0;
	//~ prioridad_tarea		= _prioridad_tarea;
	//~ reenviada=0;
	//~ for (list<int>::iterator it=_opciones_originales.begin(); it!=_opciones_originales.end(); it++)
	//~ {
		//~ //Se recorre la lista de opciones que envía el generador.
		//~ //ACÁ HAY QUE AGREGAR, PARA CADA OPCIÓN, UNA ENTRADA EN LA LISTA DE VOTACIÓN.
		//~ //¿HAY QUE DEFINIR EL NÚMERO DE OPCIONES, O SÓLO SE VAN AGREGANDO?
		//~ //CADA UNA DE LAS LISTAS DE OPCIONES EN PRINCIPIO SE DEJA VACÍA.
		//~ //SE VA A IR LLENANDO CUANDO VAYAN LLEGANDO LAS RESPUESTAS DE LOS PEERS.
		//~ /***ENTONCES, ACÁ NO HAGO MÁS NADA. LAS VOTACIONES LAS AGREGO DIRECTAMENTE EN EL CASE 3**/
		//~ opciones.push_back(0);
		//~ opciones_originales.push_back(*it);
	//~ }
	//~ tiempo_inicio		= _tiempo_inicio;
	//~ tiempo_generacion	= _tiempo_generacion;
//~ }

//~ void Rtas_Peers::copy(Rtas_Peers* rta_in)
//~ {
	//~ id_tarea=rta_in->id_tarea;
	//~ id_img=rta_in->id_img;
	//~ TTL_tarea=rta_in->TTL_tarea;
	//~ rtas_esperadas=rta_in->rtas_esperadas;
	//~ rtas_recibidas=rta_in->rtas_recibidas;
	//~ prioridad_tarea=rta_in->prioridad_tarea;
	//~ tiempo_generacion=rta_in->tiempo_generacion;
	//~ tiempo_inicio=rta_in->tiempo_inicio;
	//~ tiempo_inicio=rta_in->tiempo_inicio;
	//~ tiempo_finalizacion=rta_in->tiempo_finalizacion;
	//~ //Duplicar las listas de opciones y de peers consultados (para los casos en que haga falta).
	//~ std::list<int>::iterator it_lista;
	//~ for (it_lista=rta_in->peers_consultados.begin(); it_lista!=rta_in->peers_consultados.end(); it_lista++)
	//~ {
		//~ peers_consultados.push_back(*it_lista);
	//~ }
	//~ for (it_lista=rta_in->opciones.begin(); it_lista!=rta_in->opciones.end(); it_lista++)
	//~ {
		//~ opciones.push_back(*it_lista);
	//~ }
	//~ for (it_lista=rta_in->opciones_originales.begin(); it_lista!=rta_in->opciones_originales.end(); it_lista++)
	//~ {
		//~ opciones_originales.push_back(*it_lista);
	//~ }
//~ }
