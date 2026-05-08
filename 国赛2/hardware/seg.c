#include "seg.h"

pdata uint8_t arr[] = {0xc0,  0xf9,  0xa4,  0xb0,// 0 1 2 3  
                       0x99,  0x92,  0x82,  0xf8,// 4 5 6 7  
											 0x80,  0x90,  0xff,  0x88,//8 9 NUll A
                       0x8e,  0x89,  0x8c //F H P 
                                                }; 

void seg_proc(uint8_t n,uint8_t num,uint8_t point)
{
  P0 =0xff;
	P2 = (P2 & 0x1f) | 0xe0;
	P2 = P2 & 0x1f;
	
	P0 = 0x01 << n;
	P2 = (P2 & 0x1f) | 0xc0;
	P2 = P2 & 0x1f;
	
	P0 = arr[num];
	if(point)P0 = P0 & 0x7f;
	P2 = (P2 & 0x1f) | 0xe0;
  P2 = P2 & 0x1f;	
}
