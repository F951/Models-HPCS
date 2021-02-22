#ifndef PASTRY_MESSAGE_HPP
#define PASTRY_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>
#include "../glob.h"
#include "../atomics/p2pLayer/NodeEntry.h"
#include "../data_structures/TLCmessage.hpp"

// Add here different types of PastryMessage
enum PastryMessageType { 	PASTRY_SENDDIRECT = 0 ,
							PASTRY_SENDDHT = 1 ,  
							PASTRY_RECEIVE = 2 , 
							PASTRY_ROUTE = 3 ,
							PASTRY_BDC = 4 ,
							PASTRY_BDC_RECEIVE = 5
						 };

using namespace std;

/*******************************************/
/**************** PastryMessage ****************/
/*******************************************/
class PastryMessage{
public:
	PastryMessageType type;
	NodeEntry *srcNode;
	BIGNUM *dest;
	NodeEntry *destNode;
	int src_Transport;
	int dest_Transport;
	int hops;
	int finalDestination;
	int id_tarea;
	//~ void *DATA;		/* DATA stores the upperLayer message */
	TLCMessage DATA;		/* DATA stores the upperLayer message */
	vector <BIGNUM*> tracks;	// Keep a trace of the visited nodes during routing
	double time_aux;
	TLCMessage *temp;
	
	PastryMessage(){}	
	
	//Constructor
	//PastryMessage(PastryMessageType _type, void *_DATA)
	//{
	//	type=_type;
	//	srcNode=NULL;
	//	dest=NULL;
	//	DATA=_DATA;
	//	destNode=NULL;
	//}
   
	//Constructor
	//PastryMessage(PastryMessageType _type, NodeEntry *_src, BIGNUM * _dest, void *_DATA)
	//{	
	//	type=_type;
	//	srcNode=_src;
	//	dest=_dest;
	//	hops=0;
	//	DATA=_DATA;
	//	finalDestination=0;
	//	destNode=NULL;
	//	temp =(TLCMessage*) DATA;
	//	cout << "Constructor msg TLC - temp->type" << temp->type << endl;
	//	cout << "Constructor msg TLC - temp->src->getIP():" << temp->src->getIP() << endl;
	//	cout << "Constructor msg TLC - temp->src->getNodeID():" << BN_bn2dec(temp->src->getNodeID()) << endl;
	//
	//}
	
	// Constructor
	//~ PastryMessage(PastryMessageType _type, NodeEntry *_src, NodeEntry *_destNode, void *_DATA)
	PastryMessage(PastryMessageType _type, NodeEntry *_src, NodeEntry *_destNode, TLCMessage _DATA)
	{
		type=_type;
		srcNode=_src;
		destNode=_destNode;
		hops=0;
		DATA=_DATA;
		time_aux=0;
	}
	
	//~ void clone_PM(PastryMessage m_in){
		//~ type= m_in.type;
		//~ srcNode=m_in.srcNode;
		//~ destNode=m_in.destNode;
		//~ hops=m_in.hops;
		//~ DATA=m_in.DATA;
		//~ time_aux=m_in.time_aux;
		//~ src_Transport=m_in.src_Transport;
		//~ dest_Transport=m_in.dest_Transport;
		//~ finalDestination=m_in.finalDestination;
	//~ }

	//~ void operator =() 
	//~ { 
		//~ count = count+1; 
	//~ }
	
	void addTrack(BIGNUM* bn){		   // Adds a new node Id on the track
		tracks.push_back(bn);
	}
	
	//~ void * getData(){					// Returns the data to be casted as a message of the upperLayer
	TLCMessage getData(){					// Returns the data to be casted as a message of the upperLayer
		return DATA;
	}
	
	int getType(){						// Returns the type of the Pastry Message
		return type;
	}
	
	NodeEntry* getDestEntry(){			// Returns the message destination node
		return destNode;
	}
	
	void setDestEntry_IP(int _ip){	// Sets the message originator node
		destNode->ip= _ip;
	}	
	
	void setDestEntry_nodeID(BIGNUM* _nodeID){	// Sets the message originator node
		destNode->nodeID= _nodeID;
	}	
	
	void setDestEntry(NodeEntry* _ne){	// Sets the message originator node
		//~ srcNode= _ne;
		destNode= _ne;
	}	

	NodeEntry* getSrcEntry(){			// Returns the message originator node
		return srcNode;
	}

	BIGNUM* getDest(){					// Returns PastryMessage destnation
		return dest;
	}	

	void setType(PastryMessageType _t){	// Sets the type of the Pastry Message
		type=_t;
	}	

};

istream& operator>> (istream& is, PastryMessage& msg);

ostream& operator<<(ostream& os, const PastryMessage& msg);


#endif // PASTRY_MESSAGE_HPP
