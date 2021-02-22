#ifndef _NODE_ENTRY_H
#define _NODE_ENTRY_H
#include "../glob.h"

// Node Entry class encapsulates nodes information
// as nodeID and Ip address 
class NodeEntry 
{

private:

   int ip;
   BIGNUM* nodeID;

public:

   ~NodeEntry()
   {

   }

// Constructor
   NodeEntry(BIGNUM* _nodeID,int  _ip)
   {
      ip = _ip;
      nodeID = _nodeID;
   }

   int getIP()
   {
      return ip;
   }

   BIGNUM* getNodeID()
   {
      return nodeID;
   }

};
#endif
