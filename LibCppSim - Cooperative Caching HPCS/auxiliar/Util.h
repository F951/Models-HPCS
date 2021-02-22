#ifndef UTIL_H
#define UTIL_H

#include "../glob.h"
#include "../p2pLayer/NodeEntry.h"

class UTIL
{

public:
   
   static BIGNUM* EMPTY;
   static BIGNUM* MAX;
   static BIGNUM* MIN;
   static BIGNUM* BNZERO;
  
   UTIL();

   ~UTIL();

   static char* obtain_terms ( vector<string> t );

   static void Tokenize ( const string&,
                          vector<string>&,
                          const string& );

   static bool startWith( BIGNUM*, char );

   static bool startWith( const char*, char );

   static string truncateNodeId( NodeEntry* );

   static string truncateNodeId( BIGNUM* );

   static int chartoIndex( char );

   static char getDigit( int );
	
   static int prefixLen( BIGNUM*, NodeEntry* );
   
   static const char* myBN2HEX( BIGNUM* );
  
   static const char* myBN2HEX2( BIGNUM* );

   static int charSize(const char*);

   static bool hasDigitAt( BIGNUM*, int, char );

   static bool hasDigitAt( const char*, int, char );
};

#endif
