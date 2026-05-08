#include <STC15F2K60S2.H>

void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CWaitAck(void);

uint8_t adc_read(void);
void dac_out(uint8_t byte);
void write_byte(uint8_t addr,uint8_t byte);
uint8_t read_byte(uint8_t addr);