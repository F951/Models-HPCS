#include "RoutingTable.h"
#include <sstream>


//Checks RoutingTable candidates entries for routing
//Returns: candidate to next hop
NodeEntry* RoutingTable::checkRoutingTable(BIGNUM* key, NodeEntry* currentNode)
{
   NodeEntry* next = NULL;
   int r = UTIL::prefixLen(key, currentNode );	
	
   if(r == D)
      return currentNode;
	
   if(r >= 0)
   {
      const char* k = UTIL::myBN2HEX(key);
      char tmp = k[r];
      
      if( get(r, UTIL::chartoIndex(tmp)) != NULL )
         next = get(r, UTIL::chartoIndex(tmp) );
      //free((char*)k);
   }
	
   // if(next != NULL)
   //   cout << "Pastry: NEXT RT: " <<UTIL::myBN2HEX(next->getNodeID())<< endl;

   return next;

}


// Prints RoutingTable
// Returns: String to print
string RoutingTable::toString(NodeEntry* e)
{
   std::stringstream s;
   string node = UTIL::truncateNodeId(e->getNodeID());
 //  string line="";
   string line = "+---------------------------------------------------------------------------"+ node + "-----------------------------------------------------------------------------+\n";
  // line.append( "+---------------------------------------------------------------------------");
  // line.append(node);
  // line.append( "-----------------------------------------------------------------------------+\n");
   
   string row =  "          ";

   //free((char*)node);

   for(int j = 0; j < cols ; j++)
   {  
      row = row + "[   " + UTIL::getDigit(j) + "   ]";
   }

   line = line + row + "\n---------------------------------------------------------------------------------------------------------------------------------------------------------\n";
	
   for(int i = 0; i < 6; i++)
   {		
      s << i; 
      row = "row " + s.str() + "     ";
      
      for(int j = 0 ; j < cols; j++)
         row = row + "[" + UTIL::truncateNodeId((table[i][j])) + "]";
      line  = line + row + "\n";
      s.str("");
   }
	
   return line;
}


// Copy a complete row of the Routing Table rt
// Returns: Void
void RoutingTable::copyRow(RoutingTable rt, int i)
{
   for(int col=0; col < cols; )
      table[i][col] = rt.table[i][col];
}
