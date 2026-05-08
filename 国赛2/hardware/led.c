#include "led.h"

static uint8_t old1 = 0x00;
static uint8_t temp1 = 0xff;




void led_proc(uint8_t led_state[])
{
  static uint8_t state = 0;
	uint8_t temp =0;
	uint8_t i = 0;
	
	for(i = 0;i<8;i++)
	{
	  temp |= led_state[i] << i;
	}
	
	if(temp != state)
	{
	  P0 = ~temp;
		P2 = (P2&0x1f) | 0x80;
		P2 = P2&0x1f;
		state = temp;
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
 
 if(temp1 != old1)
 {
   P0 = temp1;
	 P2 = (P2 & 0x1f) | 0xa0;
   P2 = (P2 & 0x1f);
	 old1 = temp1;
 }
}

void buzzer_proc(bit buzzer_state)
{
	 if(buzzer_state)
	 {
		 temp1 |= 0x40;
	 }
	 else
	 {
		 temp1 &= ~0x40;
	 }
	 
	 if(temp1 != old1)
	 {
		 P0 = temp1;
		 P2 = (P2 & 0x1f) | 0xa0;
		 P2 = (P2 & 0x1f);
		 old1 = temp1;
	 }
}

void motor_proc(bit mottor_state)
{
  if(mottor_state)
	{
	  temp1 |= 0x20;
	}
	else
	 {
		 temp1 &= ~0x20;
	 }
	 
	 if(temp1 != old1)
	 {
		 P0 = temp1;
		 P2 = (P2 & 0x1f) | 0xa0;
		 P2 = (P2 & 0x1f);
		 old1 = temp1;
	 }
}

void sys_init(void)
{
 P0 = 0xff;
 P2 = (P2&0x1f) | 0x80;
 P2 = P2&0x1f;
	
 buzzer_proc(0);
 relay_proc(0);	
 motor_proc(0);	
	
 P0 = 0xff;
 P2 = (P2&0x1f) | 0xc0;
 P2 = P2&0x1f;

 P0 = 0xff;
 P2 = (P2&0x1f) | 0xe0;
 P2 = P2&0x1f;	
}