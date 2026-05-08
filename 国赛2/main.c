#include <STC15F2K60S2.H>
#include "seg.h"
#include "key.h"
#include "led.h"
#include "uart.h"
#include <stdio.h>
#include "iic.h"
#include "wave.h"

pdata uint8_t seg_show[] = {10,10,10,10,10,10,10,10};
pdata uint8_t seg_point[] = {0,0,0,0,0,0,0,0};
idata uint8_t seg_index = 0;
idata uint8_t show_mode = 0;
idata bit fre_mode = 0;
idata bit dis_mode = 0;
idata uint8_t pram_mode = 0;
idata uint8_t pram_data[] = {90,40,6};

pdata uint8_t led_state[] ={0,0,0,0,0,0,0,0};
idata bit led1_flash = 0;
idata bit led2_flash = 0;
idata bit led3_flash = 0;
idata uint8_t led1_100ms_count = 0;
idata uint8_t led2_100ms_count = 0;
idata uint8_t led3_100ms_count = 0;
idata uint8_t relay_count = 0;
idata bit relay_state = 0;
idata bit cap_flag = 0;
idata uint8_t pwm_count = 0;
idata uint8_t pwm_mode =0;


idata uint8_t key_state = 0;
idata bit key7_press = 0;
idata uint16_t key7_press_count = 0;
idata bit key7_long_press = 0;

idata uint16_t fre_1s_count = 0;
idata uint16_t fre = 0;
idata uint16_t fre_100x = 0;

idata uint8_t humi = 0;
idata uint8_t humi_volt_10x = 0;
idata float humi_out = 0;
idata bit humi_limit_higher_flag = 0;

idata uint16_t dis = 0;
idata bit dis_limit_higher_flag = 0;

idata uint8_t seg_slow = 0;
idata uint8_t key_slow = 0;
idata uint8_t humi_slow = 0;
idata uint8_t dis_slow = 0;

void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x04;
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}


void Timer1_Init(void)		//1����@12.000MHz
{
	AUXR &= 0xBF;			//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;			//���ö�ʱ��ģʽ
	TL1 = 0x18;				//���ö�ʱ��ʼֵ
	TH1 = 0xFC;				//���ö�ʱ��ʼֵ
	TF1 = 0;				//���TF1��־
	TR1 = 1;				//��ʱ��1��ʼ��ʱ
	ET1 = 1;				//ʹ�ܶ�ʱ��1�ж�
	EA  = 1;
}

void Timer2_Init(void)		//200微秒@12.000MHz
{
	AUXR &= 0xFB;			//定时器时钟12T模式
	T2L = 0x38;				//设置定时初始值
	T2H = 0xFF;				//设置定时初始值
	AUXR |= 0x10;			//定时器2开始计时
	IE2 |= 0x04;			//使能定时器2中断
}


void led_task(void)
{
	led_state[0] = led1_flash;
	led_state[1] = led2_flash;
	led_state[2] = led3_flash;
	
	led_state[3] = dis_limit_higher_flag;
	led_state[4] = humi > pram_data[1];
	led_state[5] = cap_flag;

	pwm_mode = dis_limit_higher_flag?4:1;

  if(cap_flag && !relay_state)
	{
	  relay_state = 1;
		relay_count++;
		write_byte(0,relay_count);
	}
	else if(!cap_flag && relay_state)
	{
	  relay_count++;
		relay_state = 0;
	}
  
}

void dis_task(void)
{
 if(dis_slow)return;
 dis_slow = 1;
 dis = get_distance();
 cap_flag = dis > (pram_data[2] * 10);

}

void humi_task(void)
{	
 if(humi_slow)return;
 humi_slow = 1; 
 humi_volt_10x = adc_read() * 10.0 / 51.0;	
 humi = humi_volt_10x * 2;
 
 if(humi <= pram_data[1])
 {
   humi_out = 1;
 }
 else if(humi >= 80)
 {
   humi_out = 5;
 }
 else
 {
   humi_out = 4.0 / (80 - pram_data[1]) * humi + (80.0 - 5 * pram_data[1])*1.0 / (80 - pram_data[1]) *1.0;
 }
 
 dac_out(humi_out * 51.0);  
}


void key_task(void)
{
	uint8_t key_num = 0;
  if(key_slow)return;
	key_slow = 1;
	key_state <<= 4;
	key_state |= key_getstate();
	if((key_state & 0x11) == 0x01)key_num = 4;
	if((key_state & 0x22) == 0x02)key_num = 5;
	if((key_state & 0x44) == 0x04)key_num = 6;
	if((key_state & 0x88) == 0x08)key_num = 7;
	
	if(show_mode == 1)
	{
	 if((key_state & 0x88) == 0x88)
		key7_press = 1;
	 else
		key7_press = 0;
	 if(key7_long_press)
	 {
		write_byte(0,0);
		key7_long_press = 0;
	 }		
	}
	switch(key_num)
	{
	  case 4:
			show_mode = (show_mode + 1) % 4;
      if(show_mode == 2)pram_mode = 0;
      break;
		case 5:
			pram_mode = (pram_mode + 1) % 3;
			break;
		case 6:
			if(show_mode == 2)
			{
			  dis_mode = !dis_mode;
			}
			if(show_mode == 3)
			{
			   switch(pram_mode)
				 {
				   case 0:
						 pram_data[pram_mode] += 5;
					   if(pram_data[pram_mode] > 120)
						 {
						   pram_data[pram_mode] = 10;
						 }
						 break;
					 case 1:
						 pram_data[pram_mode] += 10;
					   if(pram_data[pram_mode] > 60)
						 {
						   pram_data[pram_mode] = 10;
						 }
						 break;
           case 2:
						 pram_data[pram_mode] += 1;
					   if(pram_data[pram_mode] > 12)
						 {
						   pram_data[pram_mode] = 1;
						 }
						 break;						 
				 }
			}
			break;
		case 7:
			if(show_mode == 0)
			{
			  fre_mode = !fre_mode;
			}
			if(show_mode == 3)
			{
			   switch(pram_mode)
				 {
				   case 0:
						 pram_data[pram_mode] -= 5;
					   if(pram_data[pram_mode] < 10)
						 {
						   pram_data[pram_mode] = 120;
						 }
						 break;
					 case 1:
						 pram_data[pram_mode] -= 10;
					   if(pram_data[pram_mode] < 10)
						 {
						   pram_data[pram_mode] = 60;
						 }
						 break;
           case 2:
						 pram_data[pram_mode] -= 1;
					   if(pram_data[pram_mode] < 1)
						 {
						   pram_data[pram_mode] = 12;
						 }
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
			seg_show[0] = 12;
		  seg_show[1] = 10;
		  if(!fre_mode)
			{
			  seg_show[2] = fre / 100000;
				seg_show[3] = fre / 10000 % 10;
				seg_show[4] = fre / 1000 % 10;
				seg_show[5] = fre / 100 % 10;
				seg_show[6] = fre / 10 % 10; 
				seg_show[7] = fre % 10;
         
        seg_point[6] = 0;
				
        for(i = 2;i<8;i++)
        {
				  if(seg_show[i] == 0)
					{
					  seg_show[i] = 10;
					}
					else
					{
					  break;
					}
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
				  fre_100x  = fre / 100.0;
					seg_show[2] = fre_100x / 100000;
					seg_show[3] = fre_100x / 10000 % 10;
					seg_show[4] = fre_100x / 1000 % 10;
					seg_show[5] = fre_100x / 100 % 10;
					seg_show[6] = fre_100x / 10 % 10; 
					seg_show[7] = fre_100x % 10;
				}
				
				for(i = 2;i<6;i++)
        {
				  if(seg_show[i] == 0)
					{
					  seg_show[i] = 10;
					}
					else
					{
					  break;
					}
				}
			}
			break;
		case 1:
			seg_show[0] = 13;
		  seg_show[1] = 10;
			seg_show[2] = 10;
			seg_show[3] = 10;
			seg_show[4] = 10;
		  seg_show[5] = humi/ 100?humi/ 100:10;
			seg_show[6] = humi/ 10 % 10; 
			seg_show[7] = humi % 10;	
		  seg_point[6] = 0;
			break;
		case 2:
			seg_show[0] = 11;
		  seg_show[1] = 10;
		  if(!dis_mode)
			{
				seg_show[2] = 10;
				seg_show[3] = 10;
				seg_show[4] = 10;
				seg_show[5] = dis/ 100 ? dis/ 100 : 10;
				seg_show[6] = (dis/ 100 || dis / 10) ? (dis/ 10 % 10) : 10;
				seg_show[7] = dis % 10;	
				seg_point[5] = 0;
			}
			else
			{
			  seg_show[2] = 10;
				seg_show[3] = 10;
				seg_show[4] = 10;
				seg_show[5] = dis/ 100;
				seg_point[5] = 1;
        seg_show[6] = dis/ 10 % 10;
				seg_show[7] = dis % 10;
			}
			break;
		case 3:
       seg_point[5] = 0;
       seg_show[0] = 14;
		   seg_show[1] = pram_mode + 1;
       seg_show[2] = 10;
			 seg_show[3] = 10;
			 seg_show[4] = 10;
		   seg_show[5] = pram_data[pram_mode]/ 100 % 10 ?pram_data[pram_mode]/ 100 % 10 :10;
			 seg_show[6] = pram_data[pram_mode]/ 10 % 10;
		   seg_point[6] = pram_mode == 1?0:1;
			 seg_show[7] = pram_data[pram_mode] % 10;		
	}
}

void Timer1_Isr(void) interrupt 3
{
	if(seg_slow++ >= 100)seg_slow = 0;
	if(seg_index++ >= 8)seg_index = 0;
	if(key_slow++ >= 10)key_slow = 0;
	if(humi_slow++ >= 160)humi_slow = 0;
	if(dis_slow++ >= 160)dis_slow = 0;
  if(fre_1s_count++ >= 1000)
	{
		TR0 = 0;
	  fre = (TH0 << 8 | TL0);
		TH0 = TL0 = 0;
		TR0 = 1;
		dis_limit_higher_flag = fre > pram_data[0] * 100;
		fre_1s_count = 0;
	}
	
	if(key7_press)
	{
	  if(key7_press_count++ >= 1000)
		{
		  key7_long_press = 1;
		}
	}
	else
	{
	  key7_press_count = 0;
		key7_long_press = 0;
	}
	
	if(show_mode == 0)
	{
	  if(led1_100ms_count++ >= 100)
		{
			led1_100ms_count = 0;
			led1_flash = !led1_flash;
		}
	}
	else
	{
	  led1_100ms_count = 0;
		led1_flash = 0;
	}
	
	if(show_mode == 1)
	{
	  if(led2_100ms_count++ >= 100)
		{
			led2_100ms_count = 0;
			led2_flash = !led2_flash;
		}
	}
	else
	{
	  led2_100ms_count = 0;
		led2_flash = 0;
	}
	
	if(show_mode == 2)
	{
	  if(led3_100ms_count++ >= 100)
		{
			led3_100ms_count = 0;
			led3_flash = !led3_flash;
		}
	}
	else
	{
	  led3_100ms_count = 0;
		led3_flash = 0;
	}
	
	seg_proc(seg_index,seg_show[seg_index],seg_point[seg_index]);
	led_proc(led_state);
	relay_proc(relay_state);
}

void Timer2_Isr(void) interrupt 12
{
	if(++pwm_count >= 5)pwm_count = 0;
	motor_proc(pwm_count <= pwm_mode);
}


void main(void)
{
	Timer0_Init();
  Timer1_Init();
  sys_init();
	Uart1_Init();
	Timer2_Init();
	while(1)
	{
		led_task();
		key_task();
	  seg_task();
		humi_task();
		dis_task();
	}

}