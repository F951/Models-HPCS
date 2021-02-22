#include "Util.h"
#include <assert.h>

using namespace std;

BIGNUM* UTIL::EMPTY;
BIGNUM* UTIL::MAX;
BIGNUM* UTIL::MIN;
BIGNUM* UTIL::BNZERO;


UTIL::~UTIL()
{
BN_free(BNZERO);
BN_free(MAX);
}

UTIL::UTIL()
{
   EMPTY = NULL;
   //~ BN_init(&BNZERO);	
   //~ BN_zero(&BNZERO);
   BNZERO=BN_new();
   MIN=BNZERO;
   MAX = BN_new();
   BN_hex2bn(&MAX, "ffffffffffffffffffffffffffffffffffffffff");

}


void UTIL::Tokenize(const string& str,
                    vector<string>& tokens,
                    const string& delimiters = " ")
{
   string::size_type lastPos = str.find_first_not_of(delimiters, 0);
   string::size_type pos     = str.find_first_of(delimiters, lastPos);
   while (string::npos != pos || string::npos != lastPos)
   {
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delimiters, pos);
      pos = str.find_first_of(delimiters, lastPos);
   }
}


char* UTIL::obtain_terms( vector<string> t)
{
   char buffer[MAX_BUFFER];
   set<string> set_int;

   for( int i=1; i< (int)t.size(); i++ )
   {
      set_int.insert( t[i] ); //term
   }
   //~ ASSERT( !set_int.empty( ) );
   assert( !set_int.empty( ) );

   set<string>:: iterator itsi = set_int.begin( );
   sprintf( buffer, "%d", atoi( (*itsi).c_str() ) );
   itsi++;

   for( ; itsi != set_int.end( ); itsi++ )
   {
      int shift = strlen( buffer );
      sprintf( buffer + shift , "%d", atoi( (*itsi).c_str() ) );
      assert( strlen( buffer ) < MAX_BUFFER );
   }

   char *ret = (char*)malloc( strlen( buffer ) + 1 );
   sprintf( ret, "%s", buffer );

   return ret;
}

int UTIL::charSize(const char *c)
{
   int charCnt =0;
   while(*c != '\0')
   {
      charCnt++;       
      c++;
   }
   return charCnt;
}

int UTIL::prefixLen(BIGNUM* key, NodeEntry* n2)
{
   const char* s1 = UTIL::myBN2HEX(key);
   const char* s2 = UTIL::myBN2HEX(n2->getNodeID());
   int i = 0;
   
   for(int i = 0; i < UTIL::charSize(s1); i++)
   {
      if(s1[i] != s2[i])
      {
         //free((char*)s1);
	 //free((char*)s2);
         return i;
       }
   }
   //free((char*)s1);
   //free((char*)s2);
   return i;
}

bool UTIL::startWith(const char* hexTmp, char c)
{
      string ret (hexTmp, D);
      if(ret[0] == c)
      { 
         return true;
      }
	return false;
	}


bool UTIL::startWith(BIGNUM* k, char c)
{
   const char* hexTmp = UTIL::myBN2HEX(k);

   string ret (hexTmp, D);
   //	cout << ret[0] << endl;
   if(ret[0] == c)
   {
      //free((char*) hexTmp);
     // delete &ret;
      return true;
   }
   //free((char*)hexTmp);
   //delete &ret;
   return false;
}


int UTIL::chartoIndex(char c)
{
   switch (c)
   {
      case '0': return 0;
      case '1': return 1;
      case '2': return 2;
      case '3': return 3;
      case '4': return 4;
      case '5': return 5;
      case '6': return 6;
      case '7': return 7;
      case '8': return 8;
      case '9': return 9;
      
      case 'a': return 10;
      case 'A': return 10;
      case 'b': return 11;
      case 'B': return 11;
      case 'c': return 12;
      case 'C': return 12;
      case 'd': return 13;
      case 'D': return 13;
      case 'e': return 14;
      case 'E': return 14;
      case 'f': return 15;
      case 'F': return 15;
   } 
   
   return 0; 

}


string UTIL::truncateNodeId(NodeEntry* k)
{
	if(k == NULL)
		return "  null ";

	const char *hexTmp= myBN2HEX(k->getNodeID());
	string ret (hexTmp,6);
//	free((char*)hexTmp);
	ret.append("-");
	//cout << "debug " << ret << endl;	
	return ret;
}

string UTIL::truncateNodeId(BIGNUM* k)
{
        if(k == NULL)
		return "  null ";

        const char *hexTmp = UTIL::myBN2HEX(k);
	
	string ret (hexTmp,6);
//	free((char*)hexTmp);				
	ret.append("-");      
	return ret;
}  


const char* UTIL::myBN2HEX2(BIGNUM* k)

{
   //   const char * p;
   const char* s = BN_bn2hex(k);

   string temp2(s);
   //	    p=temp2.c_str();


   if( UTIL::charSize(s) < 40)
   {			   
      int dif = 40-UTIL::charSize(s);
      string temp (dif, '0');
      temp.append(s);
      //p=temp.c_str();
   }			
   //free((char*)s);

   return s;

}

const char* UTIL::myBN2HEX(BIGNUM* k)

{
   const char * p;
   const char* s = BN_bn2hex(k);
   
   string temp2(s);
   p=temp2.c_str();

	if( UTIL::charSize(s) < 40)
	{
		int dif = 40-UTIL::charSize(s);
		string temp (dif, '0');
		temp.append(s);
		p=temp.c_str();
	}
	free((char*)s);
	return p;

}


char UTIL::getDigit(int i)
{
	char DIGIT [] = {'0' ,'1' ,'2' ,'3' ,'4' ,'5' ,'6' ,'7' ,'8' ,'9' ,'A' ,'B' ,'C' ,'D' ,'E' , 'F'};
	return DIGIT[i];

}

bool UTIL::hasDigitAt(const char* s, int pos, char c)
{
    // const char* s = UTIL::myBN2HEX(b);

   if(s[pos] == c)
   {
	return true;
    }
    return false;
		
}

bool UTIL::hasDigitAt(BIGNUM* b, int pos, char c)
{
	const char* s = UTIL::myBN2HEX(b);
	
	if(s[pos] == c)
	{
	  // free((char*) s);
	return true;
        }
        //free((char*) s );
	return false;

}


