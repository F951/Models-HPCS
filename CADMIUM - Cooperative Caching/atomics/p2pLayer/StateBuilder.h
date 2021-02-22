#ifndef _STATEBUILDER_h
#define _STATEBUILDER_h

#include "../glob.h"
#include "Leafset.h"
#include "NodeEntry.h"
#include "Pastry_Node.h"
#include "RoutingTable.h"
//~ #include "PastryProtocol.h"
#include "../../auxiliar/Util.h"
#include <sstream>
#include <string.h>
using namespace std;

class RoutingTable;
class Leafset;

//~ class Pastry_Node{
	//~ public:
		//~ int ip;
		//~ BIGNUM* nodeID;
		//~ // BN_dec2bn(nodeID, ip.c_str());
		//~ Leafset *leafset;
		//~ int row = 40; // 160 bits/4 B
		//~ int col = 16; // 2 exp B
		//~ RoutingTable* rt;
		//~ const char* tmp_nodeID;
		//~ NodeEntry *ne;
		//~ BIGNUM  *NODEID; 
		//~ int idTransporte;

		//~ Pastry_Node(){}
		//~ Pastry_Node(	BIGNUM* _nodeID,
						//~ int _ip	
							//~ )
		//~ {
			//~ NODEID=_nodeID;
			//~ idTransporte = _ip;
			//~ //ne = new NodeEntry(NODEID, idTransporte);
			//~ ne = new NodeEntry(_nodeID,_ip);
			//~ ip = _ip;
			//~ nodeID = _nodeID;
			//~ leafset=new Leafset(nodeID, 16);
			//~ rt=new RoutingTable(row, col);
		//~ }
//~ };
	
// Class used to initialize P2P network
class StateBuilder {
public:



   //~ vector<handle<PastryProtocol>*> pastryNetwork; 
   int leafsetSize;	
   
	
   vector<Pastry_Node*> Node_vector;
   


	//~ StateBuilder(vector<handle<PastryProtocol>*> ne, int _lsize)
	//~ StateBuilder(BIGNUM* _NODEID, int _ip)
	StateBuilder(vector<Pastry_Node*> & _PastryN, int _leafset_size)
	{
		Node_vector = _PastryN;
		leafsetSize = _leafset_size;
		///******* //~ pastryNetwork= ne;
		////~ leafsetSize = _lsize;
		//for (int i=0;i<(int)ne.size();i++){
		//	//~ int ip=i;
		//	//~ BIGNUM* NODEID_AUX;
		//	//~ stringstream strs;
		//	//~ strs << ip;
		//	//~ string temp_str = strs.str();
		//	//~ char* char_type = (char*) temp_str.c_str();
		//	//~ BN_dec2bn(&NODEID_AUX, char_type);
		//	//~ Node_vector.push_back(new Pastry_Node((*ne[i])->NODEID,(*ne[i])->ne->getIP()));
		//	Node_vector.push_back(new Pastry_Node(_NODEID,_ip);
		//	//~ Node_vector[i]->nodeID=(*ne[i])->NODEID;
		//	Node_vector[i]->nodeID=_NODEID;
		//	//~ Node_vector[i]->ip=(*ne[i])->ne->getIP();
		//	Node_vector[i]->ip=_ip;
		//	//~ Node_vector[i]->ne->ip=Node_vector[i]->ip;
		//	//~ Node_vector[i]->ne->nodeID=Node_vector[i]->nodeID;
		//	
		//}
		//~ for (int i=0;i<3;i++){
			//~ cout <<"****************************************************************"<< endl;
		//~ }
		//~ for (int i=0;i<pastryNetwork.size();i++){
			//~ cout << "NODEID DECIMAL: "<<BN_bn2dec((*pastryNetwork[i])->NODEID) << " - NODEID con STRUCT: "<< BN_bn2dec(Node_vector[i]->ne->getNodeID()) << endl;
			//~ cout << "IP DECIMAL: "<< (*pastryNetwork[i])->ne->getIP() << " - IP con STRUCT: "<< (Node_vector[i])->ne->getIP() << endl;
			//Node_vector[i]->ip=(*ne[i])->ne->getIP();
		//~ }
		//~ for (int i=0;i<3;i++){
			//~ cout <<"****************************************************************"<< endl;
		//~ }
		//* *****/
	}


   //~ void buildRoutingTables();
   //~ void buildLeafsets();
   void buildRoutingTables2();
   void buildLeafsets2();

   //~ void fillLevel(int curLevel, int begin, int end, handle<PastryProtocol> *p);
   void fillLevel2(int curLevel, int begin, int end, int n);

   //~ void printLeafsets()
   //~ {
      //~ int sz = pastryNetwork.size();
      //~ cout << "-----START LEAFSETS-----" << endl;
      
      //~ for(int l=0 ; l< sz ; l ++)
      //~ {
         //~ handle<PastryProtocol>* p = get(l);
	 //~ cout << (*p)->leafset->toString() << endl;
      //~ }
      //~ cout << "----- END LEAFSETS -----" << endl;
   //~ }

	
   //~ void printRoutingTable()
   //~ {
      //~ int sz = pastryNetwork.size();
      //~ cout << "-----START RT -----" << endl;
		
      //~ for( int l=0 ; l< sz ; l ++)
      //~ {
         //~ handle<PastryProtocol>* p = get(l);
	 //~ string s = ((*p)->rt->toString( (*p)->ne )) ;
         //~ cout << s  << endl;
	 //~ //delete s;
      //~ }
      //~ cout << "----- END RT -----" << endl;	
   //~ }


   //~ handle<PastryProtocol>* get(int i);
   
	static bool compare ( Pastry_Node* l, Pastry_Node* r )
	{
		int temp;
		//~ temp = BN_cmp((*l)->NODEID,(*r)->NODEID);
		temp = BN_cmp(l->nodeID,r->nodeID);
		if (temp < 0)
		  return true;
		else 
		  return false;
	}


   void execute()
   {		
      // Sort nodes by nodeID
      //~ sort(pastryNetwork.begin(), pastryNetwork.end(), PastryProtocol::compare);	
	  sort(Node_vector.begin(), Node_vector.end(), compare);	

//      for (unsigned int i =0 ; i < pastryNetwork.size(); i++)
//      {
//         const char* s = UTIL::myBN2HEX( (*pastryNetwork[i])->NODEID);
//         cout << s  << endl;
	 //free((char*)s);
//	 s.clear();
//      }
		
      // Build Routing Tables}
      
      //~ buildRoutingTables();
      //~ cout <<"buildRoutingTables"<< endl;
      assert((int)Node_vector.size()>leafsetSize);
      buildRoutingTables2();
      //~ cout <<"buildRoutingTable2"<< endl;
		
      // Print Routing Tables
  //    printRoutingTable();

      // Build Leafsets
      //~ buildLeafsets();
      //~ cout <<"buildLeafsets"<< endl;
      buildLeafsets2();
      //~ cout <<"buildLeafsets2"<< endl;
	
      // Print Leafsets
    //  printLeafsets();
   }

};

#endif 
