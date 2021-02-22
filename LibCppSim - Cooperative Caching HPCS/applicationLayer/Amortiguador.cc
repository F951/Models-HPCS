#include "Amortiguador.h"

NodeEntry* Amortiguador::getDonante( NodeEntry* idSolicitante, double Reloj )
{
    
    anterior = actual;
    actual   = Reloj; 

    if ( actual-anterior > Delta )
       frec = 1;
    else 
       frec++;
    Tasa = (double)frec/Delta;

    nodo *nd, *ndd;
  
    NodeEntry *id = idSolicitante;

    double Tsalida = 0.0;
    double costo   = 0.0;
    double factor  = 1.2;

    int numNodos   = int(donantes.size()+heap.size());


    if ( numNodos == 0 ) 
    {  /* no hay ningun nodo insertado previamente */
       ndd = new nodo (id, Thao);
       ndd->Tsalida = Reloj + Thao;
       ndd->costo   = Thao;
       ndd->arc     = 0;
       heap.push(ndd);
       return NULL; // no hay donantes
    }

    int numOptimo  = int(factor*Tasa*Thao);
    if ( numOptimo < 1 ) numOptimo = 1;

    while ( !heap.empty() )
    {
       nd = heap.top();

       double dist = nd->Tsalida - Reloj;

       if (dist <=0)
       {
          donantes.push( nd );
          heap.pop();
       }
       else
         break;
    }

    if ( donantes.size() > 0 )
    {
       nd = donantes.front();
       donantes.pop();
       Tsalida = Reloj + Thao;
    }
    else if ( heap.size() > 0 )
    {
       nd = heap.top();
       heap.pop();
       Tsalida = nd->Tsalida + Thao;
    }

    NodeEntry* idSalida = nd->id;

    costo        = Tsalida -  Reloj;

    nd->Tsalida  = Tsalida;
    nd->costo    = costo;
    nd->arc++;

    ndd = new nodo (id, Thao);
    ndd->Tsalida = Tsalida;
    ndd->costo   = costo;
    ndd->arc     = 0;

    if ( numNodos > numOptimo+2 )
    {
       delete nd;
       delete ndd;
       return idSalida;
    }
   
    if ( donantes.size() == 0 )
    {
       heap.push(ndd);
       heap.push(nd);

       return idSalida;
    }

    if ( numNodos < numOptimo )
       heap.push(ndd);
    else delete ndd;

    if ( numNodos+1 < numOptimo )
       heap.push(nd);
    else delete nd;
/*
    sumSizeHeap += (actual-anterior)*int(heap.size()+donantes.size());
    sumTServicio += costo;
*/
    return idSalida;    

}

void Amortiguador::ActualizaCostos( double DeltaT )
{
   numNodosHeap = sumTServicio / DeltaT;
   sumTServicio = 0.0;
   sumSizeHeap  = 0.0;
}
