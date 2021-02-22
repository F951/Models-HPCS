#ifndef _MSG_WSE_H
#define _MSG_WSE_H

#include "../glob.h"
#include "ClientWSE.h"
#include "../p2pLayer/NodeEntry.h"

// Message sent by WSE

enum MessageWSEType 
{ 
   WSE_to_PEER = 0 ,
   PEER_to_WSE = 1 
};

class MessageWSE
{

public:

	handle<ClientWSE>* src;
	string key; 
	BIGNUM* query;
	
	double TTL_tarea; list<int> opciones; int prioridad_tarea; int tag; 
	int id_imagen;
	
	int version;
	int source;//peer or user
	int sentido;
	
	//BIGNUM* responsable_agreg;
	NodeEntry* responsable_agreg;
	Rtas_Peers* rta;
	MessageWSEType type;
	
	~MessageWSE()
	{
		//free( src);
        BN_free(query);
	}
	
	//========================================================================================================= Constructores nuevos
	MessageWSE(handle<ClientWSE>* _src,  BIGNUM* _query, string _key, int _srcK, double _TTL_tarea, int _tag, int _id_imagen, Rtas_Peers* _rta)
	{
		src    = _src;
		query  = BN_dup(_query);
		key.assign(_key);
		source = _srcK;   
		TTL_tarea	    = _TTL_tarea;
	    tag				= _tag;
		id_imagen		= _id_imagen;
		rta=_rta;
	}

	MessageWSE(handle<ClientWSE>* _src,  BIGNUM* _query, string _key, int _srcK, double _TTL_tarea, int _tag, int _id_imagen, Rtas_Peers* _rta, NodeEntry* _responsable_agreg)
	{
		src    = _src;
		query  = BN_dup(_query);
		key.assign(_key);
		source = _srcK;   
		TTL_tarea	    = _TTL_tarea;
	    tag				= _tag;
		id_imagen		= _id_imagen;
		rta=_rta;
		responsable_agreg = _responsable_agreg;
	}
	//=========================================================================================================
	
	string getKey()
	{
		return key;
	}

	BIGNUM* getQuery()
	{
		return query;
	}

	void setAnswer(int _version)
	{
		version=version;
	}

	int getVersion()
	{
		return version;
	}

	void set_type(MessageWSEType _type)	{
		type=_type;
	}
	
	int getType()	{
	  return type;
	}

};


#endif
