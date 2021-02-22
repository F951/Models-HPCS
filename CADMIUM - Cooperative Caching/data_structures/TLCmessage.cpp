#include <math.h> 
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "TLCmessage.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const TLCMessage& msg) {
  //~ os << msg.packet << " " << msg.bit;
  return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, TLCMessage & msg) {
  //~ is >> msg.packet;
  //~ is >> msg.bit;
  return is;
}
