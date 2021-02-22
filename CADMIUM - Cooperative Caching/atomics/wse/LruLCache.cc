#include "../auxiliar/Util.h"
#include "LruLCache.h"


LCache* LRULCACHE::check(BIGNUM* hashValue)
{
   if(hit(hashValue))
   {
      map<BIGNUM*,LCache*>::iterator iter;
    
      for(iter = cache.begin(); iter != cache.end(); iter ++)	
      {
         if(BN_cmp(hashValue, iter->first) == 0)
	 {
	    update(iter->first);
	    return iter->second; 
	 }
      }
   } 
   else 
   {
      return NULL;
   }
   return NULL;
}

bool LRULCACHE::hit( BIGNUM* hashValue )
{
   if( cache.empty( ) )
      return false;

   if( LRULCACHE::count( hashValue ) == 0 )
      return false;
   else
      return true;
}


int LRULCACHE::count(BIGNUM* b)
{
   int count =0;
   map<BIGNUM*, LCache*>::iterator it;
   it=cache.begin();
   while(it!=cache.end())
   {
      if(BN_cmp(b,it->first)==0)
      {
         count ++;
      }
      it++;
   }
   return count;
}

//---------------------------------------------------
//void LRULCACHE::insert( string key, BIGNUM* hashValue, int size, NodeEntry* loc, NodeEntry* ns, long int timestamps )
//{
 //  if ( cache.count( hashValue )!= 0 )
 //  {
 //     update( hashValue );   
 //     return;  
 //  }

   //verificar que no este
//   ASSERT( cache.count( hashValue ) == 0 );

 //  if( !this->isFreeSpace( size ) )
 //  {
  //    this->createSpace( size );
  // }

   //put in cache a new LCache entry
//   ptr = new LCache( key, hashValue, loc, ns ,timestamps);
 //  pq.push( ptr );
  
 //  freeCache -= size;

   //enlazar
 //  cache[hashValue] = ptr;

//}


void LRULCACHE::insertLCache(LCache *e)
{
   if(LRULCACHE::count(e->hashValue)==0)
   {
      if(!this->isFreeSpace(e->size))
      {
         this->createSpace(e->size);
      }
   ptr =e;
   pq.push(ptr);
   freeCache-=e->size;
   cache.insert(pair<BIGNUM*, LCache*>(e->hashValue,e));
   }
}

//---------------------------------------------------

void LRULCACHE::update( BIGNUM* hashValue )
{
  //marcar la entrada
 //  cache[hashValue]->update(timestamps);
  // cache[key]->flag=false;
//  int size = cache[key]->size;

  //agregar una nueva
//  ptr = new Entry( timestamps , key, size );
//  pq.push( ptr );

  //datos
 // cache[key] = ptr;

   map<BIGNUM*,LCache*>::iterator it;
   it=cache.begin();

   while(it!=cache.end())
   {
      if(BN_cmp(hashValue, it->first)==0)
      {
         (it->second)->update(timestamps);
      }
       it++;
   }

   timestamps++;
}

//---------------------------------------------------

bool LRULCACHE::isFreeSpace( int size )
{
   if( freeCache >= size )
      return true;
   return false;
}



//---------------------------------------------------

void LRULCACHE::remove(LCache* e)
{
   freeCache +=e->size;
   map<BIGNUM*,LCache*>::iterator it;
   it=cache.begin();

   while(it!=cache.end())
   {
      if(BN_cmp(e->hashValue, it->first)==0)
      { 
         BIGNUM* tmp = it->first;
	 cache.erase(it->first);
	 delete tmp;
      }
      else{
      it++;
      }

   }
  // cache.erase(hashValue);
  // ASSERT(cache.count(hashValue)==0);

}

void LRULCACHE::createSpace( int size )
{
  do
  {
    ptr = pq.top( );
  //  while( ptr->flag == false )
   // {
     // pq.pop( );
    //  delete ptr;
    //  ptr = pq.top( );
   // }
   pq.pop( );
 freeCache +=ptr->size;
    //ifreeCache += ptr->size;
  
   cache.erase( ptr->hashValue );
   delete ptr;
  }while ( freeCache < size );
}

//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------

