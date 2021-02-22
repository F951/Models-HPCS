#include "TlcProtocol.h"
#include <iomanip>      // std::setprecision

void TlcProtocol::inner_body(void)
{

   while (1)
   {

      if(queue_in.empty())     
         passivate();
	 // recovers messages from the lower layer
      
      
     //cout<<"TLCProtocol "<<endl; fflush(stdout);


      if((*observer)->getIteracion() != iteracion)
      {
     //    inlinks.clear();
       //  RCACHE->reset();
         iteracion = (*observer)->getIteracion();
         checkWaitTimeOut();
      }
     

//cout<<"1:TLCProtocol "<<endl; fflush(stdout);


		while(!queue_in_lower.empty())
		{
			PastryMessage *temp1 = (PastryMessage*)queue_in_lower.front();
			queue_in_lower.pop_front();

			TlcMessage *temp =(TlcMessage*)temp1->getData();

			if(temp1->finalDestination)
			temp->setFinalDest();

			if(temp1->getType()!=5)
			delete temp1;

			queue_in.push_back(temp);

		}


      ASSERT(!queue_in.empty());

      TlcMessage* tm = queue_in.front();
      queue_in.pop_front(); 


      switch ((int)Rep_Strategy)
		//~ switch (0)
      {
          case (0):
	  {

	     LeafsetBubble(tm);
	     break;
	  }
	  case (1):
	  {

	     LeafsetBubble(tm);
	     break;
	  }
	  case (2):
	  {   

	     TlcAlgorithm(tm);
	     break;
	  }
	  default:
	  {
             cout << "default " << Rep_Strategy << endl;
	  }
       }

    }
}

void TlcProtocol::TlcAlgorithm(TlcMessage * tm)
{
   Entry *e;
   LCache *lc;
   Query *q;
   
   NodeEntry *lcPeer;
	
	
	
	switch (tm->getType())
	{
		case TLC_LC_ACK:
		{
			cout<<"TLC_LC_ACK"<<endl;
			lc = tm->getLCache();
			lcack.push_back(lc);
			delete tm;
			break;
		}
		
		case TLC_WSE:
		{
			cout<<"TLC_WSE"<<endl;
			e= tm->getEntry();
			//q=tm->getQuery(); 
			//cout << "TLC: INSERT GLOBAL "<< UTIL::truncateNodeId(pid) << " - key " << UTIL::truncateNodeId(e->hashValue) << endl;

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

					if (tmp->getPathSize()>=2)
					{
					if(BN_cmp(((*pastry)->ne)->getNodeID(),(tmp->src)->getNodeID())!=0)
						storeInlink(e->hashValue, tmp->path[tmp->getPathSize()-2]);
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
			//~ cout<<"TLC_LOOKUP"<<endl;
			q = tm->getQuery();
			// cout << "Tlc: lookup - key:" << UTIL::truncateNodeId(q->hashValue)    << " " << q->id << endl;
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
				// cout << "hop2: " << tm->path.size() << endl;
				(*observer)->addHops(tm->path.size()-1);
				//printPath(tm);   
				//Calculate average queue size
				//setAvgQueue();
			}
			// check global cache entries               
			//~ cout<<"TLC_LOOKUP - CHECK RCACHE"<<endl;
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
				//if(e!=NULL)
				// cout << "TTL: "<< q->id <<  endl;
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
					(*observer)->addHitLocal();
					// send reply message and update entry
					sendReply(e->clone(),tm);
					localCache->update(e->hashValue);
				}
				else 
				{
					// check Lcache
					lc = LCACHE->check(q->hashValue);
					// if found add src to Lcache if expiration is ok
					if ( lc!=NULL)
					{
						lcPeer = lc->getDonante(q->src, (double)this->time() );
						if ( lcPeer !=NULL && lcPeer != q->src)//if ( lc !=NULL && lc->getLocation()!=q->src)
						{
							(*observer)->addHitLCache();
							lc->addNewLocation(lcPeer,this->time());
							sendLCache(lc->clone(),tm);
						}
						else goto GOSALIDA;           
					}
					else
					{
						GOSALIDA:
						//~ cout<<"TLC_LOOKUP - GO SALIDA"<<endl;
						vector<Query*>::iterator ite;
						ite=waiting.begin();
						bool wait=false;
						bool force=false;
						if (tm->getForceRouting())
						{
							force=true;
						}
						//~ cout<<"TLC_LOOKUP - Check waiting"<<endl;
						while(ite!=waiting.end() && !wait && !force)
						{
							if( BN_cmp(q->hashValue, (*ite)->hashValue)==0)
							{
								(*observer)->addHitLocal();
								//cout << "hop3: " << tm->path.size() << endl;
								(*observer)->addHops(tm->path.size()-1);
								//printPath(tm);
								//cout << "outstanding q:" << q->id << " wait " << (*ite)->id << endl;
								outstanding.push_back(tm);
								wait=true;
								break;
							}
							ite++;
						}
						if(!wait)
						{
							//~ cout<<"TLC_LOOKUP - Continue routing"<<endl;
							continueRouting(tm);
						}
						if (tm->getPathSize() == 2  )
						{ 
							//cout<< "Insert Lcache : << " << BN_bn2hex(q->hashValue) << " id:" << q->id << " src: " << BN_bn2hex(q->src->getNodeID()) << endl;
							//NodeEntry* nextStep = (*pastry)->route(q->hashValue);
							//~ cout<<"TLC_LOOKUP - Create amortiguador"<<endl;
							if(q->src!=(*pastry)->ne)
							{
								cout<<"TLC_LOOKUP - src!=ne"<<endl;
								//cout << "route1" << BN_bn2hex(q->hashValue) << endl;
								//fflush(stdout);
								//~ NodeEntry* nextStep = (*pastry)->route(q->hashValue);
								cout<<"TLC_LOOKUP - NextStepOK"<<endl;
								LCache *temp = new LCache(q->term, 
								q->hashValue,
								q->src,
								(*pastry)->route(q->hashValue),
								this->time());
								cout<<"TLC_LOOKUP - Lcache entry ready"<<endl;
								temp->CreateAmortiguador(Thao);
								cout<<"TLC_LOOKUP - Amortiguador ready"<<endl;
								temp->getDonante(q->src, (double)this->time());
								cout<<"TLC_LOOKUP - Insert amortiguador in Lcache"<<endl;
								LCACHE->insertLCache(temp);
								cout<<"TLC_LOOKUP - Insert OK"<<endl;
							}
							cout<<"TLC_LOOKUP - src==ne"<<endl;
						}
						
					}
					cout<<"TLC_LOOKUP - Put query on waiting"<<endl;
					if (tm->getPathSize()==1)
					{
						//cout << "Inserting in waiting path==1" << endl;		     
						waiting.push_back(tm->query);
					}
				}   
			}
			cout<<"EXIT TLC_LOOKUP"<<endl;
			break;
		}    
		
		case TLC_ENDS:
		{
			cout<<"TLC_ENDS"<<endl;
			//cout << "TLC_ENDS" << nFin << "-" << (*pastry)->ne->getIP() << endl;
			nFin++;
			if (nFin == NP)
			{
				//cout << "TLC_ENDS FINAL" << nFin << "-" << NP << endl;
				(*replicator)->end();
				(*observer)->end();
			}
			//cout << "T: " << terminadas << " end[NP]: " << ends[NP] << " ends[i] "  << ends[((*pastry)->ne)->getIP()] << endl;
			if (ends[NP]==1 && !Broadcast)
			{
				if(0==ends[(*pastry)->ne->getIP()] || terminadas==ends[(*pastry)->ne->getIP()] )
				{ 
					//		     cout << "send BCD -" << (*pastry)->ne->getIP() << endl;
					Broadcast=true;
					sendEndBroadcast();
				}			
			}
			break;
		}

		case TLC_REPLY:
		{    
			cout<<"TLC_REPLY"<<endl;
			e = tm->getEntry();
			q= tm->getQuery(); 
			//    if(e == NULL)
			//cout << "TLC: Entry received NULL" << endl;
			//cout << "TLC: INSERT LOCAL "<< UTIL::truncateNodeId(pid) << " - key " << UTIL::truncateNodeId(e->hashValue) << endl;
			// save in localCache
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
					//(*observer)->addHitLocal();
					//cout << "TLC: REPLY OUTSTANDING"<< ((*it1)->getQuery())->id << endl;
					TlcMessage* tmp = *it1;
					sendReply(e->clone(),tmp);
					outstanding.erase(it1); 
				}
				else
				{
					it1++;
				}
			}
			vector<LCache*>::iterator ilc;
			ilc= lcack.begin();
			while(ilc!=lcack.end())
			{
				if(BN_cmp(e->hashValue, (*ilc)->hashValue)==0)
					lcack.erase(ilc);
				else
					ilc++;
			}

			//Query *q = tm->getQuery();
			//  cout << "Arriving Query " << q->id <<"/" << this->time()  << endl;
			(*observer)->addNQueriesIn();
			(*observer)->addLatencia((double)((this->time())-(q->t_start)));
			terminadas++;
			CTE_trabajo = work_time->value();
			//~ cout << "CTE_trabajo= "<< CTE_trabajo << endl;
			busy = true;
			hold(CTE_trabajo);
			busy = false;
			
			//cout << "T: " << terminadas << " end[NP]: " << ends[NP] << " ends[i] "  << ends[((*pastry)->ne)->getIP()] << endl;
			if (ends[NP]==1)
			{
				if(terminadas==ends[(*pastry)->ne->getIP()])
				{
					//	    cout << "send BDC -" << (*pastry)->ne->getIP() << endl;
					Broadcast=true;
					sendEndBroadcast();
				}
			}
			delete tm->getQuery();
			delete tm;
			break;
		} 

		case TLC_LCACHE:
		{
			cout<<"TLC_LCACHE"<<endl;
			//ADD TO PATH
			if((tm->path).size()>0)
			{
				if(BN_cmp(((*pastry)->ne)->getNodeID(),(tm->path[(tm->path).size()-1])->getNodeID())!=0)
					tm -> addPath((*pastry)->ne);
			}
			else	
			{		
				tm -> addPath((*pastry)->ne);
			}
			//tm -> addPath((*pastry)->ne);
			lc = tm->getLCache();
			q = tm->getQuery(); 
			//cout << "TLC: LCACHE RECEIVED -" << q->id << " key " << BN_bn2hex(q->hashValue)<< endl;
			Entry* tmp = localCache->check(q->hashValue);
			if(tmp == NULL || tmp->old(this->time()) )
			{
				//cout << "not in local cache?" << endl;
				vector<Query*>::iterator at;
				bool wait = false;
				at = waiting.begin();
				while(at!=waiting.end())
				{
					if(BN_cmp((*at)->hashValue, q->hashValue)==0)
					{
						//cout << "ID wait"<< (*at)->id  << endl;
						wait=true;
						break;
					}
					at++;
				}			   
				bool isLcack= false;
				vector<LCache*>::iterator ilc;
				ilc= lcack.begin();
				while(ilc!=lcack.end())
				{
				    if(BN_cmp(q->hashValue, (*ilc)->hashValue)==0)
					{   
						isLcack=true;
						break;
					}
					ilc++;
				}
				bool isLack2=false;
				LCache* lc_temp = LCACHE->check(q->hashValue);
				if (lc_temp!=NULL)
				{
					if (BN_cmp(((lc_temp->getLocation())->getNodeID()), (q->src)->getNodeID())==0)
					{
						isLack2=true;
					}
				}
				if(wait && !isLcack && !isLack2)
				{
					(*observer)->addHitLocal();
					//		  cout << "Hops:" << tm->path.size() << endl;
					(*observer)->addHops(tm->path.size()-1);
					tm->setType(TLC_LOOKUP);
					outstanding.push_back( tm);
					//printPath(tm);
				}
				else // 
				{
					continueRouting(tm, lc->getNextStep());
					delete lc;
				}
			}
			else 
			{
				//cout << "replying" << endl;
				(*observer)->addHitLocal();
				//	        cout << "Local 2-" << q->id << endl;
				sendReply(tmp->clone(), tm);
				   localCache->update(tmp->hashValue);
			}
			//delete lc;
			break;

		}	
		
		case TLC_REPLICATE:
		{
			cout<<"TLC_REPLICATE"<<endl;
			// add entry to replicaCache
			//cout << "TLC-REPLICATE Received!!"<<endl;	
			e = tm->getEntry();
			//if(e->hashValue == NULL)
			//cout << "ENTRY NULL!!"<<endl;
			//cout << "TLC-REPLICATE recovered!! " << BN_bn2hex(e->hashValue) <<endl;
			(*client)->sendWSEQuery( new Query(0, (char*)(e->key).c_str(),BN_dup( e->hashValue), this->time()) );
			delete tm;
			//freeMessage(tm);
			break;
		}
	}
}

void TlcProtocol::sendEndBroadcast(){

   //~ TlcMessage *EB= new TlcMessage(TLC_ENDS, msgID);

   //~ PastryMessage *pm = new PastryMessage(PASTRY_BDC, EB);

   //~ (*pastry)->add_message(pm);
   //~ if((*pastry)->idle() && !(*pastry)->get_busy())
      //~ (*pastry)->activateAfter(current());
}


void TlcProtocol::printPath(TlcMessage* tm)
{
	Query* q  = tm->getQuery();
	cout<< "--------- PATH for " << BN_bn2dec(q->hashValue) << " ---------"<<endl;
	for(unsigned int k = 0; k < tm->path.size(); k++){
		cout << k << " -> " <<  BN_bn2dec((tm->path[k])->getNodeID() )  <<endl;
	}
	cout<<"--------------------------------------------------------------------"<<endl;    
}




// TODO
void TlcProtocol::continueRouting(TlcMessage* tm, NodeEntry* neD)
{
    //cout << "Tlc: continueRouting LCACHE " << endl;
    tm->setType(TLC_LOOKUP);
    tm->dest = (tm->getQuery())->hashValue;

    PastryMessage *pm = new PastryMessage(PASTRY_SENDDIRECT,(*pastry)->ne, neD, tm);
       
    (*pastry)->add_message(pm);
       
    if((*pastry)->idle() && !(*pastry)->get_busy())
       (*pastry)->activateAfter(current());
      
   
}


void TlcProtocol::continueRouting(TlcMessage* tm)
{
 //cout << "Tlc: continueRouting " << endl;

   PastryMessage *pm = new PastryMessage(PASTRY_SENDDHT,(*pastry)->ne,tm->dest, tm);  
   
   (*pastry)->add_message(pm);
 
   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());

}


void TlcProtocol::sendLCache(LCache* lc, TlcMessage* tm)
{
    
   TlcMessage* reply = new TlcMessage(TLC_LCACHE, msgID, (*pastry)->ne, (lc->getLocation()));

   msgID = msgID +1;
   reply->setLCache(lc->clone());
   reply->setQuery(tm->getQuery());
   reply->setPath(tm->path);
   
   PastryMessage *pm = new PastryMessage( PASTRY_SENDDIRECT,
                                          (*pastry)->ne,
					  lc->getLocation(), 
					  reply);
   (*pastry)->add_message(pm);

   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());
  // delete tm;
  
   TlcMessage* lc_ack = new TlcMessage(TLC_LC_ACK, msgID, (*pastry)->ne,(tm->getQuery())->src );

      msgID = msgID +1;
      lc_ack->setLCache(lc->clone());
      lc_ack->setQuery(tm->getQuery());
      lc_ack->setPath(tm->path);

      PastryMessage *pm2 = new PastryMessage( PASTRY_SENDDIRECT,
		                               (*pastry)->ne,
					       (tm->getQuery())->src,
					        lc_ack);
	(*pastry)->add_message(pm2);

	if((*pastry)->idle() && !(*pastry)->get_busy())
	(*pastry)->activateAfter(current());

}

void TlcProtocol::sendReply(Entry* _e, TlcMessage* tm)
{

   setAvgQueue();
   overloaded++;
  // (*observer)->addNQueries();
   (*observer)->addLoad(((*pastry)->ne)->getIP());
	//~ cout << "###################### Añadir carga, peer ->: " << ((*pastry)->ne)->getIP() << endl;
   Query * q= tm->getQuery();
   //cout << "TlcReply: " <<BN_bn2hex((q->src)->getNodeID())<< endl;
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

void TlcProtocol::sendReplica(Entry* e ,NodeEntry* nodeEntry)
{
   TlcMessage* replica = new TlcMessage(TLC_REPLICATE, msgID, (*pastry)->ne, nodeEntry);
   msgID = msgID+1;
   replica->setEntry(e);
   (*observer)->addReplicas();
   PastryMessage *pm = new PastryMessage (PASTRY_SENDDIRECT, (*pastry)->ne, nodeEntry, replica);
   (*pastry)->add_message(pm);

   if((*pastry)->idle() && !(*pastry)->get_busy())
      (*pastry)->activateAfter(current());
}



void TlcProtocol::setAvgQueue()
{
   t_act = this->time() ;
   //cout << "TLC: T-actual "<< t_act<< endl;
   //cout << "TLC: avg_queue " << avg_queue<<endl;
   //fflush(stdout);

   avg_queue = avg_queue + ( (queue_in.size() + 1) *(t_act - t_pas) );
   t_pas = t_act;
  // cout << "TLC: AVG_QUEUE " << avg_queue << endl;
  // fflush(stdout);
}

void TlcProtocol::resetQueue()
{
   avg_queue = 0;
   overloaded=0;
}

bool TlcProtocol::isReplicated(BIGNUM* nodeId, BIGNUM* key)
{
   map<BIGNUM*, Replicas*>::iterator it;
   it = replicaTrack.begin();
  // cout << "TLC-IsReplicated?"<< endl;
   
   while(it != replicaTrack.end())
   {
      if( BN_cmp(it->first,nodeId) == 0 && (it->second)->isContained(key)  )
      {  
         //cout << "Esta Replicado" << endl;
	 return true;
      }
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
      {
		return false;  
	  }
      ++it;
   }

   return true;
}




void TlcProtocol::trackReplica(BIGNUM* nodeId, BIGNUM* key)
{
  // bool flag = isReplicated(nodeId, key);
  bool flag = isNewReplica(nodeId);

  //DEBUG
  /*
  cout <<"TLC - Replica Track " <<BN_bn2hex(nodeId) << " "<<BN_bn2hex(key)<< endl;  
  map<BIGNUM*, Replicas*>::iterator it;
  it = replicaTrack.begin();
  cout << "------------ANTES-------------" << endl;
  while(it != replicaTrack.end())
  {
    (it->second)->print();
    //cout <<BN_bn2hex(it->first) << " "<<BN_bn2hex(it->second)<< endl;
    ++it;
  }
  cout <<"--------------------------"<<endl;
  */
  
  //No esta
  if(flag)
  {
       Replicas* r = new Replicas(key ,nodeId);
       replicaTrack.insert(pair <BIGNUM*, Replicas*>(nodeId, r) );
       replicaTrack[nodeId] = r;

       (*observer)->addReplicas();

   //Existe hay q revisar
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


 //DEBUG  
  /*
  cout <<"TLC - Replica Track "<<BN_bn2hex(nodeId) << " "<<BN_bn2hex(key)<< endl;
   map<BIGNUM*, Replicas*>::iterator it2;
   it2 = replicaTrack.begin();
   cout << "------------DESPUES-------------" << endl;
   while(it2 != replicaTrack.end() )
   {
      (it2->second)->print();
      //cout <<BN_bn2hex(it2->first) << " "<<BN_bn2hex(it2->second)<< endl;
      ++it2;
   }
   cout <<"--------------------------"<<endl;
*/
}


void TlcProtocol::storeInlink(BIGNUM* key, NodeEntry* n)
{
   bool inlinkStored = false;
   Inlink* i;
   map<BIGNUM*,Inlink*>::iterator it;
   it = inlinks.begin();

  // cout <<"-------------------------- I -------------" << endl;
   while(it != inlinks.end())
   {
      //cout<< BN_bn2hex(it -> first)<< endl;
      if(BN_cmp(it->first, n->getNodeID())==0)
      {
	  inlinkStored = true;
          i = it ->second;
	  break;
      }
      ++it;
   }
   //cout << "Storing Inlink -> "<<  inlinkStored <<endl;
   //Existe Inlink
   if( inlinkStored )
   {
      //cout <<"Put Query "<< BN_bn2hex(key)<< endl;
      //Consulta llega por ese inlink?
      i->putQuery(key);

      //Inlink no existe, se inserta y se inserta la query
   }
   else
   {
      //cout <<"New Inlink " << BN_bn2hex(n -> getNodeID() )<< endl;
      Inlink* i2 = new Inlink(n);
      i2->putQuery(key);

      inlinks.insert(pair<BIGNUM*,Inlink*>
      ((n)->getNodeID(), i2));
      inlinks[n->getNodeID()] = i2;
   }
}
/*

//TODO:
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

*/

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
         Query *q=(*at);// new Query(at->id,at->term,at->hashValue,0.0);
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

void TlcProtocol::LeafsetBubble(TlcMessage *tm)
{

   Entry *e;
   Query *q;
	load_control();
	//~ cout << std::setprecision(10) << "TLC Node: " << name() << "  - time: " << this->time()<< endl;
   switch (tm->getType())
   {
					case TLC_WSE:
						{
						e= tm->getEntry();
						//q=tm->getQuery();
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

								if (tmp->getPathSize()>=2)
								{
									if(BN_cmp(((*pastry)->ne)->getNodeID(),(tmp->src)->getNodeID())!=0)
										storeInlink(e->hashValue, tmp->path[tmp->getPathSize()-2]);
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
			//~ cout << "==========================================================================================================" << endl;
			//~ std::cout << "TLC Protocol " << (*pastry)->idTransporte << " -  Process MSG. MSG Type:" << "LOOKUP"  << std::endl;
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
			//~ printPath(tm);
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

			else if( ( tm->getFinalDest()))
			{
				outstanding.push_back(tm);
				(*observer)->addMiss();
				(*client)->sendWSEQuery(q);
			}
			else if (e!=NULL && e->old(this->time()))
			{
				switch ((int)Case_Responsable)
				{
					 case (0):
					{
						(*observer)->addMiss();
						RCACHE->remove(e);
						continueRouting(tm);
						break;
					}
					case (1):
					{
						outstanding.push_back(tm);
						(*observer)->addMiss();
						(*client)->sendWSEQuery(q);
						break;
					}
				}
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
					(*observer)->addHitLocal();
					sendReply(e->clone(),tm);
					localCache->update(e->hashValue);
				}
				else
				{
					// vector<Query*>::iterator ite;
					//  ite=waiting.begin();
					//  bool wait=false;
					//  while(ite!=waiting.end() && !wait)
					//  {
					//   if( BN_cmp(q->hashValue, (*ite)->hashValue)==0)
					//   {
					//      (*observer)->addHitLocal();
					//      (*observer)->addHops(tm->path.size()-1);
					  //      outstanding.push_back(tm);
					//      wait=true;
					//      break;
					//   }
					//   ite++;
					//  }
					//  if(!wait )
					//  {
					 continueRouting(tm);
					//  }
				}
				if (tm->getPathSize()==1)
				{
					waiting.push_back(tm->query);
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
         //~ cout << " QUERY END in TLC - TIME: " << this->time() << endl;
	  (*observer)->addLatencia((double)((this->time())-(q->t_start)));
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
         switch((int)Case_Responsable)
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
            default:
           { 
            cout << "default Tlc rep" << endl;
             break;
           }
         }
         delete tm;
         break;

      }
     default:
        cout << "error LeafsetBubble" << endl;
        break;
   }
}

//*****************************************************Funciones REPLICATOR

  
	
void TlcProtocol::load_control(void)		//Ex inner body
{
	int debug=0;
	if(debug){cout << "======================================================================================================================================" << endl;}
	if(debug){cout << "LOAD CONTROL" << endl;}
	if ( ( this->time() - start_Control_time ) < Control_TIME ){	//No hacer el control antes de tiempo.
		if(debug){cout << "Still not control Time." << endl;}
		return;
	}
	if(debug){cout << "Control load." << endl;}
	start_Control_time = this->time();
	overload = overloaded;		//En overloaded tengo que acumular la carga en relación a las consultas.
	//~ if(debug){cout << "Peer is overloaded? " << overloaded << endl;}

	//Checks nodes load
	if( isOverloaded() )
	{  
		// Replication
		// 1) Sort Entries
		// 2) Select the most frequent
		// 3) Replicate based on strategy
		if(debug){cout << "Peer Overloaded!!" << endl;}
		// Copy LocalCACHE and RCACHE entries to Vector entries
		retrieveEntries();
		// Sort by frequency  
		sort(entries.begin(), entries.end(), compare);
		//Select strategy and replicate
		replicate(Rep_Strategy);
		//RESET
		resetQueue();
		inlinks.clear();
		RCACHE->reset();
	}
	
}

double TlcProtocol::getQueueSize(){
	double size;
	size =  ( avg_queue/DELTA_T);		// En avg_queue tengo que almacenar el tamaño promedio
	return size;
}

bool TlcProtocol::isOverloaded(){
	//Si esta sobrecargado --> cola mayor a un tamano t
	if (overload > OVERLOAD_THRESHOLD)
		return true;
	return false;
}

int TlcProtocol::getAvailableCapacity()
{
	if(getQueueSize() > OVERLOAD_THRESHOLD)
		return 0;
	return int (OVERLOAD_THRESHOLD - getQueueSize() );
}

void TlcProtocol::retrieveEntries(){
	//Copy the entries from RCACHE to Vector entries to be sorted
	entries = RCACHE->getEntries();			
}

void TlcProtocol::replicate(int strategy)
{
	switch (strategy)
	{
		//LEAFSET
		case 0:
			replicateLeafset();
			break;
		//BUBBLE
		case 1:
			replicateBubble();
			break;
	}
}


void TlcProtocol::replicateLeafset()
{
	int debug=0;
	
	//Verificar linea
	//~ vector<NodeEntry*> ls;
	vector<NodeEntry*> ls = (*pastry)->leafset->listAllNodes();

	Entry *e = NULL;
	int j = 0;

	//Sort vector by Freq
	sort(entries.begin(), entries.end(), compare);
	//cout << "REP - SORTING"<< endl;

	vector< Entry*>::iterator it;
	it = entries.begin();
	//cout <<"------------------------------------------------------"<< endl;
	//while(it != entries.end())
	//{
	//   cout << BN_bn2hex((*it) ->hashValue)<<" - " <<(*it)->frequency << endl;
	//  ++it;
	//}
	
	//Search for the first not replicated entry
	if(entries.size()>0){
		do{
			e = entries[j];
			j++;
		}while(entries.size() > (unsigned)j && e->isReplicated);
		if(e->isReplicated){
			//cout << "NO SE PUEDE REPLICAR MAS..."<< BN_bn2hex((*tlc)->pid) <<endl;
			return;
		}
	}else{
		//cout << "ENTRIES VACIAS" << BN_bn2hex((*tlc)->pid) << endl;
		return;
	}

	//cout << "REP- Entry " << BN_bn2hex(e->hashValue ) <<" - " << e->frequency<< endl;
	if(debug){cout << "REP- Entry " << BN_bn2dec(e->hashValue ) <<" - " << e->frequency<< endl;}

	for(int i = 0 ; (unsigned) i < ls.size(); i++)
	{
		if(!isReplicated(ls[i]->getNodeID(), e->hashValue))
		{
			//Send Replication message to leafset node
			//Entry e over nodes
			// cout << "REP - FOR" << endl;
			e->isReplicated = true;

			if(debug){cout << "REP- Tracking Replica"<< endl;}
			//Guardamos quienes son las replicas?
			trackReplica(ls[i]->getNodeID(), e->hashValue);

			if(debug){cout << "REP- Sending Replica"<<endl;}
			//Send Replication Message
			sendReplica( e->clone() , ls[i]);
		} 
	}
}


void TlcProtocol::replicateBubble()
{
	int debug=0;
	
	// Sort vector entries by freq
	sort(entries.begin(), entries.end(), compare);

	vector< Entry*>::iterator it;
	it = entries.begin();
	if(debug){cout <<"----------------------ENTRIES------------------------------"<< endl;
		 while(it != entries.end())
		 {
		  if(debug){cout << BN_bn2hex((*it) ->hashValue)<<" - " <<(*it)->frequency << endl;}
		  ++it;
		 }
	}
	//Most frequent query
	//Search for the first not replicated entry
	Entry *e = NULL;
	int j = 0;
	if(entries.size()>0){
		do{
			e = entries[j];
			j++;
		}while(entries.size() > (unsigned) j && e->isReplicated);

		if(e->isReplicated){
			//cout << "NO SE PUEDE REPLICAR MAS..."<<endl;
			return;
		}				  
	}else{
		//cout <<"ENTRIES VACIAS!!"<< endl;
		return;
	}

	// if(e==NULL)
	//  cout << "ENTRY NULL!!" <<endl;

	//Which inlinks forwards the entry e
	// list ??? inlinks
	vector<Inlink*> list;
	map<BIGNUM*, Inlink*>::iterator it2;

	it2 = inlinks.begin();
	//cout << "INLINK - Size: " << (*tlc)->inlinks.size() << endl ; 
	// cout << "Entry: " << BN_bn2hex( e->hashValue) << endl;

	while(it2 != inlinks.end())
	{
		//  cout<< "Inlink : "<< BN_bn2hex(((it2->second)->getInlinkID())->getNodeID() )<< endl;

		if(( it2-> second)->isKeyContained(e->hashValue) )
		{
			//terminar
			//push inlink to list
			//	cout << "Entry Contained!!" << endl;
			list.push_back(it2->second );
		}
		++it2;
	}
	sort( list.begin(), list.end(), sorter(e->hashValue));
	
	//DEBUG
	vector< Inlink*>::iterator it3;
	it3 = list.begin();
	if(debug){cout <<"----------------------INLINKS------------------------------"<< endl;
		while(it3 != list.end())
		{
			 {cout << BN_bn2hex( ((*it3)->getInlinkID())->getNodeID() )<< " - " <<(*it3)->getFrequency(e->hashValue) << endl; }
			++it3;    
		}
	}
	unsigned int index = 0 ;
	// cout << "REP - list Size " << list.size() << endl;
	while(index < list.size())
	{
		if(!isReplicated(list[index]->getInlinkID()->getNodeID(), e->hashValue)
		&& list[index]->getFrequency(e->hashValue) > 0)
		{
			e->isReplicated = true;
			//Guardamos quienes son las replicas
			trackReplica((list[index]->getInlinkID())->getNodeID(), e->hashValue);

			// Replica Send
			sendReplica( e->clone() , list[index]->getInlinkID());
		}
		index++;
	}  
}


