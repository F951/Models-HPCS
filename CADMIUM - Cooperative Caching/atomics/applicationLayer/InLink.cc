#include "InLink.h"

int Inlink::getFrequency( BIGNUM* key )
{
   map<BIGNUM*,int>::iterator it;
   //it = queries.find(key);
   //if(it!=queries.end())
   it = queries.begin();
   while(it != queries.end())
   {
      if(BN_cmp(key, it->first) == 0 )
	         return it->second; 
      ++it;
   }
   return 0;
}

void Inlink::resetQueries()
{
   queries.clear();
}

bool Inlink::isKeyContained(BIGNUM* b)
{ 
   map<BIGNUM*, int>:: iterator it;
   //it = queries.find(b);
   it = queries.begin();
  
  while(it != queries.end())
  {
    if(BN_cmp(b, it->first) == 0 )
      return true;
    ++it;
  }

  return false;
  
  //if(it== queries.end() )
  //    return false;
  // else
  //    return true;
}

void Inlink::putQuery(BIGNUM* key)
{
   map<BIGNUM*, int>:: iterator it;
   //bool flag = true;
  // it = queries.find(key);
  // if(it== queries.end() )
  if(isKeyContained(key))
  {
     
     it = queries.begin();
     while(it != queries.end())
     {
        if(BN_cmp(key, it->first) == 0 )
	{
          int temp = it->second;
	  temp = temp+1;
          //(*it).second = temp;
	  
	  queries.erase(it);
	  queries.insert(pair<BIGNUM*, int>(key, temp));
	  queries[key] = temp;
	 
	  return;
	  //queries[it->first]=temp;
	   //cout << "Rep: IT2 "<< temp << endl; 
	}
	it++;

     }
  }
  else
  {
     queries.insert(pair<BIGNUM*,int>(key,1));
     queries[key] = 1;
  //   it->second=1;
  }
  //    queries.insert(pair<BIGNUM*,int>(key,1));             
  // else 
  //     it->second = it->second+1;
}
