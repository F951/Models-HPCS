/* Implementa cache con politica LRU. */
#ifndef LRUA_h
#define LRUA_h

#include "../glob.h"
#include "Answer.h"
#include "../auxiliar/Hash.h"


struct CompareAnswers: public std::binary_function<Answer*,Answer*,bool>
{

   bool operator() (Answer* e1, Answer* e2 )
   {
      return e1->priority > e2->priority;
   }
};

/* Class that implements a cache LRU */

class LRUA
{

private:
   priority_queue <Answer*, vector<Answer*>, CompareAnswers> pq;

   // Cache is form with entries
   list<Answer*> cache;
   int *cacheSize;
   int freeCache;

   int timestamps;

   Hash* h; 

   //--temporales
   Answer *ptr;
   unsigned int prio;
   //vector<string> v;
   //vector<string>::iterator itv;

public:

   LRUA( int *_cacheSize )
   {
      cacheSize   = _cacheSize;
      timestamps   = 0;
      freeCache = *_cacheSize;
      h= new Hash();
   }

   ~LRUA( )
   {

       //pq.clear();
       list<Answer*>::iterator it;

       it = cache.begin();
      
      while(cache.size()>0)
      {
         it=cache.begin();
	 if(it!=cache.end())
	 {
            Answer *temp= (*it);
            cache.erase(it);
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
  // int getMaxCacheSize()
 //  {
 //     return *cacheSize;
 //  }
  // bool hit( BIGNUM* /*key*/ );
   //void insert( string /*key*/, BIGNUM*,  int/*size*/, long int );
  // void insert (Entry);
   void insertAnswer(Answer*);
  // void updateFrequency(BIGNUM*);
  // void update( BIGNUM* /*key*/ );
   bool isFreeSpace( int/*size*/ );
   void createSpace( int/*size*/ );
   void remove(Answer*);
  // int count(BIGNUM*); 
  
   Answer* check(string);
   //Answer* check(BIGNUM*);
   void cacheErase(Answer*);
   void getSize( )
   {
      cout << "LruA:cache.size()=" << cache.size( ) << endl;
   }
};
#endif
