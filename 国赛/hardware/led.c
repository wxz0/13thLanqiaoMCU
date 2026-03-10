#include "led.h"
#include "hc138.h"

void led_proc(uint8_t state[])
{
  static uint8_t old = 0;
	uint8_t temp = 0;
	uint8_t i = 0;
  for(i = 0;i <8;i++)
	{
	  temp |= state[i] << i;
	}
	
	if(temp!= old)
	{
	  P0 = ~temp;
		hc138_select(4);
		hc138_select(0);
		old = temp;
	}
}

void relay_proc(bit state)
{
 static uint8_t old= 0x00;
	uint8_t temp = 0;
  if(state)
	{
	  temp |= 0x10;
	}
	else
	{
	  temp &= ~0x10; 
	}	
	if(temp!= old)
	{
	  P0 = temp;
		hc138_select(5);
		hc138_select(0);
		old = temp;
	}
}
