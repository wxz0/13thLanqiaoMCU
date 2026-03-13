#include "led.h"
#include "hc138.h"

static uint8_t temp1 = 0x00;
static uint8_t temp1_old = 0xff;

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

void relay_proc(bit relay_state)
{
  if(relay_state)
	{
	  temp1 |= 0x10;
	}
	else
	{
	  temp1 &= ~0x10; 
	}
  
	if(temp1!= temp1_old)
	{
	  P0 = temp1;
		hc138_select(5);
		hc138_select(0);
		temp1_old = temp1;
	}
}

void motor_proc(bit motor_state)
{
  if(motor_state)
	{
	  temp1 |= 0x20;
	}
	else
	{
	  temp1 &= ~0x20; 
	}
  
	if(temp1!= temp1_old)
	{
	  P0 = temp1;
		hc138_select(5);
		hc138_select(0);
		temp1_old = temp1;
	}
}
