#include "../auxiliar/Util.h"
#include "Lfu.h"

bool LFU::hit( BIGNUM* hashValue )
{
  if( cache.empty( ) )
    return false;

  if( cache.count( hashValue ) == 0 )
    return false;
  else
    return true;
}


//---------------------------------------------------
void LFU::insert( FreqEntry e )
{
  if ( cache.count( e.hashValue )!= 0 )
  {
   
     //verificar que no este
     ASSERT( cache.count( e.hashValue ) == 0 );
   
     if( !this->queda_espacio( e.size ) )
     {
       this->generar_espacio( e.size );
     }  

      //poner en cache nuevo nodo
      ptr = &e;
      cola_prioridad.push( ptr );
  
      libre -= e.size;

      //enlazar
      cache[e.hashValue] = ptr;
   }
}


void LFU::remove(FreqEntry e)
{
  freeCache+= e.size;
  cache.erase(e.hashValue);

}


//---------------------------------------------------

void LFU::update( BIGNUM* hashValue )
{
  cache[hashValue] -> update();

}
//---------------------------------------------------

bool LFU::queda_espacio( int tam )
{
  if( libre >= tam )
    return true;
  return false;
}

//---------------------------------------------------

void LFU::generar_espacio( int size )
{
  do
  {
    ptr = cola_prioridad.top( );
    while( ptr->flag == false )
    {
      cola_prioridad.pop( );
      delete ptr;
      ptr = cola_prioridad.top( );
    }
    cola_prioridad.pop( );
 
    libre += ptr->size;
  
    cache.erase( ptr->hashValue );
 
    delete ptr;
  }while ( libre < size );
}

//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------
