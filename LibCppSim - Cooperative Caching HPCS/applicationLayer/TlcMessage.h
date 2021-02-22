#ifndef _TlcMessage_h
#define _TlcMessage_h

#include "../p2pLayer/PastryMessage.h"
#include "LCache.h"
#include "../glob.h"
#include "Entry.h"

/* Message that are exchanged in the application Level */

/* Add here different types of TlcMessages */
enum TlcMessageType 
{ 
   TLC_LOOKUP = 0 ,
   TLC_REPLY = 1 ,
   TLC_REPLICATE = 2 , 
   TLC_LCACHE = 3 , 
   TLC_WSE = 4 ,
   TLC_ENDS = 5,
   TLC_LC_ACK = 6
};

class TlcMessage 
{

public:

   int id;

   // Optional depending on the message type
   Query *query;
   Entry *entry;
   LCache *lc;
   
   // type of the message
   TlcMessageType type;
   
   // Destination and src peers
   NodeEntry* src;
   NodeEntry* destNode;
   BIGNUM* dest;
   
   // it's true when the message has arrived to the responsible peer
   // helps in TlcProtocol 
   bool final_destination;

   // hops and tracks
   vector<NodeEntry*> path;

   // path of the message
   // should this be the same as pastry list?


 
   ~TlcMessage()
   {
 //     free(query);
 //     delete entry;
//      free(lc);
  //    free(src);
 //     free(destNode);
 //     free(dest);
      path.clear();
   }

   void addPath(NodeEntry* ne)
   {
      path.push_back(ne);
   }

void setPath(vector<NodeEntry*> oldp)
{
     path = oldp;
}

   int getPathSize()
   {
      return path.size();
   }

   TlcMessage(TlcMessageType _tmt, int _msgID)
   {
      type = _tmt;
      id = _msgID;
      src = 0;
      dest = 0;
   }

   TlcMessage(TlcMessageType _tmt, int _msgID, Entry *_e)
   {
    type=_tmt;
    id=_msgID;
    entry=_e;
  
   }

   TlcMessage(TlcMessageType _tmt, int _msgID, NodeEntry* _src, BIGNUM* _dest)
   {
      type=_tmt;
      id=_msgID;
      src = _src;
      dest=_dest;
      final_destination=false;
   }

   TlcMessage(TlcMessageType _tmt, int _msgID, NodeEntry* _src, NodeEntry* _dest)
   {
      type=_tmt;
      id=_msgID;
      src=_src;
      destNode=_dest;
      final_destination=false;
    
   }

//TlcMesisage clone()
//{
//	TlcMessage newTlc = new TlcMessage(this.type, this.id, this.src, this.dest);
//	hops= this.hops;
//	final_destination = this.final_destination;
//}

   /* returns the query within the message */


   bool getForceRouting()
   { 
      return query->getForceRouting();
   }

   void setFinalDest()
   {
      final_destination=true;
   }

   bool getFinalDest()
   {
      return final_destination;
   }

   void setQuery(Query *_q)
   {
      query= _q;
   }

   void setEntry(Entry *_e)
   {
      entry = _e;
   }

   void setLCache(LCache *_lc)
   {
      lc = _lc;
   }

   Query * getQuery()
   {
      return query;
   }

   Entry * getEntry()
   {
      return entry;
   }

   LCache * getLCache()
   {
      return lc;
   }

   /* returns the type of the message */
   int getType()
   {
      return type;
   }

   /* set the type of the message */
   void setType(TlcMessageType _type)
   {
      type=_type;
   }

   
};
#endif
