#ifndef _OBSERVER_h
#define _OBSERVER_h

#include "glob.h"

class Observer: public process {

private:

   string s;
 
   int miss;
   int hitLocal;
   int hitGlobal;
   int hitLCache;

   int nqueriesIn;
   int nqueriesOut;
   int np;

   int hops;
   double latencia;
	double latencia_network;
	
   double deltaT;
   bool busy, ends;

   int nReplicas;
   int iteracion;
   int nro_servers;


public:
	int terminadas;
	int totalQueries;
   vector<int> load;
	int param_server;
	int param_network;
	int param_peer;
	
   ~Observer()
   {

   }

   Observer(int _np, const string& name, int _totalQueries, int _nro_servers):process(name)
   {

      np=_np;
      miss = 0;
      hitLocal = 0;
      hitGlobal=0;
      hitLCache=0;
      nqueriesIn = 0;
      nqueriesOut =0;
      hops = 0;
      latencia=0.0;
      latencia_network=0.0;
      deltaT = 0.1;
      ends = false;   
      busy = false;
      nReplicas=0;
      iteracion = 0;
		terminadas=0;
		totalQueries = _totalQueries;
		nro_servers = _nro_servers;
     for(int i=0; i< np ;i++)
     {
       load.push_back(0);
     }

   }

   void inner_body( void );

   void addMiss()
   {
      miss++;
   }

   bool isBusy()
   {
      return busy;
   }

   void setBusy(bool p)
   {
      busy=p;
   }

   void addHitLocal()
   {
      hitLocal++;
   }

   void addHitGlobal()
   {
      hitGlobal++;
   }

   void addHitLCache()
   {
      hitLCache++;
   }

   void addReplicas()
   {
      nReplicas++;
   }

   void addNQueriesIn()
   {
      nqueriesIn++;
   }
   
   void addNQueriesOut()
   {
      nqueriesOut++;
   }

   void addHops(int h)
   {
      hops= hops + h;
   }

   void addLatencia(double t)
   {
      latencia= latencia+t;
   }
   
      void addLatencia_network(double t)
   {
      latencia_network= latencia_network+t;
   }
   void end()
   {
      ends = true;
   }

   void print()
   {
    cout << s << endl;
   }

   void addLoad(int ip)
   {
	   cout << "OBSERVER _ FUNCION LOAD _ Incrementar carga del peer: " << ip << endl;
      load[ip]++;
		for (int k=0;k<(int)(load.size());k++){
			cout << " " << load[k];
		}cout << "\n";
   }

   double getGini();

  int getIteracion();
};

#endif
