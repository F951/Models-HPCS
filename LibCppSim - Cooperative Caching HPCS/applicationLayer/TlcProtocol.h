#ifndef _TLCPROTOCOL_h
#define _TLCPROTOCOL_h

#include "../p2pLayer/PastryProtocol.h"
#include "../p2pLayer/PastryNode.h"
#include "../p2pLayer/PastryMessage.h"
#include "../auxiliar/Hash.h"
#include "../auxiliar/Util.h"
#include "../Observer.h"
#include "../glob.h"
#include "Lru.h"
#include "Query.h"
#include "TlcMessage.h"
#include "Replicas.h"
#include "InLink.h"
#include "LruLCache.h"
#include "Entry.h"
#include "ClientTLCWSE.h"
#include "Replicator.h"
#include "Replicas.h"


class Replicator;

/* Main class of the application Layer */

class TlcProtocol : public PastryNode
{


public:
   //Replicator
   handle<Replicator> *replicator;
	
	int cte_trabajo;
	double CTE_trabajo;
   // To calculate Queue Size
   double avg_queue;
   float t_act;
   float t_pas;
   int iteracion;

   // Message list
   list<TlcMessage *> queue_in;

   // Messages out
   vector<Query*> waiting;
   vector<TlcMessage*> outstanding;
   vector<LCache*> lcack;
  

   // Result cache global
   LRU *RCACHE;
   int cacheSize;

   // one replica for each Replicated Entry identified by its hashValue (BIGNUM)
   //map <BIGNUM*,Replicas> replicas;
 
   //track of nodes replicas
   map <BIGNUM*, Replicas*> replicaTrack;
   
   // query TTL <queryID, TTL>
   map <BIGNUM*, int> queryTTL;
   
   // query resultID <queryID, resultsID>
   map <BIGNUM*, BIGNUM*> urlsID;
   
   // one inlink for each NODE identified by its NODEID (BIGNUM)
   map <BIGNUM*,Inlink*> inlinks;

   // Result cache local
   LRU *localCache;
   int localCacheSize;

   // this is different i think
   LRULCACHE *LCACHE;
   int locationCacheSize;

   BIGNUM* pid;
   int msgID;
   bool busy;
  
   /* handle to the lower layer */
   handle<PastryProtocol> *pastry;
   /* Implemented to make the connection to with the lower layer */
   handle<PastryNode> *H_obj;

   list<long> AQueries;
   int totalQueries;
   handle<Observer> *observer;
   handle<ClientTlcWse> *client;
   
   //Fin simulacion
   int *ends, NP;
   int terminadas, nFin;
   bool Broadcast;

   double Thao;

   int Rep_Strategy;
   int Case_Responsable;
   int overloaded;
   
   rng<double>* work_time;
   
   //Variables del replicador:
   double start_Control_time;
   double Control_TIME;
   vector <Entry*> entries;
   bool ends_replicator;
   int overload;
   
protected:
   void inner_body(void);

public:

/*
  static bool compare(Entry *e1 , Entry *e2)
  {
      if(e1->frequency < e2->frequency)
      	return true;
      else
        return false;
  }

*/

   void freeTlc()
   {
   //  localCache->deleteCache();
   //  LCACHE->deleteCache();
   //  RCACHE->deleteCache();
     //free(observer);
     //free(pastry);
     // delete replicator;
     //BN_free(pid);
     // delete client;
     //replicas.clear();
     
     replicaTrack.clear();
     queryTTL.clear();
     urlsID.clear();
     inlinks.clear();
     delete RCACHE;
   }
   TlcProtocol (const string& name,
                BIGNUM* _pid,
		int _cacheSize,
		int _localSize,
		int _lcSize,
		handle<ClientTlcWse> *_c,
		handle<Observer> *_obs,
		handle<Replicator> *_rep,
		int _tQueries,
		int *_ends,
		int _NP,
		double _Thao,
		int _RS,
		int _CS,
		int _cte_trabajo
	       ):PastryNode(name, _pid)
   {	
      Rep_Strategy =_RS;
      Case_Responsable = _CS;
      Thao = _Thao;
      ends = _ends;  
      NP   = _NP;   
      pid  = _pid;
      nFin = 0;
      msgID=0;
      busy = false;
      cacheSize = _cacheSize;
      localCacheSize = _localSize;
      locationCacheSize = _lcSize;
      RCACHE = new LRU(&cacheSize);
      localCache = new LRU(&localCacheSize);
      LCACHE = new LRULCACHE(&locationCacheSize);
      replicator = _rep;
      client = _c; 
      observer = _obs;
      avg_queue = 0;
      terminadas = 0;
      t_pas = 0;
      t_act=0;
      totalQueries = _tQueries;
      Broadcast=false;
      iteracion = 0;
      overloaded=0;
      cte_trabajo=_cte_trabajo;
      
      work_time = new rngUniform( "Work Time", 10,30 );
	  work_time->reset();
	  
	 start_Control_time=0;
	 Control_TIME=1;

      
  }

   /* Implemented to make the connection with the lower layer */
   void set_H_obj( handle<TlcProtocol> *t )
   {
      H_obj = (handle<PastryNode>*)t;
   }
   
   /* set the lower layer */
   void addLowerLayer(handle<PastryProtocol> *_pastry)
   {
      pastry = _pastry;
      handle<PastryNode> *p = H_obj;

      (*pastry)->addNode(p);
   }

   void add_message(TlcMessage *tm)
   {
      queue_in.push_back(tm);

   }

   /* receives a message from the upper layer */
   void add_query(Query *q)
   {
     
      q->setQuerySrc((*pastry)->ne);	
      TlcMessage *tm = new TlcMessage(TLC_LOOKUP, msgID, (*pastry)->ne, q->hashValue) ;
      msgID = msgID + 1 ;
      tm->setQuery(q) ;
      queue_in.push_back(tm) ;
     // cout << "Tlc: addquery-" << endl; 
//           << BN_bn2hex((*pastry)->NODEID) 
//	   << " - add_query -KEY: " 
//	   << BN_bn2hex( q->hashValue) 
//	   << endl;
   }

   /* returns the state of the node */
   bool get_busy()
   {
      return busy;
   }


   void print()
   {
      cout << "TLC "<< BN_print_fp(stdout, pid) << endl;
   }


   BIGNUM* applyHash (char* argv )
   {
      using namespace std;
      Hash* h = new Hash();
      BIGNUM* b2 = h->GenerateKey(argv);
      return b2;
   }
   
   void sendEndBroadcast(); 

   void continueRouting( TlcMessage* );
   
   void continueRouting( TlcMessage*, NodeEntry* );

   void sendLCache( LCache*, TlcMessage* );

   void sendReply( Entry*,TlcMessage* );

   void sendReplica( Entry*, NodeEntry* );

   void setAvgQueue();

   bool isReplicated(BIGNUM*, BIGNUM*);

   void trackReplica(BIGNUM*, BIGNUM*);

   void checkTTL(BIGNUM*, BIGNUM*);
   
   void freeMessage(TlcMessage*);

   void printPath(TlcMessage* tm);

   void EndReplicator(); 

   void storeInlink(BIGNUM*, NodeEntry*);
    
   void checkWaitTimeOut();
   
   bool isNewReplica(BIGNUM*);

   void TlcAlgorithm(TlcMessage *tm);

   void LeafsetBubble(TlcMessage *tm);

   void resetQueue();
   
   //---------------------------------- Funciones replicador
   //static bool compare(Entry *e1 , Entry *e2);
   //~ static bool TlcProtocol::compare(Entry *e1 , Entry *e2){
   static bool compare(Entry *e1 , Entry *e2){
	if(e1->frequency > e2->frequency)
		return true;
	else
		return false;
	}
	struct sorter
   {
      BIGNUM* key;
      sorter(BIGNUM* k) : key(k){}
      bool operator() (Inlink *i1, Inlink *i2)
      {
         if(i1->getFrequency(key) > i2 ->getFrequency(key))
            return true;
         else
            return false;
      }
   };
	
void load_control();		//Ex inner body
double getQueueSize();
bool isOverloaded();
int getAvailableCapacity();
void retrieveEntries();
void replicate(int strategy);
void replicateLeafset();
void replicateBubble();

   
   


};


#endif
