#ifndef LFU_H
#define LFU_H

#include "../glob.h"
#include "FreqEntry.h"

/**
 * Esta clase implementa una politica de reemplazo LFU. 
 *
 */

struct CompareFreqEntry: public std::binary_function<FreqEntry*,FreqEntry*,bool>
{
   bool operator() (FreqEntry* e1, FreqEntry* e2 )
   {
            return e1->priority > e2->priority;
   }
};


class LFU
{
private:

  map<BIGNUM* ,FreqEntry* > cache;  ///< Representan la cache.
  int *NC;
  int libre; 

  priority_queue< FreqEntry*, vector<FreqEntry*>, CompareFreqEntry > cola_prioridad; ///< Cola de prioridad.

  //--temporales
  FreqEntry *ptr;  
  bool impreso;

public:
  LFU( int *_nc//,
       //UTIL *_util
     )
  {
    NC   = _nc;
    libre = *_nc;
    impreso = false;
  }

  ~LFU( )
  {
    cache.clear( );
    //limpiar cola
  }

  /**
   * Recorre la cola de prioridad e imprime sus elementos
   */
  void imprimir()
  {
   while( !cola_prioridad.empty() )
   {
      ptr = cola_prioridad.top();

      cola_prioridad.pop();
      if ( ptr->flag != false )
      {
        string s = BN_bn2hex(ptr->hashValue);
	cout<< s <<endl;
      }
    }
  }

  /**
   * Retorna el valor de la variable "impreso".
   *
   * @return el valor de la variable "impreso".
   */
  bool get_impr() {return impreso;}

  /**
   * Asigna un nuevo valor a la variable "impreso".
   *
   * @param x es el valor a asignar.
   */
  void set_imp( bool x ){ impreso = x;}

  /**
   * .
   *
   * @return .
   */
  int getNC()
  {
    return *NC;
  }

  /**
   * Indica si la entrada existe en la Cache (Hit) o no.
   *
   * @param key es la entrada a consultar por Hit.
   */
  bool hit( BIGNUM* /*key*/ );

  /**
   * Inserta una entrada en la Cache.
   *
   * @param key es la entrada a insertar.
   * @param tam es el tamanio de la entrada.
   */
  void insert( FreqEntry );

  /**
   * Aumenta (o actualiza) el timestamp de la entrada en la Cache.
   *
   * @param key es la entrada a actualizar su timestamp.
   */
  void update( BIGNUM* /*key*/ );


  void remove (FreqEntry);

  /**
   * Indica si en la Cache existe el espacio libre indicado.
   *
   * @param tam es el espacio.
   * @return true si queda espacio.
   */
  bool queda_espacio( int/*tam*/ );

  /**
   * Elimina entradas hasta generar espacio suficiente.
   *
   * @param tam es la cantidad de espacio requerida.
   */
  void generar_espacio( int/*tam*/ );

  /**
   * Muestra por pantalla el tamanio de la cache (el numero de entradas).
   */
  void getSize( )
  {
    cout << "cache.size()=" << cache.size( ) << endl;
  }
};


#endif
