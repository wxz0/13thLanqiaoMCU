#include "key.h"

uint8_t key_getstate(void)
{
 uint8_t state = 0;
 	
 if(P33 == 0)state |= 0x01;
 if(P32 == 0)state |= 0x02;
 if(P31 == 0)state |= 0x04;
 if(P30 == 0)state |= 0x08;
	
 return state;	
}
