/* Implementa cache con politica LRU. */
#ifndef LRULCACHE_h
#define LRULCACHE_h

#include "../glob.h"
#include "LCache.h"
#include "../p2pLayer/NodeEntry.h"
#include "Entry.h"

struct CompareLCache: public std::binary_function<LCache*,LCache*,bool>{

bool operator() (LCache* lc1, LCache* lc2 ){
return lc1->priority > lc2->priority;
}
};

/* Class that implements a cache LRU */

class LRULCACHE
{

protected:
   priority_queue <LCache*, vector<LCache*>, CompareLCache> pq;

   // Cache is form with entries
   map<BIGNUM* /*key*/,LCache* /*ptr*/> cache;
   int *cacheSize;
   int freeCache;

   //--temporales
   LCache *ptr;
   unsigned int prio;
   //vector<string> v;
   //vector<string>::iterator itv;

public:

   int timestamps;

   LRULCACHE( int *_cacheSize )
   {
      cacheSize   = _cacheSize;
      timestamps   = 0;
      freeCache = *_cacheSize;
   }

   ~LRULCACHE( )
   {
      map<BIGNUM*, LCache*>::iterator it;
      it= cache.begin();

      while(cache.size()>0)
      {
          it=cache.begin();
	  if(it!=cache.end())
	  {
	     LCache *temp = it->second;
	     cache.erase(it->first);
	     delete temp;
	  }
	  it++;

      }
   }

  //void deleteCache()
  //{
  //   cache.clear();
  //}
   // Returns the max number of cache entries
   int getMaxCacheSize()
   {
      return *cacheSize;
   }
   
   bool hit( BIGNUM* /*key*/ );
  // void insert(string,  BIGNUM* /*key*/, int/*size*/, NodeEntry*, NodeEntry*, long int );
  
   void insertLCache(LCache*);

   void update( BIGNUM* /*key*/ );
   bool isFreeSpace( int/*size*/ );
   void createSpace( int/*size*/ );
   void remove(LCache*);
   int count(BIGNUM* );
   void getSize( )
   {
      cout << "LruLCache: cache.size()=" << cache.size( ) << endl;
   }

   LCache* check(BIGNUM* /* key*/ );

};
#endif
