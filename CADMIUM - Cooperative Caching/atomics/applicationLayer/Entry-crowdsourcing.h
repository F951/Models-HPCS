#ifndef _ENTRY_h
#define _ENTRY_h
#include "../../glob.h"
//#include "../wse/Rtas_peers.h"


/* Class that represents an entry in Rcache or local Cache */

//class Rtas_Peers;

class Entry
{

public:
   long priority;
   string key;
   BIGNUM* hashValue;
   
   double TTL_tarea; list<int> opciones; int prioridad_tarea; 
   int id_imagen;
   // list<int> imagen;
   
   bool isReplicated;
   bool flag;
   int size;

   // new parameters to handle WSE entries
   int version;
   long int timeOut;

   // parameters to handle TTL of entries
  // bool valid;

	int frequency;
	//Rtas_Peers *rta;


   long int getTimeOut()
   {
      return timeOut;
   }

   ~Entry()
   {
      BN_free(hashValue);
   }

	//Entry(double _priority, string _key, BIGNUM* _hashValue,  int _version, int _size, long int time,double _TTL_tarea, list<int> _op, int _prioridad_tarea, /*list<int> _imagen*/int _id_imagen)
	//{
	//	priority = _priority;
	//	key = _key;
	//	hashValue = BN_dup(_hashValue);
	//	frequency = 0;
	//	size = _size;
	//	flag= true;
	//	// valid=true;
	//	isReplicated = false;
	//	timeOut= time;
	//	version= _version;
	//
	//	TTL_tarea	    = _TTL_tarea;
	//	prioridad_tarea = _prioridad_tarea;
	//
	//	for (list<int>::iterator it=_op.begin(); it!=_op.end(); it++)
	//	{
	//		opciones.push_back( *it );
	//	}
	//	// for (list<int>::iterator it=_imagen.begin(); it!=_imagen.end(); it++)
	//	// {
	//	// 	imagen.push_back( *it );
	//	// }
	//	id_imagen = _id_imagen;
	//	//~ rta = _rta;
	//}
   
   
   //~ Entry(double _priority, string _key, BIGNUM* _hashValue,  int _version, int _size, long int time,double _TTL_tarea, list<int> _op, int _prioridad_tarea, int _id_imagen
   Entry(double _priority, string _key, BIGNUM* _hashValue,  int _version, int _size, long int time,double _TTL_tarea, list<int> _op, int _prioridad_tarea, int _id_imagen
					//, Rtas_Peers* _rta
					)
   {
		priority = _priority;
		key = _key;
		hashValue = BN_dup(_hashValue);
		frequency = 0;
		size = _size;
		flag= true;
		// valid=true;
		isReplicated = false;
		timeOut= time;
		version= _version;
		
		TTL_tarea	    = _TTL_tarea;
		prioridad_tarea = _prioridad_tarea;

		for (list<int>::iterator it=_op.begin(); it!=_op.end(); it++)
		{
			opciones.push_back( *it );
		}
		id_imagen = _id_imagen;
		//rta = _rta;
   }

   // when the entry is used is updated (higher priority)
   void update(long timestamps)
   {
//      cout << "updateEntry" << endl;
 //     fflush(stdout);
    priority = timestamps;
   }


  void updateFrequency()
  {
  	frequency = frequency + 1;
  }



   // checks it the ttl is still valid  
   bool old(long timestamps)
   {
      if(timestamps > this->timeOut)
      {
      //   valid = false;
	 return true;
      } 
      else 
         return false;
   }

   // This simulates that a new entry from the WSE is recovered with a new TTL
  // void updateValidation(long int _timeOut, int _version)
  // {
  //    valid=true;
  //    this->timeOut = _timeOut;
  //    this->version = _version;
  // }

   // helps when sending an entry to other peer
   Entry* clone()
   {
      Entry *tmp = new Entry(this->priority, 
                             this->key,
			     this->hashValue,
			//     this->timeOut,
			     this->version,
			     this->size,
			     this->timeOut,
			     this->TTL_tarea, this->opciones, this->prioridad_tarea,this->id_imagen
				//,this->rta
				);
    //  tmp->valid = this->valid;
      //tmp->frequency = this->frequency;
    //  timeOut = STATIC_TIMEOUT;
      return tmp;
   }
  
  void reset()
  {
     frequency = 0;
  }

};

#endif
