#ifndef   __IIC_SOFT_H__
#define   __IIC_SOFT_H__
#include "main.h"


void IIC2_Init(void);
void I2c_Start(void);
void I2c_Stop(void);
void I2c_Error(void);
void I2c_Ack(void);
void I2c_NoAck(void);


#define I2C_NACK  1
#define I2C_ACK   0

char I2c_WriteByte(unsigned char databuf);
unsigned char I2c_ReadByte(unsigned char endbyte);
uint8_t SI2C_Write_Bytes(uint8_t deviceAddr, uint8_t REG, uint8_t *DAT8, uint8_t len);
uint8_t SI2C_Read_Bytes(uint8_t deviceAddr, uint8_t REG, uint8_t *DAT8, uint8_t len);
void I2c_Init(void);


#endif


