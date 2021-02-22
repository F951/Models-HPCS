#ifndef MessageT_MESSAGE_HPP
#define MessageT_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>
#include "../glob.h"
#include "../atomics/p2pLayer/NodeEntry.h"
#include "../atomics/applicationLayer/LCache.h"
#include "../atomics/applicationLayer/Entry.h"

using namespace std;

enum MessageTType { T_P2P = 0,
                    T_BDC = 1
		    };

//Esto sirve para ver el peso de los datos que transporta el msje, y simular la latencia correpondiente.
enum DataType { SOLICITUD_TAREA = 20,			//Este nunca se usa, porque los mensajes de solicitud de tarea llegan direcmtamente al voluntario, desde el wse, a través del isp.		
				SOLICITUD_OBJETO = 21,
				OBJETO_OPCIONES = 22,
				TRABAJO_TERMINADO = 23,
				TRABAJO_AGREGADO = 24		//Esta no la uso. La mezclo con TRABAJO_TERMINADO.
		    };

/*******************************************/
/**************** MessageT ****************/
/*******************************************/
class MessageT{
public:
	int src; // index of the peer source of the message
	int dest; // index of the peer destination of the meesage
	void *DATA; // data transmited by this message
	MessageTType type;
	DataType dataType;
	
	//Constructor
	MessageT(MessageTType _type, void* _D, int _src, int _dest)
	{
		src= _src;
		dest = _dest;
		DATA=_D;
		type=_type;
		setDataType((DataType)0);
	}
	
	int getType(){
		return type;
	}

	void setType(MessageTType _t){
		type=_t;
	}

	int getDataType(){
		return dataType;
	}
	void setDataType(DataType _t){
		dataType=_t;
	}

	void * getData(){
		return DATA;
	}

	MessageT* clone(){
		MessageT* newMsg = new MessageT(this->type, this->DATA, this->src, this->dest);
		return newMsg;
	}

	int getDest(){
		return dest;
	}

	int getSrc(){
		return src;
	}
	
};


istream& operator>> (istream& is, MessageT& msg);

ostream& operator<<(ostream& os, const MessageT& msg);


#endif // MessageT_MESSAGE_HPP
