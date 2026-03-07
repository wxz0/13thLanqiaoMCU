#include <STC15F2K60S2.H>
#include "led.h"
#include "key.h"
#include "seg.h"
#include "onewire.h"
#include "ds1302.h"

idata uint8_t seg_index = 0;
pdata uint8_t seg_show[] = {10,10,10,10,10,10,10,10};
pdata uint8_t seg_point[] = {0,0,0,0,0,0,0,0};
idata uint8_t show_mode = 0;

pdata uint8_t led_state[] = {0,0,0,0,0,0,0,0};
idata bit relay_state = 0;
idata bit relay_on_flag = 0;
idata uint16_t relay_5s_count = 0;
idata bit led1_on = 0;
idata bit led3_flash = 0;
idata uint16_t led_5s_count = 0;
idata uint8_t led_100ms_count = 0;

idata uint8_t key_state = 0;
idata bit key17_press = 0;

idata uint16_t temp = 0;
idata uint8_t temp_limit = 23;
idata uint8_t time[] = {12,59,50};
idata bit control_mode = 0;

idata uint8_t seg_slow = 0;
idata uint8_t key_slow = 0;
idata uint8_t time_slow = 0;
idata uint8_t temp_slow = 0;

void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x20;				//设置定时初始值
	TH0 = 0xD1;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
	EA = 1;
}

void time_task(void)
{
  if(time_slow)return;
	time_slow = 1;
	readtime(time);
}

void temp_task(void)
{
	uint16_t tmp = 0;
  if(temp_slow)return;
	temp_slow = 1;
	tmp = readtemp() * 10;
	if(tmp<=800)//限制跳变
	{
	  temp = tmp;
	}
}

//void temp_task(void)
//{
//  if(temp_slow)return;
//	temp_slow = 1;
//	temp = readtemp() * 10;
//}

void led_task(void)
{
  if(time[1] == 0 && time[2] == 0)
	{
	  led1_on = 1;
		led_state[0] = 1;
	}
	
	if(control_mode)
	{
	  led_state[1] = 0;
		if(time[1] == 0 && time[2] == 0)
	  {

			relay_on_flag = 1;
	  }
		if(relay_on_flag == 1)
		{
		  relay_state = 1;
		}
		else
		{
		  relay_state = 0;
		}
	}
	else
	{
	  led_state[1] = 1;
		if(temp >= temp_limit * 10)
		{
		  relay_state = 1;
		}
		else
		{
		  relay_state = 0;
		}
	}
	
	if(relay_state)
	{
	  led_state[2] = led3_flash;
	}
	else
	{
	  led_state[2] = 0;
	}
	relay_proc(relay_state);
}

void seg_task(void)
{
  if(seg_slow)return;
	seg_slow = 1;
  switch(show_mode)
  {
	  case 0:
			seg_show[0] = 11;
		  seg_show[1] = 1;
		  seg_show[2] = 10;
		  seg_show[3] = 10;
      seg_show[4] = 10;
      seg_show[5] = temp / 100;
     	seg_show[6] = (temp / 10) % 10;
		  seg_point[6] = 1;
      seg_show[7] = temp % 10;		
			break;
		case 1:
			seg_show[0] = 11;
		  seg_show[1] = 2;
		  seg_show[2] = 10;
		  if(key17_press)
			{
			  seg_show[3] = time[1] / 10;
				seg_show[4] = time[1] % 10;
				seg_show[5] = 12;
				seg_show[6] = time[2] / 10;
				seg_point[6] = 0;
				seg_show[7] = time[2] % 10;	
			}
			else
			{
			  seg_show[3] = time[0] / 10;
				seg_show[4] = time[0] % 10;
				seg_show[5] = 12;
				seg_show[6] = time[1] / 10;
				seg_point[6] = 0;
				seg_show[7] = time[1] % 10;	
			}
		  
   		break;
		case 2:
			seg_show[0] = 11;
		  seg_show[1] = 3;
		  seg_show[2] = 10;
		  seg_show[3] = 10;
      seg_show[4] = 10;
		  seg_show[5] = 10;
     	seg_show[6] = temp_limit / 10;
      seg_show[7] = temp_limit % 10;		  
			break;
	}
}
void key_task(void)
{
	uint8_t key_num = 0;
  if(key_slow)return;
	key_slow = 1;
  key_state = key_state << 4;
	key_state |= key_getstate();
	
	if((key_state & 0x11) == 0x10)key_num = 12;
	if((key_state & 0x22) == 0x20)key_num = 13;
	if((key_state & 0x44) == 0x40)key_num = 16;
	if((key_state & 0x88) == 0x80)
	{
	  key_num = 17;
		key17_press = 0;
	}
	
	if((key_state & 0x88) == 0x08)
	{
	  key17_press = 1;
	}
	switch(key_num)
	{
	  case 12:
			show_mode++;
		  show_mode %= 3;
			break;
		case 13:
			control_mode = !control_mode;
		  break;
		case 16:
			if(show_mode == 2)
			{
			  temp_limit++;
				if(temp_limit>=90)temp_limit = 99;
			}
		  break;
		case 17:
			if(show_mode == 2)
			{
			  temp_limit--;
				if(temp_limit<=10)temp_limit = 10;
			}
		  break;
	}

	
	
}

void Timer0_Isr(void) interrupt 1
{
	seg_index++;
	seg_slow++;
	key_slow++;
	time_slow++;
	temp_slow++;
	
	if(seg_index>=8)seg_index = 0;
	if(seg_slow>=90)seg_slow = 0;
	if(key_slow>=10)key_slow = 0;
	if(time_slow>=160)time_slow = 0;
	if(temp_slow>=160)temp_slow = 0;
	
	if(led1_on)
	{
	  led_5s_count++;
		if(led_5s_count >= 5000)
		{
			led_state[0] = 0;
			if(control_mode)
			{
			  relay_state = 0;
			}
			led1_on = 0;
		  led_5s_count = 0;
		}
	}
	
	if(relay_on_flag)
	{
	  relay_5s_count++;
		if(relay_5s_count >= 5000)
		{
			relay_on_flag = 0;
		  relay_5s_count = 0;
		}
	}
	else
	{
	  relay_5s_count = 0;
	}
	
	if(relay_state)
	{
	  led_100ms_count++;
		if(led_100ms_count >= 100)
		{
			led3_flash = !led3_flash;
		  led_100ms_count = 0;
		}
	}
	else
	{
	  led_100ms_count = 0;
	}
	
	seg_proc(seg_index,seg_show[seg_index],seg_point[seg_index]);
	led_proc(led_state);
	
}
void main(void)
{
	sys_init();
	Timer0_Init();
	writetime(time);
	readtemp();
	while(1)
	{
	  seg_task();
		key_task();
	  temp_task();
		time_task();
		led_task();
	}
	
}