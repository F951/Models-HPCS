#include "Hash.h"



BIGNUM* Hash::CreateRandomKey()
{ 
   //create a key
   //srand48(time(NULL));		
	
   char buffer[SHA_DIGEST_LENGTH+1];
   unsigned long int temp_key = lrand48();
	
   SHA1((unsigned char *)&temp_key, 
        sizeof(unsigned long int), 
	(unsigned char*)&buffer[0]);
   BIGNUM *key = BN_new();
   BN_bin2bn( (unsigned char *)&buffer[0], 20, key ); // transform in Bignumber
										
   return key;   
}
	
	
BIGNUM* Hash::GenerateKey(char *str)
{
//create a key
   char buffer[SHA_DIGEST_LENGTH+1];
   BIGNUM *key = BN_new();
	
   SHA1((unsigned char *)str, sizeof(char*), (unsigned char*)&buffer[0]);
   BN_bin2bn( (unsigned char *)&buffer[0], 20, key ); // transform in Bignumber
	
   return key;
}
