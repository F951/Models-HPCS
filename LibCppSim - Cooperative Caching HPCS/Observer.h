#ifndef _OBSERVER_h
#define _OBSERVER_h

#include "glob.h"
#include "generador/gen_rnd.h"

class Gen_rnd;

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
   double deltaT;
   bool busy, ends;

   int nReplicas;
   int iteracion;

   vector<int> load;
	int totalqueries;
	handle<Gen_rnd> * gen;
	
public:

   ~Observer()
   {

   }

   Observer(int _np, const string& name, int _totalqueries):process(name)
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
      deltaT = 0.1;
      ends = false;   
      busy = false;
      nReplicas=0;
      iteracion = 0;
      totalqueries = _totalqueries;

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
	   //~ cout << "OBSERVER _ FUNCION LOAD _ Incrementar carga del peer: " << ip << endl;
      load[ip]++;
		//~ for (int k=0;k<(int)(load.size());k++){
			//~ cout << " " << load[k];
		//~ }cout << "\n";
   }
   
	void addGen(handle<Gen_rnd> * _gen)
	{
		gen = _gen;
	}

   double getGini();

  int getIteracion();
};

#endif
