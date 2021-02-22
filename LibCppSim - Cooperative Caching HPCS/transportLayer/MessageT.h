#ifndef _MESSAGET_h
#define _MESSAGET_h

// Messages that are sent by the transport layer are of this class.
// Messages on the upper layer are encapsulated within the DATA parameter

enum MessageTType { T_P2P = 0,
                    T_BDC = 1
		    };



class MessageT
{

private:


   int src; // index of the peer source of the message
   int dest; // index of the peer destination of the meesage
   void *DATA; // data transmited by this message

public:

   MessageTType type;

   MessageT(MessageTType _type, void* _D, int _src, int _dest)
   {
      src= _src;
      dest = _dest;
      DATA=_D;
      type=_type;
   }
   
   ~MessageT()
   {
//      free(DATA);
   }


   int getType()
   {
      return type;
   }

   void setType(MessageTType _t)
   {
      type=_t;
   }

   void * getData()
   {
      return DATA;
   }
   
   MessageT* clone()
   {
      MessageT* newMsg = new MessageT(this->type, this->DATA, this->src, this->dest);
      return newMsg;
   }


   int getDest()
   {
      return dest;
   }

   int getSrc()
   {
      return src;
   }
    
    
};
#endif
