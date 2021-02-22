#include <math.h> 
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "PastryMessage.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const PastryMessage& msg) {
  //~ os << msg.src_Transport << " " << msg.dest_Transport;
  return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, PastryMessage& msg) {
  //~ is >> msg.src_Transport;
  //~ is >> msg.dest_Transport;
  return is;
}
