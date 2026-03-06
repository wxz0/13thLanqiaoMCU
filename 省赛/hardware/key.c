#include "key.h"

uint8_t key_getstate(void)
{
 uint8_t state = 0;
 	
 P35 = P34 = P32 = P33 = 1;
	
 P35 = 0;
 if(P33 == 0)state |= 0x01;
 if(P32 == 0)state |= 0x01<<1;
 P35 = 1;
	
 P34 = 0;
 if(P33 == 0)state |= 0x01<<2;
 if(P32 == 0)state |= 0x01<<3;
 P34 = 1;

 return state;	
}