#ifndef UTIL_H
#define UTIL_H

//#include "../glob.h"
#include "../glob.h"
#include "../atomics/p2pLayer/NodeEntry.h"
//~ #include <openssl/bn.h>
//~ #include <openssl/md5.h>
//~ #include "../atomics/p2pLayer/NodeEntry.h"
//~ #include <string>
//~ #include <vector>
//~ #include <iostream>
//~ #include <map>
//~ #include <sstream>
//~ #include <set>
//~ #include <vector>
//~ #include <list>
//~ #include <assert.h>
//~ //#include <cassert>
//~ #include <fstream>
//~ #include <string>
//~ #include <limits>
//~ #include <queue>
//~ #include <math.h>
//~ #include <algorithm>
//~ #include <stdio.h>
//~ #include <stdlib.h>
//~ #include <time.h>
//~ #include <string.h>
//~ #include <openssl/sha.h>
//~ #include <time.h>

#define MAX_BUFFER 2048
#define BASE 16
#define BITS 160
#define B 4
#define D 10

using namespace std;


class UTIL
{

public:
   
   static BIGNUM* EMPTY;
   static BIGNUM* MAX;
   static BIGNUM* MIN;
   static BIGNUM* BNZERO;

   UTIL();

   ~UTIL();

   static char* obtain_terms(vector<string>t);

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
