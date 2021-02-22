#ifndef _MSG_WSE_H
#define _MSG_WSE_H

#include "../glob.h"
#include "ClientWSE.h"

// Message sent by WSE

class MessageWSE
{

public:

		handle<ClientWSE>* src;
        string key; 
        BIGNUM* query;

		int version;
        int source;//peer or user


        ~MessageWSE()
	{
//        free( src);
         BN_free(query);
	}

	MessageWSE(handle<ClientWSE>* _src,  BIGNUM* _query, string _key, int _srcK)
	{
		src    = _src;
		query  = BN_dup(_query);
		key.assign(_key);
                source = _srcK;   
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
