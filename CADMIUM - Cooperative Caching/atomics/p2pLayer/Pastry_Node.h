#ifndef _PASTRY_NODE_h
#define _PASTRY_NODE_h

#include "../glob.h"
#include "Leafset.h"
#include "NodeEntry.h"
#include "RoutingTable.h"
//~ #include "PastryProtocol.h"
#include "../../auxiliar/Util.h"
#include <sstream>
#include <string.h>
using namespace std;

class RoutingTable;
class Leafset;

class Pastry_Node{
	public:
		int ip;
		BIGNUM* nodeID;
		Leafset *leafset;
		int row = 40; // 160 bits/4 B
		int col = 16; // 2 exp B
		RoutingTable* rt;
		const char* tmp_nodeID;
		NodeEntry *ne;
		BIGNUM  *NODEID; 
		int idTransporte;

		Pastry_Node(){}
		Pastry_Node(	BIGNUM* _nodeID,
						int _ip	
							)
		{
			NODEID=_nodeID;
			idTransporte = _ip;
			//~ ne = new NodeEntry(NODEID, idTransporte);
			ne = new NodeEntry(_nodeID,_ip);
			ip = _ip;
			nodeID = _nodeID;
			leafset=new Leafset(nodeID, 16);
			rt=new RoutingTable(row, col);
		}
};
#endif 
