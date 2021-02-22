#ifndef Rtas_Peers_H
#define Rtas_Peers_H
#include "../glob.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../p2pLayer/PastryMessage.h"



class Rtas_Peers
{
public:
	int consenso;
	int id_tarea;
	int id_img;
	double TTL_tarea;
	int rtas_esperadas;
	int rtas_recibidas;
	int prioridad_tarea;
	double tiempo_generacion; 
	double tiempo_inicio;  
	double tiempo_finalizacion; 
	list<int> opciones;
	list<int> opciones_originales;
	map<int, list<int> > votacion;
	list<int> peers_consultados_1;
	list<int> peers_consultados_2;
	list<int> peers_que_respondieron_1;
	list<int> peers_que_respondieron_2;
	
	NodeEntry* Responsable_agreg;
	int responsable_asignado=0;

	int reenviada; 
	int enviada_al_wse_1; 
	int enviada_al_wse_2; 
	
	Rtas_Peers(){}
	
	Rtas_Peers(int _id_tarea,int _id_img, double _ttl, int _esp, int _prioridad_tarea, list<int>& _opciones_originales, double _tiempo_inicio, double _tiempo_generacion)
	{
		id_tarea			= _id_tarea;
		id_img 				= _id_img;
		TTL_tarea    		= _ttl;
		rtas_esperadas 		= _esp;
		rtas_recibidas 		= 0;
		prioridad_tarea		= _prioridad_tarea;
		consenso=0;
		reenviada=0;
		enviada_al_wse_1=0;
		enviada_al_wse_2=0;
		for (list<int>::iterator it=_opciones_originales.begin(); it!=_opciones_originales.end(); it++)
		{
			//Se recorre la lista de opciones que envía el generador.
			//ACÁ HAY QUE AGREGAR, PARA CADA OPCIÓN, UNA ENTRADA EN LA LISTA DE VOTACIÓN.
			//¿HAY QUE DEFINIR EL NÚMERO DE OPCIONES, O SÓLO SE VAN AGREGANDO?
			//CADA UNA DE LAS LISTAS DE OPCIONES EN PRINCIPIO SE DEJA VACÍA.
			//SE VA A IR LLENANDO CUANDO VAYAN LLEGANDO LAS RESPUESTAS DE LOS PEERS.
			/***ENTONCES, ACÁ NO HAGO MÁS NADA. LAS VOTACIONES LAS AGREGO DIRECTAMENTE EN EL CASE 3**/
			opciones.push_back(0);
			opciones_originales.push_back(*it);
		}
		tiempo_inicio		= _tiempo_inicio;
		tiempo_generacion	= _tiempo_generacion;
		responsable_asignado=0;
	}

	void copy(Rtas_Peers* & rta_in)
	{
		id_tarea=rta_in->id_tarea;
		consenso=rta_in->consenso;
		id_img=rta_in->id_img;
		TTL_tarea=rta_in->TTL_tarea;
		rtas_esperadas=rta_in->rtas_esperadas;
		rtas_recibidas=rta_in->rtas_recibidas;
		prioridad_tarea=rta_in->prioridad_tarea;
		reenviada=rta_in->reenviada;
		tiempo_generacion=rta_in->tiempo_generacion;
		tiempo_inicio=rta_in->tiempo_inicio;
		tiempo_inicio=rta_in->tiempo_inicio;
		tiempo_finalizacion=rta_in->tiempo_finalizacion;
		//Duplicar las listas de opciones y de peers consultados (para los casos en que haga falta).
		std::list<int>::iterator it_lista;
		for (it_lista=rta_in->peers_consultados_1.begin(); it_lista!=rta_in->peers_consultados_1.end(); it_lista++){
			peers_consultados_1.push_back(*it_lista);
		}
		for (it_lista=rta_in->peers_consultados_2.begin(); it_lista!=rta_in->peers_consultados_2.end(); it_lista++){
			peers_consultados_2.push_back(*it_lista);
		}
		for (it_lista=rta_in->peers_que_respondieron_1.begin(); it_lista!=rta_in->peers_que_respondieron_1.end(); it_lista++){
			peers_que_respondieron_1.push_back(*it_lista);
		}
		for (it_lista=rta_in->peers_que_respondieron_2.begin(); it_lista!=rta_in->peers_que_respondieron_2.end(); it_lista++){
			peers_que_respondieron_2.push_back(*it_lista);
		}
		for (it_lista=rta_in->opciones.begin(); it_lista!=rta_in->opciones.end(); it_lista++){
			opciones.push_back(*it_lista);
		}
		for (it_lista=rta_in->opciones_originales.begin(); it_lista!=rta_in->opciones_originales.end(); it_lista++){
			opciones_originales.push_back(*it_lista);
		}
		votacion=rta_in->votacion;
		
	}
	
	void print_rta(){
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"------------------ id_tarea ------#: " << this->id_tarea << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"id_img: " << this->id_img << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"TTL_tarea: " << this->TTL_tarea << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"rtas_esperadas: " << this->rtas_esperadas << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"rtas_recibidas: " << this->rtas_recibidas << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"prioridad_tarea: " << this->prioridad_tarea << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"tiempo_generacion: " << this->tiempo_generacion << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"tiempo_inicio: " << this->tiempo_inicio << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"tiempo_finalizacion: " << this->tiempo_finalizacion << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"reenviada: " << this->reenviada << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"enviada al servidor 1: " << this->enviada_al_wse_1 << endl;
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"enviada al servidor 2: " << this->enviada_al_wse_2 << endl;
		//~ printLista(opciones,"opciones");
		printLista(peers_consultados_1,"peers_consultados_1",this->id_tarea);
		printLista(peers_consultados_2,"peers_consultados_2",this->id_tarea);
		printLista(peers_que_respondieron_1,"peers_que_respondieron_1",this->id_tarea);
		printLista(peers_que_respondieron_2,"peers_que_respondieron_2",this->id_tarea);
		//~ escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"Mapa de votación: " << endl;
		//~ imprimir_mapa_votacion();	
		escritura_a_archivo::GetStream()<< "#"<<this->id_tarea<<"---------------------------------"<< endl;
	}
	
	//void imprimir_mapa_votacion(){
	//	for(map<int,list<int> >::const_iterator it = votacion.begin(); it != votacion.end(); ++it){
	//		list<int> lista(it->second);
	//		cout << "Elemento: " << it->first << endl;
	//		printLista(lista,"---");
	//	}
	//}
		
	
	//void printLista(std::list<int>& lista,const char* nombre)
	//{
	//	std::list<int>::iterator it_lista;
	//	cout << "==============Lista: "<< nombre << "\n";
	//	for (it_lista=lista.begin(); it_lista!=lista.end(); ++it_lista)
	//	{
	//		std::cout << ' ' << *it_lista;
	//	} 
	//	cout << "\n==============\n";
	//}
	void printLista(std::list<int> & lista,const char* nombre, int indice)
	{
		std::list<int>::iterator it_lista;
		escritura_a_archivo::GetStream() << "#"<<indice<<". ==============Lista: "<< nombre << "\n";
		escritura_a_archivo::GetStream() << "#"<<indice<<". ";
		for (it_lista=lista.begin(); it_lista!=lista.end(); ++it_lista)
		{
			//~ std::escritura_a_archivo::GetStream() << ' ' << *it_lista;
			escritura_a_archivo::GetStream() << ' ' << *it_lista;
		} 
		escritura_a_archivo::GetStream() << "\n#"<<indice<<". ==============\n";
	}


};


#endif 
