#ifndef _STATEBUILDER_h
#define _STATEBUILDER_h

#include "../glob.h"
#include "Leafset.h"
#include "NodeEntry.h"
#include "RoutingTable.h"
#include "PastryProtocol.h"
#include "../auxiliar/Util.h"

// Class used to initialize P2P network
class StateBuilder {
	
   vector<handle<PastryProtocol>*> pastryNetwork; 
   int leafsetSize;	

public:

   StateBuilder(vector<handle<PastryProtocol>*> ne, int _lsize)
   {
      pastryNetwork= ne;
      leafsetSize = _lsize;
   }

   void execute()
   {		
      // Sort nodes by nodeID
      sort(pastryNetwork.begin(), pastryNetwork.end(), PastryProtocol::compare);	
	
//      for (unsigned int i =0 ; i < pastryNetwork.size(); i++)
//      {
//         const char* s = UTIL::myBN2HEX( (*pastryNetwork[i])->NODEID);
//         cout << s  << endl;
	 //free((char*)s);
//	 s.clear();
//      }
		
		// Build Routing Tables
		buildRoutingTables();

		// Print Routing Tables
		//printRoutingTable();

		// Build Leafsets
		buildLeafsets();

		// Print Leafsets
		//printLeafsets();
   }

   void buildRoutingTables();
	
   void buildLeafsets();

   void fillLevel(int curLevel, int begin, int end, handle<PastryProtocol> *p);

   void printLeafsets()
   {
      int sz = pastryNetwork.size();
      cout << "-----START LEAFSETS-----" << endl;
      
      for(int l=0 ; l< sz ; l ++)
      {
         handle<PastryProtocol>* p = get(l);
	 cout << (*p)->leafset->toString() << endl;
      }
      cout << "----- END LEAFSETS -----" << endl;
   }

	
   void printRoutingTable()
   {
      int sz = pastryNetwork.size();
      cout << "-----START RT -----" << endl;
		
      for( int l=0 ; l< sz ; l ++)
      {
         handle<PastryProtocol>* p = get(l);
	 string s = ((*p)->rt->toString( (*p)->ne )) ;
         cout << s  << endl;
	 //delete s;
      }
      cout << "----- END RT -----" << endl;	
   }


   handle<PastryProtocol>* get(int i);

};

#endif 
