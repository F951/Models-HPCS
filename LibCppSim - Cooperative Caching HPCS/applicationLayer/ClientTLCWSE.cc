#include "ClientTLCWSE.h"
#include "TlcProtocol.h"

void ClientTlcWse::inner_body(void)
{
   while(1)
   {
		if(ClientWSE::queue_in_wse.empty())
			passivate();

		//   cout<<"ClientWSE "<<endl; fflush(stdout);

		ASSERT(!ClientWSE::queue_in_wse.empty());


		MessageWSE *temp;
		temp = ClientWSE::queue_in_wse.front();
		ClientWSE::queue_in_wse.pop_front();

		Entry *newEntry = new Entry(this->time(), temp->getKey(), temp->getQuery(),  temp->getVersion(), ENTRY_SIZE, getTTL(temp->getQuery()));

		TlcMessage *tm = new TlcMessage(TLC_WSE, (*tlc)->msgID, newEntry);
		(*tlc)->msgID=(*tlc)->msgID+1;
		(*tlc)->add_message(tm);
		if ((*tlc)->idle() && !(*tlc)->get_busy())         
			(*tlc)->activateAfter(current());

       //delete temp;
   }

}

long int ClientTlcWse::getTTL(BIGNUM* b)
{
   long int query_TTL;
   long int last_TTL;
   Entry* old = (*tlc)->RCACHE->check(b);
   if(old==NULL)
   {
      query_TTL = MIN_TTL;
   }
   else
   {
      last_TTL = old->getTimeOut();
      query_TTL = ClientTlcWse::getTimeIncremental(last_TTL);
      
   }
   return query_TTL;
}

long int ClientTlcWse::getTimeIncremental(long int last_TTL)
{
//   cout << " incremental" << endl;
    last_TTL= this->time() + last_TTL + STATIC_TIMEOUT;
    if((last_TTL-this->time())>MAX_TTL)
       last_TTL=this->time() + MAX_TTL;
    return last_TTL;
}

void ClientTlcWse::set_tlc(handle<TlcProtocol> *t)
{
  tlc =t;
}


void ClientTlcWse::sendWSEQuery(Query* q)
{
    MessageWSE* wseQuery = new MessageWSE(ClientTlcWse::get_ClientWSE_obj(), q->hashValue, q->term, PEER);

    (*wse)->add_request(wseQuery);
    if((*wse)->idle() && !(*wse)->get_busy())
      (*wse)->activateAfter(current());
}

