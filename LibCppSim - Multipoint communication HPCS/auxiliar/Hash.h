#ifndef _HASH_h
#define _HASH_h
#include "../glob.h"


class Hash
{
		
public:

   Hash()
   {
   }

   ~Hash()
   {
   }

   BIGNUM* CreateRandomKey();

   BIGNUM* GenerateKey(char*);
}
;
#endif
