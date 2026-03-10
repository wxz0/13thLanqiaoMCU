#include "wave.h"

void Delay12us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void sendwave(void)
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

float readdist(void)
{ 
 CMOD &= 0x00;
 CR = 0;
 CF = 0;	
 CL = 0;
 CH = 0;
 sendwave();
 CR = 1;
 while(P11 == 1  && CF == 0);
 CR = 0;
 if(CF == 0)
 {
   return (CH << 8 | CL) * 17.0 / 1000.0;
 }
 else
 {
   CF = 0;
	 return 999;
 }
}
