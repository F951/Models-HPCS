#ifndef _INLINK_H
#define _INLINK_H

#include "../glob.h"
#include "../p2pLayer/NodeEntry.h"

class Inlink
{

   NodeEntry* inlinkId;
   map <BIGNUM*, int > queries;

public:

   NodeEntry* getInlinkID()
   {
      return inlinkId;
   }

   int getFrequency(BIGNUM* /* key*/ );

   void resetQueries();

   Inlink(NodeEntry* _inlink)
   {
   	inlinkId = _inlink;
   }

   bool isKeyContained(BIGNUM* /*key*/);

   void putQuery(BIGNUM* /* key */ );

};

#endif
