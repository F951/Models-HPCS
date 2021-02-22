#ifndef _ROUTING_TABLE_H
#define _ROUTING_TABLE_H

#include "../../glob.h"
#include "NodeEntry.h"
//~ #include "../auxiliar/Util.h"
#include "../../auxiliar/Util.h"

class RoutingTable 
{

public:

NodeEntry *EMPTY;

NodeEntry  ***table;
int rows;
int cols;


   ~RoutingTable()
   {
//       for (int i=0; i< rows; i++)
//       {
//	 for( int j=0; j < cols; j++ )
//		              free(table[i][j]);
//	delete[] table;
//        }
   free (table);
   }

// RoutingTable Constructor
RoutingTable(int _rows, int _cols)
{

   EMPTY = NULL;
   rows = _rows;
   cols = _cols;
	
   table = new NodeEntry **[rows];
   for (int i=0; i< rows; i++)
   {
      table[i] = new NodeEntry*[cols];
      for( int j=0; j < cols; j++ )
         table[i][j] = EMPTY;
   }

}

// Retrieves the NodeEntry at row, col
// Returns: NodeEntry
NodeEntry* get( int rows, int cols )
{
   return table[rows][cols];
}

// Sets a NodeEntry at row, col
// Returns: Void
void set( int row, int col, NodeEntry* value)
{
   table[row][col] = value;
}

//Copy a complete row
void copyRow( RoutingTable rt, int i);

//Checks RoutingTable for candidate entries to continue routing
NodeEntry* checkRoutingTable( BIGNUM* key, NodeEntry* currentNode);

//Prints RoutingTable content
string toString( NodeEntry* e);

};

#endif 
