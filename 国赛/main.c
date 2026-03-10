#include <STC15F2K60S2.H>
#include "seg.h"
#include "led.h"
#include "key.h"
#include "iic.h"
#include "wave.h"
idata uint8_t seg_index= 0;
pdata uint8_t seg_show[] = {10,10,10,10,10,10,10,10};
pdata uint8_t seg_point[] = {0,0,0,0,0,0,0,0};
idata uint8_t show_mode = 0;

idata uint16_t fre = 0;
idata uint16_t fre_100x = 0;
idata uint16_t fre_1s_count = 0;
idata bit fre_mode = 0;

idata uint8_t humi = 0;
idata uint8_t dac = 0;
float volt = 0;

idata uint16_t dist = 0;
idata bit dist_mode = 0;

idata uint8_t pram_mode = 0;
idata uint16_t fre_limit = 90;
idata uint8_t humi_limit = 40;
idata uint8_t dist_limit = 6;

idata uint8_t led_state[] = {0,0,0,0,0,0,0,0};

idata uint8_t key_state =0;

idata uint8_t key_slow = 0;
idata uint8_t seg_slow = 0;
idata uint8_t adc_slow = 0;
idata uint8_t wave_slow = 0;

void Timer0_Init(void)		//1??@12.000MHz
{
	AUXR |= 0x80;			//?????1T??
	TMOD &= 0xF0;			//???????
	TMOD |= 0x04;
	TL0 = 0x00;				//???????
	TH0 = 0x00;				//???????
	TF0 = 0;				//??TF0??
	TR0 = 1;				//???0????
}

void Timer1_Init(void)		//1??@12.000MHz
{
	AUXR |= 0x40;			//?????1T??
	TMOD &= 0x0F;			//???????
	TL1 = 0x20;				//???????
	TH1 = 0xD1;				//???????
	TF1 = 0;				//??TF1??
	TR1 = 1;				//???1????
	ET1 = 1;				//?????1??
	EA = 1;
}

void wave_task(void)
{
  if(wave_slow)return;
	wave_slow = 1;
	dist = readdist();
}

void adc_task(void)
{
 if(adc_slow)return;
 adc_slow = 1;
 dac = readvlot();
 volt = dac / 51.0;
 humi = volt * 20;	
}

void key_task(void)
{
 uint8_t key_num = 0;
 if(key_slow)return;
 key_slow = 1;
 key_state = key_state << 4;
 key_state |= key_getstate();

 if((key_state & 0x11) == 0x10)key_num = 4;
 if((key_state & 0x22) == 0x20)key_num = 5;
 if((key_state & 0x44) == 0x40)key_num = 6;
 if((key_state & 0x88) == 0x80)key_num = 7;

 switch(key_num)
 {
   case 4:
		 show_mode++;
	   show_mode %= 4;
		 break;
	 case 5:
		 pram_mode++;
	   pram_mode %= 3;
	   break;
	 case 6:
		 if(show_mode == 2)
		 {
		   dist_mode = !dist_mode;
		 }
		 else if(show_mode == 3)
		 {
		   switch(pram_mode)
			 {
				 case 0:
					 fre_limit = fre_limit + 5;
					 break;
				 case 1:
					 humi_limit = humi_limit + 10;
					 break;
				 case 2:
					 dist_limit = dist_limit + 1;
					 break;
			 } 
		 }
		 break;
	 case 7:
		 if(show_mode == 0)
		 {
		   fre_mode = !fre_mode;
		 }
		 else if(show_mode == 3)
		 {
		   switch(pram_mode)
			 {
				 case 0:
					 fre_limit = fre_limit - 5;
					 break;
				 case 1:
					 humi_limit = humi_limit - 10;
					 break;
				 case 2:
					 dist_limit = dist_limit - 1;
					 break;
			 } 
		 }
		 break;
 }	 
}

void seg_task(void)
{
 uint8_t i = 0;
 if(seg_slow)return;
 seg_slow = 1;
 switch(show_mode)
 {
	 case 0:
		 seg_show[0] = 11;
	   seg_show[1] = 10;
	   seg_point[6] = 0;
	   if(!fre_mode)
		 {
			 seg_show[2] = fre / 100000;
		   seg_show[3] = fre / 10000;
			 seg_show[4] = fre / 1000 % 10;
			 seg_show[5] = fre / 100 % 10;
			 seg_show[6] = fre / 10 % 10;
			 seg_show[7] = fre % 10;
			 for(i = 2;seg_show[i] == 0;i++)
			 {
				 seg_show[i] = 10;
			 }
		 }
		 else
		 {
			 seg_point[6] = 1;
		   if(fre < 100)
			 {
			   seg_show[2] = 10;
				 seg_show[3] = 10;
				 seg_show[4] = 10;
				 seg_show[5] = 10;
				 seg_show[6] = 0;
				 seg_show[7] = 0;
			 }
			 else 
			 {
			   fre_100x = fre / 100.0;
				 seg_show[2] = fre_100x / 100000;
				 seg_show[3] = fre_100x / 10000;
				 seg_show[4] = fre_100x / 1000 % 10;
				 seg_show[5] = fre_100x / 100 % 10;
				 seg_show[6] = fre_100x / 10 % 10;
				 seg_show[7] = fre_100x % 10;
				 for(i=2 ;i<=5;i++)
				 {
					 if( seg_show[i] == 0)
					 {
					   seg_show[i] = 10;
					 }
					 else
					 {
					   break;
					 } 
				 }
			 }
			 
		 }
		 break;
	 case 1:
		 seg_show[0] = 14;
	   seg_point[6] = 0;
	   seg_show[1] = 10;
		 seg_show[2] = 10;
		 seg_show[3] = 10;
		 seg_show[4] = 10;
		 seg_show[5] = 10;
		 seg_show[6] = humi / 10;
		 seg_show[7] = humi % 10;
	   
		 break;
	 case 2:
		 seg_show[0] = 12;
	   if(!dist_mode)
		 {
			 seg_show[1] = 10;
			 seg_show[2] = 10;
			 seg_show[3] = 10;
			 seg_show[4] = 10;
			 seg_show[5] = dist / 100;
			 seg_point[5] = 0;
			 seg_show[6] = dist / 10;
			 seg_show[7] = dist % 10;
			 if(seg_show[5] == 0)seg_show[5] = 10;
		 }
		 else
		 {
		   seg_show[1] = 10;
			 seg_show[2] = 10;
			 seg_show[3] = 10;
			 seg_show[4] = 10;
			 seg_show[5] = dist / 100;
			 seg_point[5] = 1;
			 seg_show[6] = dist / 10;
			 seg_show[7] = dist % 10;
		 }
		 break;
	 case 3:
		 seg_point[5] = 0;
		 seg_show[0] = 13;
	   seg_show[1] = pram_mode + 1;
		 seg_show[2] = 10;
		 seg_show[3] = 10;
		 seg_show[4] = 10;
		 seg_show[5] = 10;
	   switch(pram_mode)
		 {
			 case 0:
				 seg_show[6] = fre_limit / 10;
			   seg_point[6] = 1;
			   seg_show[7] = fre_limit % 10;
				 break;
			 case 1:
				 seg_show[6] = humi_limit / 10;
			   seg_point[6] = 0;
			   seg_show[7] = humi_limit % 10;
			   break;
			 case 2:
				 seg_show[6] = dist_limit / 10;
			   seg_point[6] = 1;
			   seg_show[7] = dist_limit % 10;
			   break;
		 }
		 break;
 }
}

void Timer1_Isr(void) interrupt 3
{
	if(++seg_slow >= 90)seg_slow = 0;
	if(++seg_index >= 8)seg_index = 0;
	if(++key_slow >= 10)key_slow = 0;
	if(++adc_slow >= 160)adc_slow = 0;
	if(++wave_slow >= 160)wave_slow = 0;
	if(++fre_1s_count >= 1000)
	{
	  fre_1s_count = 0;
		TR0 = 0;
		fre = (TH0 << 8 | TL0);
		TH0 = TL0 = 0;
		TR0 = 1;
	}
	
	seg_proc(seg_index,seg_show[seg_index],seg_point[seg_index]);
}

void main(void)
{
  Timer0_Init();
	Timer1_Init();
  while(1)
	{
		wave_task();
		adc_task();
	  seg_task();
		key_task();
	}

}