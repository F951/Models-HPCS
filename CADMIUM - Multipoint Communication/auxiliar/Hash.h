#ifndef _HASH_h
#define _HASH_h
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>


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
