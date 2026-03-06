#include <STC15F2K60S2.H>
#include "led.h"
#include "key.h"
#include "seg.h"

idata uint8_t seg_index = 0;
pdata uint8_t seg_show[] = {10,10,10,10,10,10,10,10};
pdata uint8_t seg_point[] = {0,0,0,0,0,0,0,0};
idata uint8_t show_mode = 0;

pdata uint8_t led_state[] = {0,0,0,0,0,0,0,0};

idata uint8_t key_state = 0;

idata uint8_t seg_slow = 0;
idata uint8_t key_slow = 0;

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

void seg_task(void)
{
  if(seg_slow)return;
	seg_slow = 1;
  switch(show_mode)
  {
	  case 0:
			seg_show[0] = 1;
			break;
		case 1:
			seg_show[0] = 2;
			break;
		case 2:
			seg_show[0] = 3;
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
	if((key_state & 0x88) == 0x80)key_num = 17;
	
	if(key_num == 12)
	{
	  show_mode++;
		show_mode %= 3;
	}
}

void Timer0_Isr(void) interrupt 1
{
	seg_index++;
	seg_slow++;
	key_slow++;
	
	if(seg_index>=8)seg_index = 0;
	if(seg_slow>=90)seg_slow = 0;
	if(key_slow>=10)key_slow = 0;
	
	seg_proc(seg_index,seg_show[seg_index],seg_point[seg_index]);
	led_proc(led_state);
}
void main(void)
{
	sys_init();
	Timer0_Init();
	led_state[0] = 1;
	while(1)
	{
	  seg_task();
		key_task();
	
	}
	
}