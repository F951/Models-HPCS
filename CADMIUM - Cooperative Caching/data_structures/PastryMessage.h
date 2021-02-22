#ifndef _PASTRYMESSAGE_h
#define _PASTRYMESSAGE_h

#include "../glob.h"
#include "NodeEntry.h"
#include "../transportLayer/MessageT.h"

// Add here different types of PastryMessage
enum PastryMessageType { PASTRY_SENDDIRECT = 0 ,
                         PASTRY_SENDDHT = 1 ,  
			 PASTRY_RECEIVE = 2 , 
			 PASTRY_ROUTE = 3 ,
			 PASTRY_BDC = 4 ,
			 PASTRY_BDC_RECEIVE = 5
			 };

// PastryMessage class ancapsulates the messages used in Pastry for
// comunication
class PastryMessage {

public:

   PastryMessageType type;
   NodeEntry *srcNode;
   BIGNUM *dest;
   NodeEntry *destNode;
   int hops;
   int finalDestination;
	
   /* DATA stores de upperLayer message */
   void *DATA;

   // Keep a trace of the visited nodes during routing
   vector <BIGNUM*> tracks;

   // Adds a new node Id on the track
   void addTrack(BIGNUM* bn)
   {
      tracks.push_back(bn);
   }

   ~PastryMessage()
   {
   // free(srcNode);
   // free(dest);
  //  free(destNode);
  //  free(DATA);
    tracks.clear();

   }
   
	//Constructor
	PastryMessage(PastryMessageType _type, void *_DATA)
	{
		type=_type;
		srcNode=NULL;
		dest=NULL;
		DATA=_DATA;
		destNode=NULL;
	}
   
	//Constructor
	PastryMessage(PastryMessageType _type, NodeEntry *_src, BIGNUM * _dest, void *_DATA)
	{	
		type=_type;
		srcNode=_src;
		dest=_dest;
		hops=0;
		DATA=_DATA;
		finalDestination=0;
		destNode=NULL;
	}
	
	// Constructor
	PastryMessage(PastryMessageType _type, NodeEntry *_src, NodeEntry *_destNode, void *_DATA)
	{
		type=_type;
		srcNode=_src;
		destNode=_destNode;
		hops=0;
		DATA=_DATA;
	}

	// Returns the data to be casted as a message of the upperLayer
	void * getData()
	{
		return DATA;
	}

   // Returns the type of the Pastry Message
   int getType()
   {
      return type;
   }

   // Returns the message destination node
   NodeEntry* getDestEntry()
   {
      return destNode;
   }

   // Sets the message originator node
   void setDestEntry(NodeEntry* _ne)
   {
      srcNode= _ne;
   }	

   // Returns the message originator node
   NodeEntry* getSrcEntry()
   {
      return srcNode;
   }

   // Returns PastryMessage destnation
   BIGNUM* getDest()
   {	
      return dest;
   }	

   // Sets the type of the Pastry Message
   void setType(PastryMessageType _t)
   {
      type=_t;
   }	

};

#endif
