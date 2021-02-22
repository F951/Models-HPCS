#include "../auxiliar/Util.h"
#include "LruA.h"


Answer* LRUA::check(string hashValue)
{
      list<Answer*>::iterator iter;
      for(iter = cache.begin(); iter != cache.end(); iter ++)
      {
         string tmp= (*iter)->getKey();
	   if(tmp.compare(hashValue) == 0)
	   {
	      (*iter)->update(timestamps);
	      return (*iter);
           }
      }
   return NULL;
}

void LRUA::cacheErase(Answer *a)
{
   list<Answer*>::iterator iter;
   for(iter = cache.begin(); iter != cache.end(); iter ++)
   {
      string tmp= (*iter)->getKey();
      if(tmp.compare(a->key) == 0)
      {
         cache.erase(iter);
         break;
      }
   }
}




/*
Answer* LRUA::check(BIGNUM* hashValue)
{	
   if(hit(hashValue))
   {
      map<BIGNUM*,Answer*>::iterator iter;
         
      for(iter = cache.begin(); iter != cache.end(); iter ++)
      {
         BIGNUM* tmp= iter->first;
         if(BN_cmp(tmp,hashValue) == 0)
	 {
	    update(tmp);
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

void LRUA::remove(Answer* e)
{
   freeCache += e->size;
    map<BIGNUM*,Answer*>::iterator it;
    it=cache.begin();

    while(it!=cache.end())
    {
       if(BN_cmp(e->hashValue, it->first)==0)
       {
           BIGNUM * tmp= it->first;
	   
           cache.erase(it->first);
	   delete tmp;
	   break;
       }
       else
       {
       it++;
       }
    }
  // delete e;
}


bool LRUA::hit( BIGNUM* hashValue )
{
   if( cache.empty( ) )
      return false;

   if( LRUA::count( hashValue  ) == 0 )
      return false;
   else
      return true;
}

int LRUA::count(BIGNUM* b)
{
   int count=0;
   map<BIGNUM*,Answer*>::iterator it;
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
*/ 

void LRUA::insertAnswer(Answer *a)
{
  // if (LRUA::count(a->hashValue)==0)
  // {   
      if(!this->isFreeSpace (a->size))
      {
         this->createSpace(a->size);
      }

      ptr = a; 
      pq.push(ptr);
      freeCache -= a->size;
   //  cout << "debug 3 " << endl;
   //   cache[e->hashValue] = ptr;
       cache.push_front( a);
  // } 
}

/*

void LRUA::update( BIGNUM* hashValue )
{

//   updateFrequency(hashValue);   
   //it=cache.find(hashValue);
   
   //if (it!=cache.end())
   //{
    map<BIGNUM*,Answer*>::iterator it;
    it=cache.begin();

    while(it!=cache.end())
    {
       if(BN_cmp(hashValue, it->first)==0)
       {
          (it->second)->update(timestamps);
	  break;
       }
       it++;
    }
   
   timestamps++;
}
*/
bool LRUA::isFreeSpace( int size )
{
   if( freeCache >= size )
      return true;
   return false;
}

//---------------------------------------------------

void LRUA::createSpace( int size )
{
  do
   {
     ptr = pq.top();
     pq.pop();
     freeCache += ptr->size;
     cacheErase( ptr );
     delete ptr ;
  }  while ( freeCache < size );
}

