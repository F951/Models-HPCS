#ifndef _MSG_H
#define _MSG_H

#include "../glob.h"
#include "ClientWSE.h"

// Message sent by WSE

class Message
{

public:

	handle<ClientWSE>* src;
	string key; 
	BIGNUM* query;
	int server;
	int version;
	int source;//peer or user
	double start_time;
	double size;
	int id_tarea;

	~Message()
	{
//        free( src);
         BN_free(query);
	}

	Message(handle<ClientWSE>* _src,  BIGNUM* _query, string _key, int _srcK, int _server, double _size, int _id_tarea)
	{
		src    = _src;
		query  = BN_dup(_query);
		key.assign(_key);
		source = _srcK;   
		server= _server;
		size = _size;
		id_tarea =  _id_tarea;
	}

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

};


#endif
