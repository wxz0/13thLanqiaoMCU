#include <STC15F2K60S2.H>
#include <stdio.h>

void Uart1_Init(void);
void uart_sendbyte(uint8_t byte);
extern char putchar(char ch);