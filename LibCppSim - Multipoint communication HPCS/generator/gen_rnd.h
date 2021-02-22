#ifndef GEN_H
#define GEN_H

#include "../glob.h"
#include "../auxiliar/Util.h"
#include "../auxiliar/Hash.h"
#include "../Observer.h"
#include "../server/SERVER.h"
#include "../server/Message.h"

class TlcProtocol;

class Gen_rnd:public process
{  
  
  int *totalQueries, sentQueries, lastStepQueries; 


  int NP;
  UTIL *util; 
  Hash* h;
  char traces_file[2048];
  rng<double>* arrival_time;
  bool phase; 

  rng<double>* SelectSource;

  double prev, actual;
  ifstream endStream;
  vector<string> tokens;

  double CTE;
  double *init_time;
  
  handle<Observer> *observer;
  
  vector<handle<SERVER>*> server;
  
  //ends simulation
  int *ends;
  int Nuser;
  int Totaluser; 
  int porcentaje_peers;

  int Peer_Selection;
 int nro_servers;
protected:
  void inner_body( void );
   
public:

   void freeGen_rnd()
   {
      delete util;
      delete h;
      delete arrival_time;
      delete SelectSource;
      
   }

  Gen_rnd ( const string& name, char *_traces_file, int *_totalQueries, 
            int _NP, int _nro_servers, handle<Observer> *obs, int *_ends, int _Nuser,
	     int _PS ): process( name )
  {
	  nro_servers = _nro_servers;
     totalQueries    = _totalQueries;
     sentQueries     = 0;
     lastStepQueries = 0;
     Nuser           = _Nuser;   
     NP              = _NP;
     Totaluser       = NP + Nuser; 
     porcentaje_peers= (NP*100)/Totaluser;

     util            = new UTIL();
     h               = new Hash();
     strcpy( traces_file, _traces_file );
     ends            = _ends;
     Peer_Selection = _PS;
     phase=false;
     
     double media1 = (1.0/NORMAL_RATE);
     arrival_time = new rngExp( "Arrive Time", media1 );
     arrival_time->reset();

     SelectSource = new rngUniform("SelectSource", 0,100);
     SelectSource->reset();

     prev = 0.0;
     actual   = 0.0;
     observer = obs;
  }
 
  void add_server(handle<SERVER> * p)
 {
    server.push_back(p);
 }
 
 int getZipf(int, int);
 void setQueryRate(int);
 double rand_val(int);
};
#endif
