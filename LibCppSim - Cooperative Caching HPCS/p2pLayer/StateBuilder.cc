#include "StateBuilder.h"
#include <time.h>


void StateBuilder::buildRoutingTables()
{
   srand ( time(NULL) );
   int sz = pastryNetwork.size();
   int begin = 0;
   int end = 0;
	srand (1);
   for(int n=0; n < sz; n++)
   {
      handle<PastryProtocol> * p = get(n);
      (*p)->tmp_nodeID= UTIL::myBN2HEX2((*p)->ne->getNodeID());
    }

   for(int n=0; n < sz; n++) 
   {
      begin = 0;
      end = 0;
      handle<PastryProtocol> * p = get(n);


      for(int i = 0; i < BASE ; i++)
      {
         if(begin >= sz)
			break;
	 
		char currentChar = UTIL::getDigit(i);

		if(!UTIL::startWith((*get(begin))->tmp_nodeID, currentChar ) )
			continue;			
		end = begin;
			
		while( (end < sz) && (UTIL::startWith((*get(end))->tmp_nodeID, currentChar)) )
			end++;
			
		int randomIndex; 
		do
		{
			randomIndex = begin + (rand() % (end-begin) );
		}
		while ( ( end-begin) > 1     &&   ( BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0) );	//Ensure that is not the own ID
		//~ std::cout << " # "<< randomIndex << ", ";	
		if((end-begin) ==  1 && (BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0))
			(*p)->rt->set(0, UTIL::chartoIndex(currentChar), NULL);	
        else
			(*p)->rt->set(0, UTIL::chartoIndex(currentChar), (*get(randomIndex))->ne );
	
		if(UTIL::hasDigitAt((*p)->tmp_nodeID, 0, currentChar))
			fillLevel(1, begin, end, p);
			
         begin = end;
      }

   //cout <<n  << endl;

   }

 for(int n=0; n < sz; n++)
 {
     handle<PastryProtocol> * p = get(n);
     free((char*)((*p)->tmp_nodeID));
  }



}


void StateBuilder::fillLevel( int curLevel, int begin, int end, handle<PastryProtocol> *p)
{
	srand (1);
   if(curLevel >= D)
      return;

   int end2 = begin;
   int begin2 = begin;

   for(int i =0; i < BASE; i++)
   {
      if(begin2 >= end)
         break;

      char curChar = UTIL::getDigit(i);
      
      if(!UTIL::hasDigitAt( (*get(begin2))->tmp_nodeID, curLevel , curChar   ))
         continue;

      while ( (end2 < end ) && 
              (UTIL::hasDigitAt( (*get(end2))->tmp_nodeID, curLevel, curChar )) )
         end2++;

      if(end2 == begin2)
         return;

      int randomIndex;
      do
      {
         randomIndex = begin2 + (rand() % (end2-begin2) );
      }
      while ( (end2-begin2) > 1  && 
               (BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0) );

      if((end2-begin2) ==  1 && (BN_cmp((*get(randomIndex))->ne->getNodeID(),(*p)->ne->getNodeID() ) == 0))
         (*p)->rt->set(curLevel, UTIL::chartoIndex(curChar), NULL);
      else
         (*p)->rt->set(curLevel, UTIL::chartoIndex(curChar), (*get(randomIndex))->ne );

      if(UTIL::hasDigitAt( (*p)->tmp_nodeID,curLevel, curChar ) ){
		 fillLevel(curLevel+1, begin2, end2, p); 
	  }
         

         begin2 = end2;
   }

}



void StateBuilder::buildLeafsets(){

srand (1);
   int sz = pastryNetwork.size();

   // CASO de los extremos partiendo de cero - counterwise
   for (int k = 0; k < leafsetSize/2 ; k++)
   {	 	
      handle<PastryProtocol>* p = get(k);
      for(int s = k-1 ; s>=0; s--)
         (*p)->leafset->pushToLeft((*get(s))->NODEID, (*get(s))->ne);
      for(int s = sz - 1 ; s >= sz - leafsetSize/2 +k ; s--) 
         (*p)->leafset->pushToLeft((*get(s))->NODEID, (*get(s))->ne);
   }

   // CASO DE LOS EXTREMOS PARTIENDO de 2^128 - clockwise
	
   for (int k = sz -1 ; k >= sz - leafsetSize/2 ; k--)
   {
      handle<PastryProtocol>* p = get(k);
      for ( int s = k+1 ; s < sz ; s++)
         (*p)->leafset->pushToRight((*get(s))->NODEID, (*get(s))->ne);
      for(int s =0; s <= leafsetSize/2 -(sz-k) ; s++)
         (*p)->leafset->pushToRight((*get(s))->NODEID, (*get(s))->ne);
   }

   // CASO GENERAL RIGHT
   for (int k = 0 ; k < sz - leafsetSize/2 ; k++)
   {
      handle<PastryProtocol>* p = get(k);
      for(int s = k ; s <= k+ leafsetSize/2 ; s++)
         (*p)->leafset->pushToRight((*get(s))->NODEID, (*get(s))->ne);
   }
   
   // CASO GENERAL LEFT
   for(int k = leafsetSize/2 ; k < sz ; k++)
   {
      handle<PastryProtocol>* p = get(k);
      for(int s = k - 1; s > k-1 - leafsetSize/2 ; s--)
         (*p)->leafset->pushToLeft((*get(s))->NODEID, (*get(s))->ne);
   }
}

handle<PastryProtocol>* StateBuilder::get(int i)
{
   return pastryNetwork[i];
}
