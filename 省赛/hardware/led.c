#include "led.h"
#include "74hc138.h"

void sys_init(void)
{
	P0 = 0xff;
  hc138_select(4);
	hc138_select(0);
	
	P0 = 0xaf;
  hc138_select(5);
	hc138_select(0);
	
	P0 = 0xff;
  hc138_select(6);
	hc138_select(0);
	P0 = 0xff;
	
  hc138_select(7);
	hc138_select(0);
}
void led_proc(uint8_t state[])
{
 static uint8_t state_old = 0x00;
 uint8_t i,temp = 0;
	
 for(i=0;i<8;i++)
 {
   temp = temp | (state[i] << i);
 }
 
 if(temp != state_old)
 {
   P0 = ~temp;
	 hc138_select(4);
	 hc138_select(0);
	 state_old = temp;
 }
 
}

void relay_proc(bit flag)
{
	static uint8_t state = 0;
	uint8_t temp = 0;
  if(flag)
	{
	  temp |= ~0xef;
	}
	else
	{
	  temp &= 0xef;
	}
	if(temp != state)
	{
	  P0 = temp;
		hc138_select(5);
		hc138_select(0);
		state = temp;
	}
}