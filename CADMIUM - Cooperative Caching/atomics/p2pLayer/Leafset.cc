#include "Leafset.h"

//Performs a shift at position pos
void Leafset::shift( BIGNUM** v, int pos ) 
{
   for( int i = hsize-1; i > pos ; i--)
      v[i] = v[i-1];
}


// Check different encompass
bool Leafset::encompass( BIGNUM* k )
{
   bool ret = false;
   // normal area
   if (BN_cmp(leftmost(),rightmost())==-1)
   {
      if((BN_cmp(leftmost(),k)==-1) && (BN_cmp(k,rightmost())==-1))
         ret= true;
   }
   // critical area
   else
   {
      if ((BN_cmp(k,rightmost())==-1))
         ret = true;
      else if((BN_cmp( leftmost(),k)==-1))
         ret = true;
   }
   return ret;
}

// List of all leafset nodes
// Returns: Vector containing all the leafset nodes
vector<NodeEntry*> Leafset::listAllNodes()
{

   int numLeft = nodesL.size();
   int numRight = nodesR.size();

   vector<NodeEntry*> result(numLeft + numRight);

   for(int i = 0 ; i< numLeft; i++)
      result[i]=nodesL[i];
   for(int i = 0 ; i < numRight ; i++)
      result[numLeft+i]=nodesR[i];

   return result;
}

// Prints Leafset
// Returns: String to print
string Leafset::toString()
{
   string l = "[XX]";
   for (int i= hsize -1 ; i>= 0 ; i--)
   {
      if (BN_cmp(left[i], UTIL::EMPTY)!=0)
      {
         string b = UTIL::truncateNodeId(left[i]);
	 b.append(";XX");
	 myreplace(l, "XX", b);
      }
   }
   myreplace(l,";XX","");
   myreplace(l,"XX","");
   string p = "{";
   
   p.append(UTIL::truncateNodeId(nodeID));
   p.append("}");
   l.append(p);
		
   string r = "[XX]";

   for (unsigned int i = 0; (i<hsize) && (BN_cmp(right[i], UTIL::EMPTY)!=0); i++)
   {
      string c= UTIL::truncateNodeId(right[i]);
      c.append(";XX");
      myreplace(r,"XX",c);
   }
   
   myreplace(r,";XX", "");
   myreplace(r,"XX","");

   l.append(r);

   return l;
}

// Checks candidate entries to continue routing
// Returns: Candidate NodeEntry
NodeEntry* Leafset::checkLeafset( BIGNUM* m )
{
   BIGNUM* min = NULL;
   NodeEntry* minNode = NULL;
   vector<NodeEntry*> allNodes;

   allNodes = listAllNodes();
	unsigned int i;
   //~ for( i=0; i < allNodes.size(); i++){   cout << "Allnodes " << i << " IP: " << allNodes[i]->getIP() << " - NODEID: " << BN_bn2dec(allNodes[i]->getNodeID()) << endl; }

   //~ cout << "CheckLeafset() - m:" << BN_bn2dec(m) << endl;
	for( i=0; i < allNodes.size(); i++)
	{
		//~ cout << "CheckLeafset() " << i << endl;
		if(min==NULL)
		{
			min = absMinDist(m,allNodes[i]->getNodeID());
			//~ cout << "Min OK " << i << endl;
			minNode = allNodes[i];
		}
		else
		{
			BIGNUM *tmp= absMinDist(m, allNodes[i]->getNodeID());
			//~ cout << "TMP " << i << endl;
			if(BN_cmp(min, tmp)>0)
			{
				//~ cout << "Min Node is " << i << endl;
				BN_clear_free(min);
				min = tmp;
				minNode= allNodes[i];
			}
			else
			{
				BN_free(tmp);
			}
		}
	}	
	//~ cout << "Exit CheackLeafset() " << i << endl;
   BN_clear_free(min);
   return minNode;
}

// Calculate the absolute distance between two IDs
// Returns: distance
BIGNUM* Leafset::absMinDist( BIGNUM* a, BIGNUM* b )
{
   BIGNUM *result1  = BN_new();
   BIGNUM *result2 = BN_new();
   if(BN_cmp(a,b)>0)
   {
	 
      BN_sub(result1, b,a);
      BN_sub(result2, UTIL::MAX,b);
      BN_add(result2, result2, a);
   }
   else
   {
		BN_sub(result1, a,b);
		BN_sub(result2, UTIL::MAX, a);
		BN_add(result2, result2,b);
   }
   if(BN_cmp(result1,result2)>0)
   {
      BN_free(result2);
      return result1;
   }
   else
   {
      BN_free(result1);
      return result2;
   }
}


// Replaces the string old within str with newStr
// Return: void
void Leafset::myreplace( string& str, const string& oldStr, const string& newStr )
{
   size_t pos = 0;
   while ((pos =str.find(oldStr, pos)) != std::string::npos)
   {
      str.replace(pos, oldStr.size(), newStr);
      pos +=newStr.length();
   }
}

// Returns the leftmost nodeid
BIGNUM* Leafset::leftmost()
{
   if(nodesL.size() == 0)
      return nodeID;
   return left[nodesL.size()-1];
}

// Returns the rightmost nodeid
BIGNUM* Leafset::rightmost()
{
   if(nodesR.size() == 0 )
      return nodeID;
   return right[nodesR.size()-1];
}

// Returns the index of an ID from the leafset
int Leafset::indexOf( BIGNUM* keyToFind )
{
   unsigned int index =0;
   while(index < hsize)
   {
      if (BN_cmp(keyToFind, left[index])==0)
         return index;
      else if (BN_cmp(keyToFind, right[index])==0)
         return index;
   
      index++;
   }
   return -1;
}

// Returns the position of a key on the right part of the leafset
// Returns: the position 
int Leafset::isInRight(BIGNUM* b)
{
   if (b==NULL)
      return -1;
   
   for (unsigned int index=0; index < hsize; index ++)
      if ( BN_cmp(right[index], b)==0)
         return index;
   
   return -1;

}

// Returns the position of a key on the left part of the leafset
// Returns: the position
int Leafset::isInLeft(BIGNUM* b)
{
   if(b==NULL)
      return -1;
   
   for(unsigned int index=0; index < hsize; index ++)
      if (BN_cmp(left[index], b)==0)
         return index;

   return -1;

}

// Removes nodeID from the Leafset
// Returns: void
void Leafset::removeNodeId(BIGNUM* b)
{
   int l=-1;
   int r = -1;

   r = isInRight(b);
   l =  isInLeft(b);

   if (r!=-1)
   {
      for(unsigned int i = r ; i < hsize-1;i++)
         right[i] = right[i+1];
		
      right[hsize -1] = UTIL::EMPTY;
      nodesR.erase(nodesR.begin()+r);
   }

   if (l!=-1)
   {
      for(unsigned int i = l ; i < hsize-1;i++)
         left[i] = left[i+1];
      
      left[hsize -1] = UTIL::EMPTY;
      nodesL.erase(nodesL.begin()+l);

   }
}

// Returns the position on the right part of the leafset to correct
// Returns: the position
int Leafset::correctRightPosition(BIGNUM* n)
{
   unsigned int l=0;
   while(l< hsize &&( !(right[l]==UTIL::EMPTY)))
   {
      if ( BN_cmp(right[l],n)==0)
         return -1; // do not reinsert
   
      l++;
   }

   if (BN_cmp(n, nodeID)>0)
   {
      for(unsigned int i = 0 ; i < hsize ; i++)
      {
         if(right[i] == UTIL::EMPTY)
	    return i;
	 if(BN_cmp(right[i], n) == 0)
            return -1;
         if((BN_cmp(right[i],n) > 0) && (BN_cmp(right[i], nodeID) > 0))
	    return i;
	 if((BN_cmp(right[i],n) < 0) && (BN_cmp(right[i],nodeID) < 0))
            return i;
      }
      return hsize;

   }
   else
   {
      for(unsigned int i = 0 ; i < hsize ; i++)
      {
         if(right[i]== UTIL::EMPTY)
	    return i;
	 if(BN_cmp(right[i],n)== 0)
            return -1;
	 if((BN_cmp(right[i],n) > 0) && (BN_cmp(right[i], nodeID)<0))
            return i;
      }
      return hsize;
   }
}

// Returns the position on the left part of the leafset to correct
// Returns:  the position
int Leafset::correctLeftPosition(BIGNUM* n)
{
   unsigned int l=0;
   while( l< hsize && (!(left[l] == UTIL::EMPTY)))
   {
      if (BN_cmp(left[l],n)==0)
         return -1; // do not reinsert
      l++;

   }

   if(BN_cmp(n,nodeID)<0)
   {
      for(unsigned int i=0; i < hsize ; i++)
      {
         if(left[i] == UTIL::EMPTY)
            return i;
         if( BN_cmp(left[i],n)==0 )
            return -1;
         if( (BN_cmp(left[i],n) < 0 ) && ( BN_cmp(left[i],nodeID) < 0) )
            return i;
         if( (BN_cmp(left[i],n) > 0 ) && (BN_cmp(left[i],nodeID) > 0) )
            return i;
      }
      return hsize;
   }
   else
   {
      for(unsigned int i = 0 ; i < hsize ; i++)
      {
         if(left[i]==UTIL::EMPTY)
            return i;
         if(BN_cmp(left[i],n)== 0)
	    return -1;
	 if( BN_cmp(left[i],n) < 0 && BN_cmp(left[i],nodeID) > 0 )
	    return i;
      }
      return hsize;
   }

}


// Insert a nodeEntry ne into the right part of the leafset
// Returns: void
void Leafset::pushToRight(BIGNUM* newNode, NodeEntry* ne)
{
   if(BN_cmp(newNode,nodeID)==0)
      return;
    int index = correctRightPosition(newNode);

   if (index == -1)
      return;
   if ((unsigned)index == hsize )
      return ;

   shift(right, index );

   right[index] = newNode;
   nodesR.insert(nodesR.begin()+index,ne);
	
}

// Inserts a NodeEntry ne to the left part of the leafset
// Returns: void
void Leafset::pushToLeft(BIGNUM* newNode, NodeEntry* ne)
{

   if (BN_cmp(newNode,nodeID)==0)
      return;
	
   int index = correctLeftPosition(newNode);
	
   if (index == -1)
      return ;

   if((unsigned)index == hsize)
      return;

   shift(left,index);
   left[index] = newNode;
   nodesL.insert(nodesL.begin()+index, ne);

}


// Generic method to insert a new NodeEntry to the leafset
// Returns: void
void Leafset::push(BIGNUM* newNode, NodeEntry* ne)
{
   if( BN_cmp(newNode,nodeID)!=0)
   {		
      if(nodesL.size()<hsize || nodesR.size()<hsize)
      {
         if(nodesL.size()<hsize)
            pushToLeft(newNode,ne);
         if(nodesR.size()<hsize)
            pushToRight(newNode, ne);
      }
      if(BN_cmp(newNode,rightmost())<0 && BN_cmp(newNode,nodeID)>0)
         pushToRight(newNode,ne);
      
      if (BN_cmp(newNode,leftmost())>0 && BN_cmp(newNode,nodeID)<0)
         pushToLeft(newNode,ne);
		
      // critical intarval right
      if ( (BN_cmp(newNode,nodeID)>0 && BN_cmp(newNode,rightmost())>0)
           || (BN_cmp(newNode,nodeID)<0 && BN_cmp(newNode,rightmost())<0) )
         pushToRight(newNode,ne);
		
      // critical interval left
      if ( (BN_cmp(newNode,nodeID) <0 && BN_cmp(newNode,leftmost())<0)
            || (BN_cmp(newNode,nodeID)>0 && BN_cmp(newNode,leftmost())>0))
        pushToLeft(newNode,ne);
		
   }
}


// Checks if a given node id is contained on the leafset
// Returns: true if is containes otherwise false
bool Leafset::containsNodeId(BIGNUM* node)
{
   return indexOf(node) !=-1;
}



