#ifndef BOOST_SIMULATION_MESSAGE_HPP
#define BOOST_SIMULATION_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>
#include "../glob.h"
#include "../atomics/p2pLayer/NodeEntry.h"


enum Direction {
	DIR_UP = 0,
	DIR_DOWN = 1
};

enum LayerFlag {
	CLIENT_LAYER = 0,
	TLC_LAYER = 1,
	PASTRY_LAYER = 2
};

//~ enum MessageType 
//~ { 
	//~ //CLIENT - SERVER
	//~ //TAG
	//~ ISP_TASK_OUT		= 0,
	//~ ISP_IMG_RQST		= 1,
	//~ ISP_IMG_SENT		= 2,
	//~ ISP_TASK_RESP		= 3,
	//~ ISP_RESULT			= 4,
	
	//~ //TLC
	//~ TLC_LOOKUP 			= 10,
	//~ TLC_REPLY 			= 11,
	//~ TLC_REPLICATE 		= 12, 
	//~ TLC_LCACHE  		= 13, 
	//~ TLC_WSE  			= 14,
	//~ TLC_ENDS  			= 15,
	//~ TLC_LC_ACK  		= 16,
	//~ TLC_SEND_RESP  		= 17,
	//~ TLC_AGGREGATE  		= 18,

	//~ //Pastry
	//~ PASTRY_SENDDIRECT  	= 20,
	//~ PASTRY_SENDDHT 		= 21,  
	//~ PASTRY_RECEIVE 		= 22, 
	//~ PASTRY_ROUTE  		= 23,
	//~ PASTRY_BDC  		= 24,
	//~ PASTRY_BDC_RECEIVE 	= 25
	
	//~ //Transport
	//~ //No need to use tags, there's only one type of msg
	   
//~ };

//Esto sirve para ver el peso de los datos que transporta el msje, y simular la latencia correpondiente.
enum DataType { 
			SOLICITUD_TAREA = 20,			//Este nunca se usa, porque los mensajes de solicitud de tarea llegan direcmtamente al voluntario, desde el wse, a través del isp.		
			SOLICITUD_OBJETO = 21,
			OBJETO_OPCIONES = 22,
			TRABAJO_TERMINADO = 23,
			TRABAJO_AGREGADO = 24		//Esta no la uso. La mezclo con TRABAJO_TERMINADO.
		};

using namespace std;

//~ /*******************************************/
//~ /**************** Message_t ****************/
//~ /*******************************************/
//~ struct Message_t{
  //~ Message_t(){}
  //~ Message_t(int i_packet, int i_bit, int _source, int _dest)
   //~ :packet(i_packet), bit(i_bit){
		//~ source=_source; 
		//~ dest=_dest;
	//~ }
	//~ int source;
	//~ int dest;
	//~ int tag;
  	//~ int packet;	//Este es el ID de la tarea.
  	//~ int bit;
  	//~ DataType MsgType;
  	//~ LayerFlag layer;
  	//~ Direction direction;
  	//~ MessageType tag_Client_layer;
  	//~ MessageType tag_Tlc_layer;
  	//~ MessageType tag_Pastry_layer;
  	//~ int id_tarea;
  	//~ BIGNUM * hashValue;
  	//~ int id_imagen;
  	//~ NodeEntry* src_node;
  	//~ NodeEntry* dest_node;
//~ };
/*******************************************/
/**************** Message_t ****************/
/*******************************************/
class Message_t{
public:
	int tag;
  	int packet;	//Este es el ID de la tarea.
  	int bit;
  	int id_tarea;
  	BIGNUM * hashValue;
  	int id_imagen;
  	
  	int source;					//Transport
	int dest;					//Transport
	BIGNUM* hash_source;		//p2p
	BIGNUM* hash_dest;			//p2p
	NodeEntry* src_node;		//p2p
  	NodeEntry* dest_node;		//p2p
	
  	//~ DataType MsgType;
  	LayerFlag layer;
  	Direction direction;
  	//~ MessageType tag_Client_layer;
  	//~ MessageType tag_Tlc_layer;
  	//~ MessageType tag_Pastry_layer;
  	
  	
  	bool finalDestination;
  Message_t(){}
  Message_t(int i_packet, int i_bit, int _source, int _dest)
   {	
		packet = i_packet;
		bit = i_bit;
		source=_source; 
		dest=_dest;
		finalDestination=0;
	}

	Message_t copy(Message_t m_in){
			source=				m_in.source;
			dest=				m_in.dest;		
			tag=				m_in.tag;
			packet=				m_in.packet;	//Este es el ID de la tarea.
			bit=				m_in.bit;
			//~ MsgType=			m_in.MsgType;
			layer=				m_in.layer;
			direction=			m_in.direction;
			//~ tag_Client_layer=	m_in.tag_Client_layer;
			//~ tag_Tlc_layer=		m_in.tag_Tlc_layer;
			//~ tag_Pastry_layer=	m_in.tag_Pastry_layer;
			id_tarea=			m_in.id_tarea;
			hashValue=			m_in.hashValue;
			id_imagen=			m_in.id_imagen;
			src_node=			m_in.src_node;
			dest_node=			m_in.dest_node;
			finalDestination = m_in.finalDestination;
			hash_source		=m_in.hash_source;
			hash_dest		=m_in.hash_dest;
		}
};



istream& operator>> (istream& is, Message_t& msg);

ostream& operator<<(ostream& os, const Message_t& msg);


#endif // BOOST_SIMULATION_MESSAGE_HPP
