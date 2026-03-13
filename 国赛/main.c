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
float volt_humi = 0;
float volt_out = 0;

idata uint16_t dist = 0;
idata bit dist_mode = 0;

idata uint8_t pram_mode = 0;
idata uint8_t fre_limit = 90;
idata uint8_t humi_limit = 40;
idata uint8_t dist_limit = 60;

idata bit fre_higher_flag = 0;
idata bit humi_higher_flag = 0;
idata bit dist_higher_flag =0;


idata uint8_t led_state[] = {0,0,0,0,0,0,0,0};
idata bit led_flash =0;
idata uint8_t led100ms_count = 0;

idata uint8_t pwm_level =0;
idata uint8_t pwm_count = 0;

idata bit relay_state = 0;
idata uint8_t relay_count = 0;

idata uint8_t key_state =0;
idata bit key7_press = 0;
idata uint16_t key1s_count = 0;

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

void Timer2_Init(void)		//100??@12.000MHz
{
	AUXR |= 0x04;			//?????1T??
	T2L = 0x50;				//???????
	T2H = 0xFB;				//???????
	AUXR |= 0x10;			//???2????
	IE2 |= 0x04;			//?????2??
}

void led_task(void)
{
	uint8_t i = 0;
	static bit relay_has_count =0;
	if(show_mode < 3)
	{
		for(i = 0;i<3;i++)
		{
		 led_state[i] = (i == show_mode); 
		}
	}
	else
	{
	  for(i = 0;i<3;i++)
		{
			led_state[i] = (i == pram_mode)?led_flash:0; 
		}
	}
	
	led_state[3] = fre_higher_flag;
	led_state[4] = humi_higher_flag;
	led_state[5] = dist_higher_flag;
  
  pwm_level = fre_higher_flag?8:2;
	
	if(!relay_has_count && dist_higher_flag)
	{
	  relay_has_count = 1;
    relay_count++;
	}
	else if(relay_has_count && !dist_higher_flag)
	{
	  relay_has_count = 0;
    relay_count++;
	}
	
	relay_proc(dist_higher_flag);
	writebyte(relay_count,0);
}

void wave_task(void)
{
  if(wave_slow)return;
	wave_slow = 1;
	dist = readdist();

	dist_higher_flag = dist > dist_limit;
}

void adc_task(void)
{
 if(adc_slow)return;
 adc_slow = 1;
 dac = readvlot();
 volt_humi = dac / 51.0;
 if(volt_humi >= 5.0)
 {
   humi = 100;
 }
 else
 {
   humi = volt_humi * 20;
 }	 

 if(humi < humi_limit)
 {
   volt_out =1;
 }
 else if(humi <= 80)
 {
   volt_out = 4.0/(80-humi_limit) * humi + (80 - 5*humi_limit)/(80 - humi_limit)*1.0;
 }
 else
 {
	 volt_out = 5;
 }
 humi_higher_flag = humi > humi_limit;
 dacout(volt_out*51);  
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

 if(show_mode == 1)
 {
	 if((key_state & 0x88) == 0x88)	
	 {
		key7_press = 1;
	 }
	 else
	 {
		key7_press = 0; 
	 }
 }
 switch(key_num)
 {
   case 4:
		 show_mode++;
	   show_mode %= 4;
	   if(show_mode == 3)pram_mode = 0;
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
					 fre_limit += 5;
				   if(fre_limit > 120) fre_limit= 10;
					 break;
				 case 1:
					 humi_limit += 10;
				   if(humi_limit > 60) humi_limit= 10;
					 break;
				 case 2:
					 dist_limit += 10;
				   if(dist_limit > 120) dist_limit= 10;
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
					 fre_limit -= 5;
				   if(fre_limit < 10) fre_limit= 120;
					 break;
				 case 1:
					 humi_limit -= 10;
				   if(humi_limit < 10)humi_limit = 60;
					 break;
				 case 2:
					 dist_limit -= 10;
				   if(dist_limit < 10)dist_limit = 120;
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
	   seg_show[5] = humi / 100 % 10?humi / 100 % 10:10;
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
			 seg_show[5] = dist / 100?dist/100:10;
			 seg_point[5] = 0;
			 seg_show[6] = ((dist / 100  == 0) && (dist / 10 == 0))?10:(dist / 10 % 10);
			 seg_show[7] = dist % 10;
		 }
		 else
		 {
		   seg_show[1] = 10;
			 seg_show[2] = 10;
			 seg_show[3] = 10;
			 seg_show[4] = 10;
			 seg_show[5] = dist / 100;
			 seg_point[5] = 1;
			 seg_show[6] = dist / 10 % 10;
			 seg_show[7] = dist % 10;
		 }
		 break;
	 case 3:
		 seg_point[5] = 0;
		 seg_point[6] = (pram_mode == 1) ? 0 : 1;
		 seg_show[0] = 13;
	   seg_show[1] = pram_mode + 1;
		 seg_show[2] = 10;
		 seg_show[3] = 10;
		 seg_show[4] = 10;

	  
	   switch(pram_mode)
		 {
			 case 0:
				 seg_show[5] = fre_limit / 100 % 10?fre_limit /100%10:10;
				 seg_show[6] = fre_limit / 10 % 10;
			   seg_point[6] = 1;
			   seg_show[7] = fre_limit % 10;
				 break;
			 case 1:
				 seg_show[5] = 10;
				 seg_show[6] = humi_limit / 10;
			   seg_point[6] = 0;
			   seg_show[7] = humi_limit % 10;
			   break;
			 case 2:
				 seg_show[5] = 10;
				 seg_show[6] = dist_limit / 100;
			   seg_point[6] = 1;
			   seg_show[7] = dist_limit /10 % 10;
			   break;
		 }
		 break;
 }
}

void Timer1_Isr(void) interrupt 3
{
	if(++seg_slow >= 100)seg_slow = 0;
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
		fre_higher_flag = fre > (fre_limit * 100);
	}
	
	if(show_mode == 3)
	{
	  if(++led100ms_count >= 100)
		{
		 led100ms_count = 0;
		 led_flash = !led_flash;	
		}
	}
	else
	{
	  led100ms_count =0;
	}
	
	if(key7_press)
	{
	 if(++key1s_count >= 1000)
	 {
		 relay_count = 0;
		 writebyte(relay_count,0x00);
		 key1s_count = 0;
	 }		 
	}
	else
	{
	  key1s_count = 0;
	}
	seg_proc(seg_index,seg_show[seg_index],seg_point[seg_index]);
  led_proc(led_state);
}

void Timer2_Isr(void) interrupt 12
{
	if(++pwm_count ==10)
	{
	  pwm_count = 0;
	}
  motor_proc(pwm_count < pwm_level);
}


void main(void)
{
  Timer0_Init();
	Timer1_Init();
	Timer2_Init();
	writebyte(0,0);
  while(1)
	{
		wave_task();
		adc_task();
	  seg_task();
		key_task();
		led_task();
	}

}