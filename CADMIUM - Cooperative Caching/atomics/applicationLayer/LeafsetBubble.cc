#include "LeafsetBubble.h"


void LeafsetBubble::inner_body(void)
{
   while (1)
   {
      if(queue_in.empty())     
         passivate();
	 // recovers messages from the lower layer
           
      cout<<"LeafsetBubble "<<endl; fflush(stdout);
      if((*observer)->getIteracion() != iteracion)
      {
         inlinks.clear();
         RCACHE->reset();
         iteracion = (*observer)->getIteracion();
         checkWaitTimeOut();
      }
      
      while(!queue_in_lower.empty())
      {
        PastryMessage *temp1 = (PastryMessage*)queue_in_lower.front();
	queue_in_lower.pop_front();
	
	TlcMessage *temp =(TlcMessage*)temp1->getData();
         
	if(temp1->finalDestination)
	   temp->setFinalDest();
	
	  delete temp1;
	
	queue_in.push_back(temp);
	
      }

      ASSERT(!queue_in.empty());

      TlcMessage* tm = queue_in.front();
      queue_in.pop_front(); 

      Entry *e;
      Query *q;
 
      switch (tm->getType())
      {
         case TLC_WSE:
         {	  
	    e= tm->getEntry();
	     
	    RCACHE->insertEntry ( e->clone() );
	  
            vector<TlcMessage*>::iterator it;	    
	    vector< Query*>::iterator at;
	  
            it=outstanding.begin();
            while(it!=outstanding.end())
            { 
               if(BN_cmp(((*it)->getQuery())->hashValue, e->hashValue)==0)
               {

	          TlcMessage* tmp = (*it);
	          Entry* clone = e->clone();
                  sendReply(clone,tmp);
                  RCACHE->update(e->hashValue);
                  outstanding.erase(it);

	          if (tm->getPathSize()>=2)
	          {
		    if(BN_cmp(((*pastry)->ne)->getNodeID(),(tm->src)->getNodeID())!=0)
		        storeInlink(q->hashValue, tm->path[tm->getPathSize()-2]);
		  }
               }
	       else
	       {
                   it++;
	       }
            }
	   
            at = waiting.begin();
	    while(at!=waiting.end())
	    {
	       if(BN_cmp((*at)->hashValue, e->hashValue)==0)
	       {
	          waiting.erase(at);
	       }
	       else
	       {
	          at++;
	       }
	    }

	     delete tm;
	    break;
	 }
         case TLC_LOOKUP:
	 {
	    q = tm->getQuery();
            
	    if((tm->path).size()>0)
	    {
	       if(BN_cmp(((*pastry)->ne)->getNodeID(),(tm->path[(tm->path).size()-1])->getNodeID())!=0)
	          tm -> addPath((*pastry)->ne);
            }
            else
	    {
	       tm -> addPath((*pastry)->ne);
	    }

            if (tm->getFinalDest())
            {
               (*observer)->addHops(tm->path.size()-1);
            }
	    // check global cache entries               
            e = RCACHE->check(q->hashValue);
	  
            if(e!=NULL && !e->old(this->time()))
            {
	       (*observer)->addHitGlobal();     
               sendReply(e->clone(), tm );
	       RCACHE->update(e->hashValue);
	       if (tm->getPathSize()>=2)
	       {
                  if(BN_cmp(((*pastry)->ne)->getNodeID(),(tm->src)->getNodeID())!=0)
                   storeInlink(q->hashValue, tm->path[tm->getPathSize()-2]);
	      }
	    }
	    // if it is the final Dest or it the entry is old.
	    else if( (e==NULL && tm->getFinalDest()) || (e!=NULL && e->old(this->time())) )
            {
               outstanding.push_back(tm);
	       (*observer)->addMiss();
	       (*client)->sendWSEQuery(q);
	    }
	    else 
	    {
	       // check local cache entries
               e= localCache->check(q->hashValue);
	       
	       if(e!=NULL && e->old(this->time()))
	       {
	          localCache->remove(e);
		  e=NULL;
	       }
	       if (e!=NULL)
	       {
	          //COMENTADO:: (*observer)->addHitLocal();
	          sendReply(e->clone(),tm);
		  localCache->update(e->hashValue);
	       }
	       else 
	       {
                     vector<Query*>::iterator ite;
                     ite=waiting.begin();
                     bool wait=false;
                     while(ite!=waiting.end() && !wait)
		     {
		        if( BN_cmp(q->hashValue, (*ite)->hashValue)==0)
		        {
			    	          //COMENTADO:: (*observer)->addHitLocal();:
			    (*observer)->addHops(tm->path.size()-1);
			    outstanding.push_back(tm);
			    wait=true;
			    break;
		        }
			
		        ite++;

		     }
		     if(!wait )
		     {
		        continueRouting(tm);
		     }
		}

		  if (tm->getPathSize()==1)
                  {		     
		     waiting.push_back(tm->query);
	          }
               }   
	    }
	     
	    break;
         }    
	 case TLC_ENDS:
	 {

	    nFin++;
	    if (nFin == NP)
	    {
	       (*replicator)->end();
	       (*observer)->end();
	    }
	    
             if (ends[NP]==1 && !Broadcast)
	     {
	         if(0==ends[(*pastry)->ne->getIP()] || terminadas==ends[(*pastry)->ne->getIP()] )
		 {
		    Broadcast=true;
		    sendEndBroadcast();
		 }			
	      }
            break;
	 }

         case TLC_REPLY:
	 {    
	    e = tm->getEntry();
	    q= tm->getQuery(); 
            localCache->insertEntry(e->clone());
	   
	    vector<Query*>::iterator at;
	    at= waiting.begin();
	  
	    while(at!=waiting.end())
	    { 
	       if(BN_cmp((*at)->hashValue, e->hashValue)==0)
	       {  
                   waiting.erase(at); 
	       } 
	        else
	       {
	         at++;
	       }
            }
	    vector<TlcMessage*>::iterator it1;
	   
	    it1 = outstanding.begin();
	    while(it1!=outstanding.end())
	    {
              if(BN_cmp(e->hashValue, ((*it1)->getQuery())->hashValue)==0)
	       {
	          TlcMessage* tmp = *it1;
	          sendReply(e->clone(),tmp);
                  outstanding.erase(it1); 
	       }
	       else
	       {
	         it1++;
	       }
	    } 
         
            (*observer)->addNQueriesIn();

            terminadas++;
           
	    if (ends[NP]==1)
            {
	 	if(terminadas==ends[(*pastry)->ne->getIP()])
		{
           	    Broadcast=true;
                    sendEndBroadcast();
		}
	    }
	     delete tm->getQuery();
             delete tm;
	    break;
         } 

         case TLC_REPLICATE:
	 {
            switch(CASE_RESPONSABLE)
	    {
	    // ask directly to the WSE
	       case (1):
	       {
                  e = tm->getEntry();
            
	          (*client)->sendWSEQuery( new Query(0, (char*)(e->key).c_str(),BN_dup( e->hashValue), this->time()) );
                  break;
	       }
            // stores entry locally
	       case (0):
	       {
                  RCACHE->insertEntry((tm->getEntry())->clone());
                  break;
               }
	    }
	    
	    delete tm;
	    break;
         }
      }
   }
}

void LeafsetBubble::sendEndBroadcast(){

   TlcMessage *EB= new TlcMessage(TLC_ENDS, msgID);

   PastryMessage *pm = new PastryMessage(PASTRY_BDC, EB);

   (*pastry)->add_message(pm);
   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());
}


void LeafsetBubble::printPath(TlcMessage* tm)
{
   Query* q  = tm->getQuery();
   const char* s = UTIL::myBN2HEX( q->hashValue);
   cout<< "--------- PATH for " << s << " ---------"<<endl;
   for(unsigned int k = 0; k < tm->path.size(); k++)
   {
      const char* t = UTIL::myBN2HEX((tm->path[k])->getNodeID()) ;
      cout << k << " -> " <<  t <<endl;
   }
   cout<<"--------------------------------------------------------------------"<<endl;

}


void LeafsetBubble::continueRouting(TlcMessage* tm)
{
   PastryMessage *pm = new PastryMessage(PASTRY_SENDDHT,(*pastry)->ne,tm->dest, tm);  
   
   (*pastry)->add_message(pm);
 
   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());

}


void LeafsetBubble::sendReply(Entry* _e, TlcMessage* tm)
{

   setAvgQueue();

   (*observer)->addLoad(((*pastry)->ne)->getIP());

   Query * q= tm->getQuery();
   TlcMessage* reply = new TlcMessage(TLC_REPLY, msgID, (*pastry)->ne, q->src);
   msgID = msgID + 1;
   reply->setEntry(_e);
   reply->setQuery(tm->getQuery());
   PastryMessage *pm = new PastryMessage(PASTRY_SENDDIRECT,(*pastry)->ne,q->src, reply);

    
   (*pastry)->add_message(pm);

   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());
  // delete tm;
}

void LeafsetBubble::sendReplica(Entry* e ,NodeEntry* nodeEntry)
{
   TlcMessage* replica = new TlcMessage(TLC_REPLICATE, msgID, (*pastry)->ne, nodeEntry);
   msgID = msgID+1;
   replica->setEntry(e);
   
   PastryMessage *pm = new PastryMessage (PASTRY_SENDDIRECT, (*pastry)->ne, nodeEntry, replica);
   (*pastry)->add_message(pm);

   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());
}



void LeafsetBubble::setAvgQueue()
{
   t_act = this->time() ;

   avg_queue = avg_queue + ( (queue_in.size() + 1) *(t_act - t_pas) );
   t_pas = t_act;
   fflush(stdout);
}

bool TlcProtocol::isReplicated(BIGNUM* nodeId, BIGNUM* key)
{
   map<BIGNUM*, Replicas*>::iterator it;
   it = replicaTrack.begin();
   
   while(it != replicaTrack.end())
   {
      if( BN_cmp(it->first,nodeId) == 0 && (it->second)->isContained(key)  )
         return true;

      ++it;
   }

   return false;
}
				    
bool TlcProtocol::isNewReplica(BIGNUM* id)
{
   map<BIGNUM*, Replicas*>::iterator it;
   it = replicaTrack.begin();
   while(it != replicaTrack.end())
   {
     if( BN_cmp(it->first, id) == 0 )
      return false;
      ++it;
   }

   return true;
}




void TlcProtocol::trackReplica(BIGNUM* nodeId, BIGNUM* key)
{
  bool flag = isNewReplica(nodeId);
  
  
  if(flag)
  {
       Replicas* r = new Replicas(key ,nodeId);
       replicaTrack.insert(pair <BIGNUM*, Replicas*>(nodeId, r) );
       replicaTrack[nodeId] = r;

       (*observer)->addReplicas();

   }else
   {
      map<BIGNUM*, Replicas*>::iterator it5;
      it5 = replicaTrack.begin();

      while(it5 != replicaTrack.end())
      {
         if(BN_cmp(nodeId,it5->first) == 0)
         {
	   (it5->second)->putReplica(key);
           (*observer)->addReplicas();
	 }
	 ++it5;
      }
   }

}


void TlcProtocol::storeInlink(BIGNUM* key, NodeEntry* n)
{
   bool inlinkStored = false;
   Inlink* i;
   map<BIGNUM*,Inlink*>::iterator it;
   it = inlinks.begin();

   while(it != inlinks.end())
   {
      if(BN_cmp(it->first, n->getNodeID())==0)
      {
	  inlinkStored = true;
          i = it ->second;
	  break;
      }
      ++it;
   }
   if( inlinkStored )
   {
      i->putQuery(key);
   }
   else
   {
      Inlink* i2 = new Inlink(n);
      i2->putQuery(key);

      inlinks.insert(pair<BIGNUM*,Inlink*>
      ((n)->getNodeID(), i2));
      inlinks[n->getNodeID()] = i2;
   }
}



void TlcProtocol::checkTTL(BIGNUM* query, BIGNUM* urlsId)
{
   int query_TTL, last_TTL;
   BIGNUM* lastUrlId = urlsID.find(query)->second;

   //STALE ENTRY
   if(BN_cmp(urlsId, lastUrlId )!= 0)
   {
      // TTL to min
      query_TTL  = MIN_TTL;
      
      //Update URLs_ID
      urlsID.erase(urlsID.find(query));
      urlsID.insert(pair<BIGNUM*, BIGNUM*> (query, urlsId));
      
      //Update TTLs
      queryTTL.erase(queryTTL.find(query));
      queryTTL.insert(pair<BIGNUM*,int>(query, query_TTL) );

      //STILL FRESH ENTRY
      }else
      {
         last_TTL = queryTTL.find(query)->second;

	 //Estimate TTL using incremental Function F
	 //query_TTL = last_TTL + F(last_TTL);
	 
	 if(query_TTL > MAX_TTL)
	    query_TTL = MAX_TTL;
	 
	 //Update URLs_ID
	 urlsID.erase(urlsID.find(query));
	 urlsID.insert(pair<BIGNUM*, BIGNUM*> (query, urlsId));
	 
	 queryTTL.erase(queryTTL.find(query)); 
	 queryTTL.insert(pair<BIGNUM*,int>(query, query_TTL) ); 
   }
}



void TlcProtocol::freeMessage(TlcMessage * msg)
{
   free(msg->dest);
   free(msg);
}


void TlcProtocol::EndReplicator(){ (*replicator)->end();}

void TlcProtocol::checkWaitTimeOut()
{
   vector<Query*>::iterator at;
   at = waiting.begin();
   while(at!=waiting.end())
   {

   if((*at)->getTimeOut()< this->time() && !((*at)->getForceRouting()))
      {
         Query *q=(*at);
         q->renewTimeOut(this->time());
	 q->forceRouting();
	 this->add_query(q);
	 waiting.erase(at);
      }
      else
      {
         at++;
      }
   }
}

