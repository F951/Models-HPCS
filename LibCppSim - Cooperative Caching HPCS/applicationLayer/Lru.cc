#include "../auxiliar/Util.h"
#include "Lru.h"


Entry* LRU::check(BIGNUM* hashValue)
{	
   if(hit(hashValue))
   {
      map<BIGNUM*,Entry*>::iterator iter;
         
      for(iter = cache.begin(); iter != cache.end(); iter ++)
      {
         BIGNUM* tmp= iter->first;
         if(BN_cmp(tmp,hashValue) == 0)
	 {
//	    update(tmp);
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

void LRU::remove(Entry* e)
{
    freeCache += e->size;
    map<BIGNUM*,Entry*>::iterator it;
    it=cache.begin();

    //set<BIGNUM *> elimina;

    while(it!=cache.end())
    { 
       BIGNUM* b = it->first;
       BIGNUM* b1 = e->hashValue;
   
       if( BN_cmp(b1, b)==0)
       {
         //  BIGNUM * tmp= it->first;
	  // elimina.insert(tmp);
           cache.erase(it->first);
	 //  delete tmp;
	   break;
       }
      // else
      // {
          it++;
       //}
    }
   // fori (set<BIGNUM *>::iterator it=elimina.begin(); it!= elimina.end();it++)
   // {
   //    BIGNUM * tmp=*it;
    //   cache.erase(*it);
    //   delete tmp;
  //  }  
  // delete e;
}


bool LRU::hit( BIGNUM* hashValue )
{
   if( cache.empty( ) )
      return false;

   if( LRU::count( hashValue  ) == 0 )
      return false;
   else
      return true;
}

int LRU::count(BIGNUM* b)
{
   int count=0;
   map<BIGNUM*,Entry*>::iterator it;
   it=cache.begin();

   while(it!=cache.end())
   {
      if(BN_cmp(b, it->first)==0)
      {
         count++;
	 return count;
      }
      it++;
   }
   return count;
}
 

void LRU::insertEntry(Entry *e)
{
   if (LRU::count(e->hashValue)==0)
   {   
      if(!this->isFreeSpace (e->size))
      {
         this->createSpace(e->size);
      }

      ptr = e; 
      pq.push(ptr);
      freeCache -= e->size;
   //  cout << "debug 3 " << endl;
   //   cache[e->hashValue] = ptr;
       cache.insert(pair<BIGNUM*,Entry*>(e->hashValue, e));
   }
   else
   {
      remove(e);
      cache.insert(pair<BIGNUM*,Entry*>(e->hashValue,e));
   }
   
}


//---------------------------------------------------

//void LRU::insert( string key, BIGNUM* hashValue, int size, long int timestamp )

//void LRU::insert(Entry* e)
//{
//   if ( cache.count( e->hashValue )!= 0 )
//   {   
//      return;  
//   } 
   //verificar que no este
//   ASSERT( cache.count( e->hashValue ) == 0 );

//   if( !this->isFreeSpace( e->size ) )
//   {
//      this->createSpace( e->size );
//   }

   //put in cache a new Entry
   //ptr = new Entry( timestamp , key, hashValue, size );
//   pq.push( e );
  
//   freeCache -= e->size;

   //enlazar
  // cache[e->hashValue] = ptr;

//}

//---------------------------------------------------
// TODO: Ask why is this necessary
//void LRU::update( string key )
//{
  //marcar la entrada
//  cache[key]->flag = false;
//  int size = cache[key]->size;

  //agregar una nueva
//  ptr = new Entry( timestamps , key, size );
//  pq.push( ptr );

  //datos
//  cache[key] = ptr;

  //aumentar timestamp
//  timestamps++;
//}

//---------------------------------------------------

void LRU::updateFrequency(BIGNUM* hashValue)
{
    map<BIGNUM*,Entry*>::iterator it;
    it=cache.begin();

    while(it!=cache.end())
    {
	if(BN_cmp(hashValue, it->first)==0)
	{
            (it->second)->updateFrequency();
   //cache[hashValue] -> updateFrequency();
        }
	it++;
    }
}



void LRU::update( BIGNUM* hashValue )
{
 
   
   //updateFrequency(hashValue);   
   //it=cache.find(hashValue);
   
   //if (it!=cache.end())
   //{
    map<BIGNUM*,Entry*>::iterator it;
    it=cache.begin();

    while(it!=cache.end())
    {
       if(BN_cmp(hashValue, it->first)==0)
       {
          (it->second)->update(timestamps);
	   (it->second)->updateFrequency();
       }
       it++;
    }
   
   timestamps++;
}

bool LRU::isFreeSpace( int size )
{
   if( freeCache >= size )
      return true;
   return false;
}

//---------------------------------------------------

void LRU::createSpace( int size )
{
  do
   {
     ptr = pq.top();
     pq.pop();
     freeCache += ptr->size;
     cache.erase( ptr->hashValue );
     delete ptr ;
  }  while ( freeCache < size );
}

void LRU::reset()
{
   map<BIGNUM*,Entry*>::iterator it;
   it=cache.begin();

   while(it!=cache.end())
   {
      (it->second)->reset();
      it++;
   }
}
