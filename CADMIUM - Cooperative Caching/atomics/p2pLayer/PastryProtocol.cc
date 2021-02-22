#include "PastryProtocol.h"

void PastryProtocol::inner_body(void)
{	
	while(1)
	{
		//~ cout << "Pastry" <<  endl;
		
		while ( flag_terminar==1 ){
		//while ( (*observer)->get_end() ){
			passivate();
		}
		if(queue_in.empty())
		{
			passivate();
		}

		// cout<<"PastryProtocol "<<endl; fflush(stdout);
		/* Recovers messages sent from the lower layer */
		while( !queue_in_lower.empty() )
		{                  
			/* Recovers the PastryMessage within MessageT */
			MessageT *mt = (MessageT*)queue_in_lower.front();
			PastryMessage *pm = (PastryMessage*)mt->getData();
			pm->setDataType((DataType)mt->getDataType());
			queue_in_lower.pop_front();
			/* Maybe we have to change where the type is changed */
			if (pm->getType()!=5)
			{
				pm->setType(PASTRY_RECEIVE);
			}
			queue_in.push_back(pm);
			delete mt;
		}

		ASSERT(!queue_in.empty());
		m = queue_in.front();
		queue_in.pop_front();

		//const char * s = BN_bn2hex(NODEID);	
		//      cout <<"Pastry: this.NodeID "<< s << "- " << m->getType()<< endl;
		//free((char*)s);

		switch (m->getType())
		{			
			case PASTRY_SENDDIRECT:
			{			
				/* creates a message where to put PastryMessage */
				//    cout << "Pastry: send direct destNodeID: " ;
				//fflush(stdout);
				//const char* s= UTIL::myBN2HEX((m->getDestEntry())->getNodeID());
				//   cout    << s << endl;
				//free((char*)s);

				int src = ne->getIP(); // m->src;
				int dest = m->getDestEntry()->getIP();

				//Linea Agregada por Segmentation
				m->dest = ne -> getNodeID();

				MessageT *mt1 = new MessageT(T_P2P,(void*)m, src, dest);	  
				mt1->setDataType((DataType)m->getDataType());
				     
				(*transport)->add_message(mt1);
				if((*transport)->idle() && !(*transport)->get_busy())
					(*transport)->activateAfter(current());
				break;
			}

			case PASTRY_SENDDHT:
			{
				//const char* s= UTIL::myBN2HEX(m->dest);
				// cout << "Pastry: send DHT key:" << s << endl;
				//free((char*)s);

				int src = ne->getIP();

				//PUSH ON TRACK TRACE
				//m->addTrack(ne->getNodeID());


				//cout << "route 2 " << BN_bn2hex(m->dest) << endl;
				//   fflush(stdout);
				NodeEntry* nextStep = route(m->dest);

				//    cout << "NEXT STEP " << BN_bn2hex(nextStep->getNodeID()) << "-" << nextStep->getIP() << endl;

				if(BN_cmp(NODEID, nextStep->getNodeID())!=0)
				{
					int dest = nextStep->getIP();

					//Linea agregada para el path
					//m->addTrack(nextStep->getNodeID());
					// cout << "Pastry: Sending to transportLayer" << endl;	

					m->setDestEntry(nextStep);	
					MessageT *mt1 = new MessageT(T_P2P,(void*)m, src, dest);
					mt1->setDataType((DataType)m->getDataType());
					
					(*transport)->add_message(mt1);
					if((*transport)->idle() && !(*transport)->get_busy())
						(*transport)->activateAfter(current());
				}
				else 
				{
					//  cout << "Pastry: receive" << endl; 
					m->finalDestination=1;
					(*upperLayer)->deliver(m);
					if((*upperLayer)->idle() && !(*upperLayer)->get_busy())
						(*upperLayer)->activateAfter(current());
				}
				break;
			}

			case PASTRY_RECEIVE: 
			{	
				/* Send message to the upperLayer through the function deliver */
				// cout << "Pastry: receive" << endl;

				// cout << "route3" << BN_bn2hex(m->dest) << endl;
				// fflush(stdout);
				NodeEntry* nextStep = route(m->dest);
				if(BN_cmp(NODEID, nextStep->getNodeID())==0)
				{	
					m->finalDestination = 1;	
				}

				(*upperLayer)->deliver(m);
				if((*upperLayer)->idle() && !(*upperLayer)->get_busy())
				(*upperLayer)->activateAfter(current());

				break;
			}

			/****
			case PASTRY_BDC:
			{
				//cout << "Pastry: Broadcast" << endl;
				m->setType(PASTRY_BDC_RECEIVE);
				MessageT* mtb = new MessageT (T_BDC,(void*)m, 0, 0);
				mtb->setDataType((DataType)(27));
				(*transport)->add_message(mtb);

				if((*transport)->idle() && !(*transport)->get_busy())
					(*transport)->activateAfter(current());

				break;
			}
			case PASTRY_BDC_RECEIVE:
			{
				//cout << "Pastry: Receive Broadcast" << endl;
				m->finalDestination =1;

				(*upperLayer)->deliver(m);
				if((*upperLayer)->idle() && !(*upperLayer)->get_busy())
					(*upperLayer)->activateAfter(current());
				break;
			}
			*****/
			
			default:
				//cout << "Msg incorrecto" << endl; 
				fflush(stdout);
		}
	}
}

NodeEntry* PastryProtocol::route(BIGNUM* dest)
{	
   NodeEntry* nextHop = NULL;
   NodeEntry* nextHopI = NULL;
   NodeEntry* nextHopLS= NULL;
   NodeEntry * nextHopRT = NULL;

   nextHopI = this->ne;
   nextHopLS = leafset->checkLeafset(dest);
   nextHopRT = rt->checkRoutingTable(dest, ne);

   //IM THE RESPONSIBLE
   if( BN_cmp( nextHopI->getNodeID(),dest) == 0 )
      return nextHopI;

   //NO RT INFO -> CHECK LEAFSET
   if(nextHopRT == NULL)
      nextHop = nextHopLS;

   //CLOSEST AMONG RT AND LS
   if(nextHopRT != NULL && nextHopLS != NULL)
      nextHop =  checkClosest(dest, nextHopRT, nextHopLS);
	
   //CLOSEST AMONG ME AND OTHERS OPTIONS
   nextHop = checkClosest(dest, nextHopI, nextHop);

   //cout << "Pastry: KEY:" 
   //     << BN_bn2hex(dest)  
//	<< "-  NEXT HOP: "
//	<< BN_bn2hex(nextHop->getNodeID()) 
//	<< endl;

	return nextHop;
}


NodeEntry* PastryProtocol::checkClosest(BIGNUM* dest, NodeEntry *a, NodeEntry *b)
{
   BIGNUM* d1= leafset->absMinDist(dest,a->getNodeID());
   BIGNUM* d2= leafset->absMinDist(dest,b->getNodeID());

   if(BN_cmp(d1,d2)>0)
   {
      BN_free(d1);
      BN_free(d2);
      return b;    
   }
   else
   {
      BN_free(d1);
      BN_free(d2);
      return a;
   }
}



