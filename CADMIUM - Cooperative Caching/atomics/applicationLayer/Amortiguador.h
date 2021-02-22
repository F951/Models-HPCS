#ifndef AMORTIGUADOR_H
#define AMORTIGUADOR_H

#include "../glob.h"
#include "../../auxiliar/Util.h"
#include "../p2pLayer/NodeEntry.h"

class nodo //cola prioridad de 1 qi
{
   public:
     NodeEntry* id;  //ID peer solicitante
     double costo;
     double Tsalida;
     double thao;
     int    arc;
    
     nodo(NodeEntry* x, double _thao)
     {
        id      = x;
	costo   = 0.0;
	Tsalida = 0.0;
	arc     = 0;
	thao    = _thao;
     }
};

struct Compara: binary_function<nodo*,nodo*,bool>
{
   bool operator()(nodo *t1, nodo *t2)
   {
      if ( t1->Tsalida > t2->Tsalida )
           return true;
      else return false;
   }
};

class Amortiguador
{
  vector<string> tokens;

  queue<nodo*> donantes;

  priority_queue<nodo*,vector<nodo*>, Compara> heap;

  double anterior, actual;

  double Thao;
  double Tasa;

  double numNodosHeap;
  double sumTServicio;
  double numTServicio;
  double sumSizeHeap;
  double Delta;
  int frec;

public:
  Amortiguador( double _thao ) 
  {
     Thao   = _thao;
     frec   = 0; 

     Delta    = 5.0; //intervalo que mide la tasa de la consulta

     anterior = 0.0;
     actual   = 0.0;
     numNodosHeap = 10.0;
     sumTServicio = 0.0;
     sumSizeHeap = 0.0;
  }

  void ActualizaCostos( double DeltaT ); 

  void setNodosHeap( int n ) { numNodosHeap= n; } 

  NodeEntry* getDonante( NodeEntry* idSolicitante, double Reloj );
};

#endif
