#include "Replicator.h"

void Replicator::inner_body(void)
{
	while(1)
	{
		if(ends || (*(*tlc)->observer)->get_end()){
			passivate();
		}
		//cout<<"Replicator" <<endl; fflush(stdout);
		
		//RESET
		(*tlc)-> resetQueue();
		(*tlc)-> inlinks.clear();
		(*tlc)-> RCACHE->reset();

		hold(DELTA_T);
		// cout << "Replicator Check..."<< endl;
		// overload = getQueueSize(tlc);
		overload = (*tlc)->overloaded;

		//Checks nodes load
		if( isOverloaded(tlc) )
		{  
			// Replication
			// 1) Sort Entries
			// 2) Select the most frequent
			// 3) Replicate based on strategy
			
			//cout << "Peer Overloaded!!" << endl;
			
			// Copy LocalCACHE and RCACHE entries to Vector entries
			retrieveEntries(tlc);
			// Sort by frequency  
			sort(entries.begin(), entries.end(), compare);
			//Select strategy and replicate
			replicate(Rep_Strategy);
		}

	}

}



double Replicator::getQueueSize(handle<TlcProtocol> *t)
{
	double size;

	// if(t == NULL)
	//cout<<"*T ES NULL"<< endl;
	size =  ( ((*t)->avg_queue)/DELTA_T);
	//cout << "REP - QUEUE SIZE: " << size << "-" << (*t)->avg_queue << " - " << DELTA_T << endl;fflush(stdout);
	
	return size;

}


bool Replicator::isOverloaded(handle<TlcProtocol> *t)
{
	//Si esta sobrecargado --> cola mayor a un tamano t
	if (overload > OVERLOAD_THRESHOLD)
		return true;
	return false;
}


int Replicator::getAvailableCapacity(handle<TlcProtocol> *t)
{
	if(getQueueSize(t) > OVERLOAD_THRESHOLD)
		return 0;
	return int (OVERLOAD_THRESHOLD - getQueueSize(t) );
}



void Replicator::retrieveEntries(handle<TlcProtocol> *t)
{
	// Copy the entries from LocalCACHE and RCACHE to Vector entries to be sorted

	//Inserting entries from RCACHE
	entries = (*t)->RCACHE->getEntries();
	//entries.insert(entries.end(), 0, (*t)->RCACHE->cache->getSize() );

	//Inserting entries from LocalCACHE
	//entries.insert(entries.end(), t->localCache->cache.begin(), t->localCache->cache.end());
}




void Replicator::replicate(int strategy)
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
		//TLC
		case 2: 
			replicateTLC();
			break;
	}
}


void Replicator::replicateLeafset()
{
	//Verificar linea
	vector<NodeEntry*> ls = (*(*tlc)->pastry)->leafset->listAllNodes();

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

	for(int i = 0 ; (unsigned) i < ls.size(); i++)
	{
		if(!(*tlc)->isReplicated(ls[i]->getNodeID(), e->hashValue))
		{
			//Send Replication message to leafset node
			//Entry e over nodes
			// cout << "REP - FOR" << endl;
			e->isReplicated = true;

			//cout << "REP- Tracking Replica"<< endl;
			//Guardamos quienes son las replicas?
			(*tlc)->trackReplica(ls[i]->getNodeID(), e->hashValue);

			//cout << "REP- Sending Replica"<<endl;
			//Send Replication Message
			(*tlc)->sendReplica( e->clone() , ls[i]);
		} 
	}
}


void Replicator::replicateBubble()
{
	// Sort vector entries by freq
	sort(entries.begin(), entries.end(), compare);

	vector< Entry*>::iterator it;
	it = entries.begin();
	//cout <<"----------------------ENTRIES------------------------------"<< endl;
	// while(it != entries.end())
	// {
	//   cout << BN_bn2hex((*it) ->hashValue)<<" - " <<(*it)->frequency << endl;
	//  ++it;
	// }

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

	it2 = (*tlc)->inlinks.begin();
	//cout << "INLINK - Size: " << (*tlc)->inlinks.size() << endl ; 
	// cout << "Entry: " << BN_bn2hex( e->hashValue) << endl;

	while(it2 != (*tlc)->inlinks.end())
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
	// cout <<"----------------------INLINKS------------------------------"<< endl;
	// while(it3 != list.end())
	// {
	//  cout << BN_bn2hex( ((*it3)->getInlinkID())->getNodeID() )<< " - " <<(*it3)->getFrequency(e->hashValue) << endl; 
	// ++it3;    
	// }


	unsigned int index = 0 ;
	// cout << "REP - list Size " << list.size() << endl;
	while(index < list.size())
	{
		if(!(*tlc)->isReplicated(list[index]->getInlinkID()->getNodeID(), e->hashValue)
		&& list[index]->getFrequency(e->hashValue) > 0)
		{
			e->isReplicated = true;
			//Guardamos quienes son las replicas
			(*tlc)->trackReplica((list[index]->getInlinkID())->getNodeID(), e->hashValue);

			// Replica Send
			(*tlc)->sendReplica( e->clone() , list[index]->getInlinkID());
		}
		index++;
	}  
}


void Replicator::set_tlc(handle<TlcProtocol> *t)
{
   tlc = t;
}


void Replicator::replicateTLC()
{
	// Sort vector entries by freq
	sort(entries.begin(), entries.end(), compare);

	//Most frequent query
	//Search for the first not replicated entry
	Entry *e = NULL;
	//int j = 0;

	if(entries.size()>0)
	{
		e=entries[0];
		// do
		// {
		//   e = entries[j];
		//	 j++;
		//    }while(entries.size() > (unsigned)j && e->isReplicated);

		//   if(e->isReplicated){
		//cout << "NO SE PUEDE REPLICAR MAS..."<<endl;  
		// return;
		//  }
	}else{
		//cout << "ENTRIES VACIAS" <<endl;
		return;
	}

	//Which inlinks forwards the entry e
	vector<Inlink*> list;   
	map<BIGNUM*, Inlink*>::iterator it;

	it = (*tlc)->inlinks.begin();
	while(it != (*tlc)->inlinks.end() )
	{
		if(( it-> second)->isKeyContained(e->hashValue) )
		{
			//terminar
			//push inlink to list 
			list.push_back(it->second );
		}
		++it;
	}
	sort( list.begin(), list.end(), sorter(e->hashValue));

	//int overload = getQueueSize(tlc); 

	//Overload limited to most frequent 
	if(overload > e->frequency)
	overload = e->frequency;


	int index = 0;
	//Cubrir la sobrecarga con la capacidad de los peers
	while(overload > 0 && (unsigned)index < list.size() )
	{
		//Si no esta replicado y la freq
		//if(!(*tlc)->isReplicated(list[index]->getInlinkID()->getNodeID(), e->hashValue) && list[index]->getFrequency(e->hashValue) > 0)
		if(list[index]->getFrequency(e->hashValue) > 0)
		{
			//   e->isReplicated = true;

			//Guardamos quienes son las replicas
			//(*tlc)->trackReplica(list[index]->getInlinkID()->getNodeID(), e->hashValue);
			//cout << "Replicator::replicateTLC- Sending Replica"<<endl;
			(*tlc)->sendReplica( e->clone() , list[index]->getInlinkID());

			overload = overload - list[index] ->getFrequency(e->hashValue);
		}
		index++;
	}
}

