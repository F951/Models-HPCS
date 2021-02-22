#include <math.h> 
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "ServerMessage.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const ServerMessage & msg) {
  //~ os << msg.packet << " " << msg.bit;
  return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, ServerMessage & msg) {
  //~ is >> msg.packet;
  //~ is >> msg.bit;
  return is;
}
