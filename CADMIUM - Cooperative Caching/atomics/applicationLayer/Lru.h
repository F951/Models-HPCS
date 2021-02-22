/* Implementa cache con politica LRU. */
#ifndef LRU_h
#define LRU_h

#include "../../glob.h"
#include "Entry.h"
#include "../../auxiliar/Hash.h"


struct CompareEntry: public std::binary_function<Entry*,Entry*,bool>
{

   bool operator() (Entry* e1, Entry* e2 )
   {
      return e1->priority > e2->priority;
   }
};

/* Class that implements a cache LRU */

class LRU
{

private:
   priority_queue <Entry*, vector<Entry*>, CompareEntry> pq;

   // Cache is form with entries
   map<BIGNUM* /*key*/,Entry* /*ptr*/> cache;
   int *cacheSize;
   int freeCache;

   int timestamps;

   Hash* h; 

   //--temporales
   Entry *ptr;
   unsigned int prio;
   //vector<string> v;
   //vector<string>::iterator itv;

public:

   LRU( int *_cacheSize )
   {
      cacheSize   = _cacheSize;
      timestamps   = 0;
      freeCache = *_cacheSize;
      h= new Hash();
   }

   ~LRU( )
   {

       //pq.clear();
       map<BIGNUM*, Entry*>::iterator it;

       it = cache.begin();
      
      while(cache.size()>0)
      {
         it=cache.begin();
	 if(it!=cache.end())
	 {
            Entry *temp= it->second;
            cache.erase(temp->hashValue);
     	     delete temp;
         }
	 it++;
  //    cout << "DEBUG CS : " << cache.size() << endl;
    // // cache.clear( );
     }

   }
  // void deleteCache()
  // {
  //    cache.clear();
  // }
  
   // Returns the max number of cache entries
   int getMaxCacheSize()
   {
      return *cacheSize;
   }
   bool hit( BIGNUM* /*key*/ );
   //void insert( string /*key*/, BIGNUM*,  int/*size*/, long int );
  // void insert (Entry);
   void insertEntry(Entry*);
   void updateFrequency(BIGNUM*);
   void update( BIGNUM* /*key*/ );
   bool isFreeSpace( int/*size*/ );
   void createSpace( int/*size*/ );
   void remove(Entry*);
   int count(BIGNUM*); 
   void reset();
  
   vector <Entry*> getEntries()
   {
      vector <Entry*> v;
      map<BIGNUM*, Entry*>::iterator it;
      
      it = cache.begin();
      while( it!= cache.end())
      {
         v.push_back(it->second);
         it++;
      }
      
      return v;  
   }
  
  Entry* check(BIGNUM*);
   void getSize( )
   {
      cout << "Lru:cache.size()=" << cache.size( ) << endl;
   }
};
#endif
