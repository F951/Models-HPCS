#ifndef ANSWER_H
#define ANSWER_H
#include "../glob.h"


class Answer
{

public:
    long priority;
    string key;
    BIGNUM * hashValue;
    int size;

    int version;
    long int timeOut;

    ~Answer()
    {
       BN_free(hashValue);
    }

    Answer(double _priority,string s,  BIGNUM* _hashValue,
           long int _tout, int _version, int _size)
	  {
   priority = _priority;
  
   hashValue = BN_dup(_hashValue);
   size = _size;
   timeOut = _tout;
   version = _version;
   key=s;
	  }

    string getKey()
    {
       return key;
    }
    void update (long timestamps)
    {
      priority = timestamps;
    }

    Answer* clone()
    {
       Answer *tmp = new Answer(this->priority,this->key, this->hashValue, this->timeOut,
                                this->version,this->size);

       return tmp;
     }

long int getTimeOut()
{

   return timeOut;

}

int getVersion()
{
    return version;
}

void setNewVersion(long int ttl)
{
    timeOut=ttl;
    version=version+1;
}

  };
#endif 
