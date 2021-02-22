#ifndef TLCMessage_MESSAGE_HPP
#define TLCMessage_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>
#include "../glob.h"
#include "../atomics/p2pLayer/NodeEntry.h"
#include "../atomics/applicationLayer/LCache.h"
#include "../atomics/applicationLayer/Entry.h"
#include "../atomics/applicationLayer/Query.h"

using namespace std;

enum TlcMessageType 		/* Add here different types of TlcMessages */
{ 
	TLC_LOOKUP = 0 ,
	TLC_REPLY = 1 ,
	TLC_REPLICATE = 2 , 
	TLC_LCACHE = 3 , 
	TLC_WSE = 4 ,
	TLC_ENDS = 5,
	TLC_LC_ACK = 6
};

/*******************************************/
/**************** TLCMessage ****************/
/*******************************************/
class TLCMessage{
public:
	int id;
	int id_tarea;
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
	bool final_destination;			// it's true when the message has arrived to the responsible peer
	// hops and tracks
	vector<NodeEntry*> path;
	int queueSize;

	TLCMessage(){}

	//Constructor
	TLCMessage(TlcMessageType _tmt, int _msgID){
		type = _tmt;
		id = _msgID;
		src = 0;
		dest = 0;
		queueSize=0;
	}

	//Constructor
	TLCMessage(TlcMessageType _tmt, int _msgID, Entry *_e){
		type=_tmt;
		id=_msgID;
		entry=_e;
		queueSize=0;
	}

	//Constructor
	TLCMessage(TlcMessageType _tmt, int _msgID, NodeEntry* _src, BIGNUM* _dest){		//Uso este en el tag0 del client para enviar las queries a la red p2p.
		type=_tmt;
		id=_msgID;
		src = _src;
		dest=_dest;
		final_destination=false;
		queueSize=0;
	}

	//Constructor
	TLCMessage(TlcMessageType _tmt, int _msgID, NodeEntry* _src, NodeEntry* _dest){
		type=_tmt;
		id=_msgID;
		src=_src;
		destNode=_dest;
		final_destination=false;
		queueSize=0;
	}
		
	void addPath(NodeEntry* ne)	{
		path.push_back(ne);
	}

	void setPath(vector<NodeEntry*> oldp){
		path = oldp;
	}

	int getPathSize(){
		return path.size();
	}
	
	bool getForceRouting()	{ 
		return query->getForceRouting();
	}

	void setFinalDest()	{
		final_destination=true;
	}

	bool getFinalDest()	{
		return final_destination;
	}

	void setQuery(Query *_q){
		query= _q;
	}

	void setEntry(Entry *_e){
		entry = _e;
	}

	void setLCache(LCache *_lc)	{
		lc = _lc;
	}

	Query * getQuery()	{
		return query;
	}

	Entry * getEntry()	{
		return entry;
	}

	LCache * getLCache(){
		return lc;
	}

	int getType(){	/* returns the type of the message */
		return type;
	}

	void setType(TlcMessageType _type){	/* set the type of the message */
		type=_type;
	}	
	
	TLCMessage copy(TLCMessage m_in){
		TLCMessage m_out;
		m_out.type=m_in.type;
		m_out.id=m_in.id;
		m_out.query=m_in.query;
		m_out.entry=m_in.entry;
		m_out.lc=m_in.lc;
		m_out.src=m_in.src;
		m_out.destNode=m_in.destNode;
		m_out.dest=m_in.dest;
		m_out.final_destination=m_in.final_destination;
		m_out.path.clear();
		m_out.path=m_in.path;
		m_out.queueSize=m_in.queueSize;
		m_out.id_tarea=m_in.id_tarea;
		
		return m_out;
	}
	void print_msg_TLC(){
		cout << "PRINT TLC-msg. id: " << id;
		cout << ". id_tarea: " << id_tarea;
		cout << ". type: " << type;
		cout << ". queueSize: " << queueSize;
		cout << ". final_destination: " << final_destination << endl;			// it's true when the message has arrived to the responsible peer
		//Los punteros se imprimen. Si hay violación de segmento, se les asigna NULL.
		//~ BIGNUM* dest;
		//~ NodeEntry* src;
		//~ NodeEntry* destNode;
		//~ query->printQuery();
		//~ entry->print();
		
	}

	
};



istream& operator>> (istream& is, TLCMessage& msg);

ostream& operator<<(ostream& os, const TLCMessage& msg);


#endif // TLCMessage_MESSAGE_HPP
