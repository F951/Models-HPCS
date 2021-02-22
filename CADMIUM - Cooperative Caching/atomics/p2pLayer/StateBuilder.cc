#include "StateBuilder.h"
#include <time.h>


//~ void StateBuilder::buildRoutingTables()
//~ {
	//~ srand ( time(NULL) );
	//~ int sz = pastryNetwork.size();
	//~ int begin = 0;
	//~ int end = 0;

	//~ //For every node in the network, get TMP id in HEX format.
	//~ for(int n=0; n < sz; n++)
	//~ {
		//~ handle<PastryProtocol> * p = get(n);
		//~ (*p)->tmp_nodeID= UTIL::myBN2HEX2((*p)->ne->getNodeID());
	//~ }
	
	//~ //For every node in the network
	//~ for(int n=0; n < sz; n++) 
	//~ {
		//~ begin = 0;
		//~ end = 0;
		//~ handle<PastryProtocol> * p = get(n);

		//~ //Iterate over the BASE constant. Is the number of elements in the numeric base. e.g: HEX->16, DEC->10, BIN->2.
		//~ for(int i = 0; i < BASE ; i++)
		//~ {
			//~ if(begin >= sz)
				//~ break;
			
			//~ //Convert "int" to "HEX" in char format. e.g: "15"->"F"
			//~ char currentChar = UTIL::getDigit(i);
			//~ //See if "begin" node in BASE starts with HEX digit. If not, check next digit.
			//~ if(!UTIL::startWith((*get(begin))->tmp_nodeID, currentChar ) )
				//~ continue;
			//~ //If yes... keep going
			//~ end = begin;
			//~ //If starts with the current digit, increase end.
			//~ while( (end < sz) && (UTIL::startWith((*get(end))->tmp_nodeID, currentChar)) )
				//~ end++;
			//~ int randomIndex; 
			//~ //create random index, different of the current node ID.
			//~ do
			//~ {
				//~ randomIndex = begin + (rand() % (end-begin) );		//Always an "int" between 0 and sz.
			//~ }
			//~ while ( ( end-begin) > 1  && 	( BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0) );
			
			//~ //if random index is equal to current nodeID and the size of interval is 1
			//~ if((end-begin) ==  1 && (BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0))
				//~ (*p)->rt->set(0, UTIL::chartoIndex(currentChar), NULL);		//Do this -> table[0][to_int(currentChar)] = NULL;	(We don't add the random index)
			//~ else
				//~ (*p)->rt->set(0, UTIL::chartoIndex(currentChar), (*get(randomIndex))->ne ); //Do this -> table[0][to_int(currentChar)] = randomIndex->ne; (We add the random index)		
			
			//~ //If tmpNodeID begins with current digit (is it the same has "startsWith"?), fill level. 
			//~ if(UTIL::hasDigitAt((*p)->tmp_nodeID, 0, currentChar))
				//~ fillLevel(1, begin, end, p);
			
			//~ begin = end;
		//~ }
			//~ //cout <<n  << endl;
	//~ }
	
	//~ //Free tmp IDs.
	//~ for(int n=0; n < sz; n++){
		//~ handle<PastryProtocol> * p = get(n);
		//~ free((char*)((*p)->tmp_nodeID));
	//~ }
//~ }
void StateBuilder::buildRoutingTables2()
{
	//srand ( time(NULL) );
	srand(1);
	int sz = Node_vector.size();
	int begin = 0;
	int end = 0;
	srand (1);
	//For every node in the network, get TMP id in HEX format.
	for(int n=0; n < sz; n++)
	{
		//~ handle<PastryProtocol> * p = get(n);
		//~ (*p)->tmp_nodeID= UTIL::myBN2HEX2((*p)->ne->getNodeID());
		Node_vector[n]->tmp_nodeID=UTIL::myBN2HEX2(Node_vector[n]->ne->getNodeID());
	}
	//For every node in the network
	for(int n=0; n < sz; n++) 
	{
		begin = 0;
		end = 0;
		//~ handle<PastryProtocol> * p = get(n);

		//Iterate over the BASE constant. Is the number of elements in the numeric base. e.g: HEX->16, DEC->10, BIN->2.
		for(int i = 0; i < BASE ; i++)
		{
			if(begin >= sz)
				break;
			
			//Convert "int" to "HEX" in char format. e.g: "15"->"F"
			char currentChar = UTIL::getDigit(i);
			//See if "begin" node in BASE starts with HEX digit. If not, check next digit.
			//~ if(!UTIL::startWith((*get(begin))->tmp_nodeID, currentChar ) )
			if(!UTIL::startWith(Node_vector[begin]->tmp_nodeID, currentChar ) )
				continue;
			//If yes... keep going
			end = begin;
			//If starts with the current digit, increase end.
			//~ while( (end < sz) && (UTIL::startWith((*get(end))->tmp_nodeID, currentChar)) )
			while( (end < sz) && (UTIL::startWith(Node_vector[end]->tmp_nodeID, currentChar)) )
				end++;
			int randomIndex; 
			//create random index, different of the current node ID.
			do
			{
				randomIndex = begin + (rand() % (end-begin) );		//Always an "int" between 0 and sz.
				//~ std::cout << " # "<< randomIndex << ", ";
			}
			
			while ( ( end-begin) > 1  && 	( BN_cmp(Node_vector[randomIndex]->ne->getNodeID(),Node_vector[n]->ne->getNodeID() ) == 0) );
			//~ while ( ( end-begin) > 1  && 	( BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0) );
			
			//if random index is equal to current nodeID and the size of interval is 1
			//~ if((end-begin) ==  1 && (BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0))
			if((end-begin) ==  1 && ( BN_cmp(Node_vector[randomIndex]->ne->getNodeID(),Node_vector[n]->ne->getNodeID() ) == 0))
			{	//~ (*p)->rt->set(0, UTIL::chartoIndex(currentChar), NULL);		//Do this -> table[0][to_int(currentChar)] = NULL;	(We don't add the random index)
				Node_vector[n]->rt->set(0, UTIL::chartoIndex(currentChar), NULL);		//Do this -> table[0][to_int(currentChar)] = NULL;	(We don't add the random index)
			}
			else
			{ 	//~ (*p)->rt->set(0, UTIL::chartoIndex(currentChar), (*get(randomIndex))->ne ); //Do this -> table[0][to_int(currentChar)] = randomIndex->ne; (We add the random index)					
				Node_vector[n]->rt->set(0, UTIL::chartoIndex(currentChar), Node_vector[randomIndex]->ne ); //Do this -> table[0][to_int(currentChar)] = randomIndex->ne; (We add the random index)		
			}
			//If tmpNodeID begins with current digit (is it the same has "startsWith"?), fill level. 
			//~ if(UTIL::hasDigitAt((*p)->tmp_nodeID, 0, currentChar))
			if(UTIL::hasDigitAt(Node_vector[n]->tmp_nodeID, 0, currentChar)){
				//~ fillLevel(1, begin, end, p);
				fillLevel2(1, begin, end,n);
			}
			begin = end;
		}
			//cout <<n  << endl;
	}
	
	//Free tmp IDs.
	//~ for(int n=0; n < sz; n++){
		//~ handle<PastryProtocol> * p = get(n);
		//~ free((char*)((*p)->tmp_nodeID));
	//~ }
}


//~ void StateBuilder::fillLevel( int curLevel, int begin, int end, handle<PastryProtocol> *p )	//Recursive
//~ {
	//~ // In glob.h, D=10. D is the depth of the routing table
	//~ if(curLevel >= D)
		//~ return;
	
	//~ int end2 = begin;
	//~ int begin2 = begin;

	//~ for(int i =0; i < BASE; i++)
	//~ {
		//~ if(begin2 >= end)
			//~ break;

		//~ char curChar = UTIL::getDigit(i);

		//~ if(!UTIL::hasDigitAt( (*get(begin2))->tmp_nodeID, curLevel , curChar   ))
			//~ continue;

		//~ while ( (end2 < end ) && (UTIL::hasDigitAt( (*get(end2))->tmp_nodeID, curLevel, curChar )) )
			//~ end2++;

		//~ if(end2 == begin2)
			//~ return;

		//~ int randomIndex;
		//~ do
		//~ {
			//~ randomIndex = begin2 + (rand() % (end2-begin2) );
		//~ }
		//~ while ( (end2-begin2) > 1  &&  (BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0) );

		//~ if((end2-begin2) ==  1 && (BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0))
			//~ (*p)->rt->set(curLevel, UTIL::chartoIndex(curChar), NULL);
		//~ else
			//~ (*p)->rt->set(curLevel, UTIL::chartoIndex(curChar), (*get(randomIndex))->ne );

		//~ if(UTIL::hasDigitAt( (*p)->tmp_nodeID,curLevel, curChar ) )
			//~ fillLevel(curLevel+1, begin2, end2, p);

		//~ begin2 = end2;
	//~ }

//~ }

void StateBuilder::fillLevel2( int curLevel, int begin, int end,int n /***handle<PastryProtocol> *p***/ )	//Recursive
{
	srand (1);
	// In glob.h, D=10. D is the depth of the routing table
	if(curLevel >= D)
		return;
	
	int end2 = begin;
	int begin2 = begin;

	for(int i =0; i < BASE; i++)
	{
		if(begin2 >= end)
			break;

		char curChar = UTIL::getDigit(i);

		//~ if(!UTIL::hasDigitAt( (*get(begin2))->tmp_nodeID, curLevel , curChar   ))
		if(!UTIL::hasDigitAt( (Node_vector[begin2])->tmp_nodeID, curLevel , curChar   ))
			continue;

		//~ while ( (end2 < end ) && (UTIL::hasDigitAt( (*get(end2))->tmp_nodeID, curLevel, curChar )) )
		while ( (end2 < end ) && (UTIL::hasDigitAt( Node_vector[end2]->tmp_nodeID, curLevel, curChar )) )
			end2++;

		if(end2 == begin2)
			return;

		int randomIndex;
		do
		{
			randomIndex = begin2 + (rand() % (end2-begin2) );
		}
		while ( (end2-begin2) > 1  &&  (BN_cmp(Node_vector[randomIndex]->ne->getNodeID(),Node_vector[n]->ne->getNodeID() ) == 0) );
		//~ while ( (end2-begin2) > 1  &&  (BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0) );

		//~ if((end2-begin2) ==  1 && (BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0))
		if((end2-begin2) ==  1 && (BN_cmp(Node_vector[randomIndex]->ne->getNodeID(),Node_vector[n]->ne->getNodeID() ) == 0))
		{
			//~ (*p)->rt->set(curLevel, UTIL::chartoIndex(curChar), NULL);
			Node_vector[n]->rt->set(curLevel, UTIL::chartoIndex(curChar), NULL);
		}
		else
		{
				//~ (*p)->rt->set(curLevel, UTIL::chartoIndex(curChar), (*get(randomIndex))->ne );
				Node_vector[n]->rt->set(curLevel, UTIL::chartoIndex(curChar), Node_vector[randomIndex]->ne );
		}
		//~ if(UTIL::hasDigitAt( (*p)->tmp_nodeID,curLevel, curChar ) )
		if(UTIL::hasDigitAt( Node_vector[n]->tmp_nodeID,curLevel, curChar ) )
		{
			fillLevel2(curLevel+1, begin2, end2, n);
		}

		begin2 = end2;
	}

}



//~ void StateBuilder::buildLeafsets(){

   //~ int sz = pastryNetwork.size();

   //~ // CASO de los extremos partiendo de cero - counterwise
   //~ for (int k = 0; k < leafsetSize/2 ; k++)
   //~ {	 	
      //~ handle<PastryProtocol>* p = get(k);
      //~ for(int s = k-1 ; s>=0; s--)
         //~ (*p)->leafset->pushToLeft((*get(s))->NODEID, (*get(s))->ne);
      //~ for(int s = sz - 1 ; s >= sz - leafsetSize/2 +k ; s--) 
         //~ (*p)->leafset->pushToLeft((*get(s))->NODEID, (*get(s))->ne);
   //~ }

   //~ // CASO DE LOS EXTREMOS PARTIENDO de 2^128 - clockwise
	
   //~ for (int k = sz -1 ; k >= sz - leafsetSize/2 ; k--)
   //~ {
      //~ handle<PastryProtocol>* p = get(k);
      //~ for ( int s = k+1 ; s < sz ; s++)
         //~ (*p)->leafset->pushToRight((*get(s))->NODEID, (*get(s))->ne);
      //~ for(int s =0; s <= leafsetSize/2 -(sz-k) ; s++)
         //~ (*p)->leafset->pushToRight((*get(s))->NODEID, (*get(s))->ne);
   //~ }

   //~ // CASO GENERAL RIGHT
   //~ for (int k = 0 ; k < sz - leafsetSize/2 ; k++)
   //~ {
      //~ handle<PastryProtocol>* p = get(k);
      //~ for(int s = k ; s <= k+ leafsetSize/2 ; s++)
         //~ (*p)->leafset->pushToRight((*get(s))->NODEID, (*get(s))->ne);
   //~ }
   
   //~ // CASO GENERAL LEFT
   //~ for(int k = leafsetSize/2 ; k < sz ; k++)
   //~ {
      //~ handle<PastryProtocol>* p = get(k);
      //~ for(int s = k - 1; s > k-1 - leafsetSize/2 ; s--)
         //~ (*p)->leafset->pushToLeft((*get(s))->NODEID, (*get(s))->ne);
   //~ }
//~ }


void StateBuilder::buildLeafsets2()
{
	srand (1);
	int sz = Node_vector.size();
	// CASO de los extremos partiendo de cero - counterwise
	for (int k = 0; k < leafsetSize/2 ; k++)
	{	 	
		//~ handle<PastryProtocol>* p = get(k);
		for(int s = k-1 ; s>=0; s--)
		{
			//~ (*p)->leafset->pushToLeft((*get(s))->NODEID, (*get(s))->ne);
			Node_vector[k]->leafset->pushToLeft(Node_vector[s]->nodeID, Node_vector[s]->ne);
		}
		for(int s = sz - 1 ; s >= sz - leafsetSize/2 +k ; s--) 
		{
			//~ (*p)->leafset->pushToLeft((*get(s))->NODEID, (*get(s))->ne);
			Node_vector[k]->leafset->pushToLeft(Node_vector[s]->nodeID, Node_vector[s]->ne);
		}
	}
	// CASO DE LOS EXTREMOS PARTIENDO de 2^128 - clockwise

	for (int k = sz -1 ; k >= sz - leafsetSize/2 ; k--)
	{
		//~ handle<PastryProtocol>* p = get(k);
		for ( int s = k+1 ; s < sz ; s++){
			//~ (*p)->leafset->pushToRight((*get(s))->NODEID, (*get(s))->ne);
			Node_vector[k]->leafset->pushToRight(Node_vector[s]->nodeID, Node_vector[s]->ne);
		}

		for(int s =0; s <= leafsetSize/2 -(sz-k) ; s++){
			//~ (*p)->leafset->pushToRight((*get(s))->NODEID, (*get(s))->ne);
			Node_vector[k]->leafset->pushToRight(Node_vector[s]->nodeID, Node_vector[s]->ne);
		}
	}
	// CASO GENERAL RIGHT
	for (int k = 0 ; k < sz - leafsetSize/2 ; k++)
	{
		//~ handle<PastryProtocol>* p = get(k);
		for(int s = k ; s <= k+ leafsetSize/2 ; s++)
		{
			//~ (*p)->leafset->pushToRight((*get(s))->NODEID, (*get(s))->ne);
			Node_vector[k]->leafset->pushToRight(Node_vector[s]->nodeID, Node_vector[s]->ne);
		}
	}
	// CASO GENERAL LEFT
	for(int k = leafsetSize/2 ; k < sz ; k++)
	{
		//~ handle<PastryProtocol>* p = get(k);
		for(int s = k - 1; s > k-1 - leafsetSize/2 ; s--)
		{
			//~ (*p)->leafset->pushToLeft((*get(s))->NODEID, (*get(s))->ne);
			Node_vector[k]->leafset->pushToLeft(Node_vector[s]->nodeID, Node_vector[s]->ne);
		}
	}
}

//~ handle<PastryProtocol>* StateBuilder::get(int i)
//~ {
   //~ return pastryNetwork[i];
//~ }
