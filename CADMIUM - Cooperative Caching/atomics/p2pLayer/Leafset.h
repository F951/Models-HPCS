#ifndef _LEAFSET_H
#define _LEAFSET_H

#include "../../glob.h"
#include "../../auxiliar/Util.h"
#include "NodeEntry.h"

// Leafset Class encapsulates the behavior of responsable peer neigbourhood
// These peers are the +- L/2 closest, with L the size of the leafset
class Leafset

{ 

public:
   BIGNUM** left ;
   BIGNUM** right;
	
   vector<NodeEntry*> nodesL;	
   vector<NodeEntry*> nodesR;
	
   // Total size leaf set
   unsigned int size ;
	
   // Half leafseat size
   unsigned int hsize;
	
   BIGNUM* nodeID;

//   BIGNUM *result1;
//    BIGNUM *result2;



   ~Leafset()
   {
//       for(unsigned int i=0; i< hsize ; i++)
//       {
//           free(left[i]);
//	   free(right[i]);
//       }
       free( left);
       free( right);

       free (nodeID);
       nodesL.clear();
       nodesR.clear();
   }

// Constructor
Leafset( BIGNUM* _myNodeID, int _size )
{
   nodeID = _myNodeID;
   size= _size;
   hsize =_size/2;

   left = new BIGNUM*[hsize];
   right= new BIGNUM*[hsize];
   for(unsigned int i=0; i< hsize ; i++)
   {	
      left[i]=UTIL::EMPTY;
      right[i]=UTIL::EMPTY;
   }

   // result1  = BN_new();
   // result2 = BN_new();
	//cout << "new leafset" << endl;
}


// Makes a shift of the array by one position, starting from pos
// the idea is to make an available slot at the index pos
void shift( BIGNUM** v, int pos );

// Returns true if k is between the leftmost and the rightmost
bool encompass( BIGNUM* k);

// Outputs array of nodes in the leafset
vector<NodeEntry*> listAllNodes();

// Outputs a representation of the leafset
string toString();
 
// Ckecks candidates entries from leafset to continue routing
NodeEntry* checkLeafset( BIGNUM* dest );

// Inserts a node in the leafset
void push(BIGNUM* l);
  
  
void myreplace( string&, const string&, const string& );

BIGNUM* leftmost();

BIGNUM* rightmost();

int indexOf( BIGNUM* );

int isInRight( BIGNUM* );

int isInLeft( BIGNUM* );

void removeNodeId( BIGNUM* );

int correctRightPosition( BIGNUM* );

int correctLeftPosition( BIGNUM* );

void pushToRight( BIGNUM*, NodeEntry* );

void pushToLeft( BIGNUM*, NodeEntry* );

int countNonEmpty( BIGNUM[] );

bool checkCriticInterval( BIGNUM* );

void push( BIGNUM*, NodeEntry* );

bool containsNodeId( BIGNUM* );

BIGNUM* absMinDist( BIGNUM*, BIGNUM* );

};


#endif
