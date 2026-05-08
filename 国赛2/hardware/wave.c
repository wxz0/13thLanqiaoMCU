#include "wave.h"

void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void wave_start(void)
{
  uint8_t i = 0;
	for(i = 0;i<8;i++)
	{
	  P10 = 1;
		Delay12us();
		P10 = 0;
		Delay12us();
	}

}

float get_distance(void)
{
	uint16_t time = 0;
	CMOD &= 0x00;
  CH = CL = CF = CR = 0;
	
	wave_start();
	CR = 1;
	while(!CF && P11);
	CR = 0;
	if(!CF)
	{
	  time = (CH << 8 | CL);
		CH = CL = 0;
		return time * 0.017;
	}
	else
	{
	  CF = 0;
		return 0;
	}

}