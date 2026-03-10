#include "seg.h"
#include "hc138.h"

uint8_t arr[] = {0xc0, 0xf9,  0xa4,  0xb0,  
0x99,  0x92,  0x82,  0xf8,  
0x80,  0x90,  0xff,  0x8e,// 8 9 null f
0x88,0x8c,0x89//a p h
};

void seg_proc(uint8_t n,uint8_t num,uint8_t point)
{
 P0 = 0xff;
 hc138_select(7);
 hc138_select(0);

 P0 = 0x01 << n;
 hc138_select(6);
 hc138_select(0);

 P0 = arr[num];
 if(point)P0 &= 0x7f;
 hc138_select(7);
 hc138_select(0);	
}
