#include "WSE.h"



void WSE::inner_body(void)
{
	srand(1);
   while(1)
   {

		if(queue_in.empty() )
			passivate();

		ASSERT(!queue_in.empty());

		//~ cout<<"WSE inner body"<<endl;
		MessageWSE* m = queue_in.front();

		queue_in.pop_front();

		switch ( m->source )
		{
			case PEER:
				//~ cout<<"WSE - CASE PEER - Send object to peer " << (*(m->src))->name() << ". Time: " << this->time()<<endl;
				//           m->setAnswer(WSE::getVersion(m->getQuery())); 
				m->setAnswer(WSE::getVersion(m->getKey(),m->getQuery()));
				(*(m->src))->receiveWSE(m);
				if( (*(m->src))->idle() && !(*(m->src))->get_busy() )
					(*(m->src))->activateAfter( current() );
				break;
			
			case USER:
				//chequea en la Rcache si esta la query, sino la inserta
				//	   m->setAnswer(WSE::getVersion(m->getQuery()));
				m->setAnswer(WSE::getVersion(m->getKey(),m->getQuery()));
				//~ cout<<"USER QUERY WSE"<<endl;
				break;

			default: cout<<"ERROR: WSE Type o msg"<<endl;
		}	    
	}
}

int WSE::getVersion(string s, BIGNUM* b)
{
	srand(1);
   int ttl;
   Answer* a= ANSWERS->check(s);
   if (a!=NULL)
   {
      if(a->getTimeOut()<this->time())
      {
         ttl = (this->time())+ TTL_WSE;
         // TTL es igual a new random number
         a->setNewVersion(ttl);
         //version +1
      }
   }
   else
   {
   // TTL es igual a new random number y version = 1
      ttl= (this->time()) + TTL_WSE;
      Answer *tmp=new Answer(this->time(), s, b,ttl,1,SIZE_ANSWER);
      a=tmp;
      ANSWERS->insertAnswer(tmp);
   }
   return a->getVersion();
}

long int WSE::randomTTL()
{
	srand(1);
   long int r = rand();
  
   return r;
}
