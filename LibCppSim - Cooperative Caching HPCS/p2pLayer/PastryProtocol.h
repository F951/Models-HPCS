#ifndef _PASTRYPROTOCOL_h
#define _PASTRYPROTOCOL_h

#include "../transportLayer/redtubo.h"
#include "../glob.h"
#include "PastryMessage.h"
#include "PastryNode.h"
#include "NodeEntry.h"
#include "Leafset.h"
#include "RoutingTable.h"

/* MAIN CLASS of the Pastry Protocol 
 * PastryProtocol
 * IMPORTANT: PastryNode is the upperLayer Node (implemented by TLC)*/

class PastryProtocol: public NodeT
{

public:

   const char* tmp_nodeID;

static bool compare ( handle<PastryProtocol>* l, handle<PastryProtocol>* r )
{
   int temp;
   temp = BN_cmp((*l)->NODEID,(*r)->NODEID);
   if (temp < 0)
      return true;
   else 
      return false;
}

   bool busy;

   /* receives the messages */
   list<PastryMessage*> queue_in;
   /* handle towards the lower layer */
   handle<RedTubo> *transport;
   /* handle towards the upper layer */
   handle<PastryNode> *upperLayer;

   PastryMessage *m;

   BIGNUM  *NODEID; 
	
   int idTransporte;

   NodeEntry *ne;

   Leafset *leafset;

   RoutingTable *rt;

   /* This was included to make a conection with the lower layer 
    * H_obj is a pointer to the object */
   handle<NodeT> *H_obj; 

   void freePastryPeers()
   { 
     delete ne;
     delete leafset;
     delete rt;
    // BN_free(NODEID);
   }


   /* Constructor */
PastryProtocol( const string& name, 
                int _nid, BIGNUM *_id, 
		int leafsetSize ): 
		NodeT(name,_nid)
{
        	
   //cout << "Pastry init0" << endl;
   NODEID=_id;
   idTransporte = _nid;
   ne = new NodeEntry(NODEID, idTransporte);
   leafset= new Leafset(_id, leafsetSize);

   int row = 40; // 160 bits/4 B
   int col = 16; // 2 exp B

   //cout << "New Routing table" << endl;
   rt = new RoutingTable(row, col);
   busy = false;  
}
 // 2 exp B
/* This was included to make a connection with the lower layer 
 * sets the object to itself */
void set_H_obj( handle<PastryProtocol> *t ) 
{
   //cout << "SetHandleObject PAstry" << endl;
   H_obj = (handle<NodeT>*)t;
}

/* Adds the upper layer Pastry Node */ 
void addNode( handle<PastryNode> *p )
{
   upperLayer=p;
}

/* Adds the lower layer */
void addLowerLayer(handle<RedTubo> * t)
{
   transport=t;
   handle<NodeT> *p = H_obj;
   (*transport)->add_Node(p);
}

/* delivers a message to the upper layer */
void deliver(MessageT *m)
{
   PastryMessage *pm = (PastryMessage*)m->getData();
//   delete m;
   
   pm->setType(PASTRY_RECEIVE);
   queue_in.push_back(pm);
   
}

/* adds a message in queue_in */
void add_message(PastryMessage *pm)
{
  // cout << "PASTRY pmTipo: " << pm->getType() << endl;
   queue_in.push_back(pm);
}

/* return the state of the node */
bool get_busy()
{
   return busy;
}

void print()
{
   cout << "PASTRY " << BN_print_fp(stdout, NODEID) << endl;
}


/* gets NODEID */
BIGNUM* getNodeID()
{
   return NODEID;
}


void inner_body( void );

NodeEntry* route( BIGNUM* pm );

//NodeEntry* route( BIGNUM* bn );

NodeEntry* checkClosest(BIGNUM*, NodeEntry*, NodeEntry*);

};

#endif
