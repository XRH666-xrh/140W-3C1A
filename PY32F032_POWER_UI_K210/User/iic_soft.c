#include "iic_soft.h"

#define SDA_PORT		GPIOF
#define SDA_PIN			LL_GPIO_PIN_0
#define SDA_CLOCK       LL_IOP_GRP1_PERIPH_GPIOF

#define SCL_PORT		GPIOF
#define SCL_PIN			LL_GPIO_PIN_1
#define SCL_CLOCK       LL_IOP_GRP1_PERIPH_GPIOF


#define SOFT_IIC_SDA_CLOCK_EN 		    LL_IOP_GRP1_EnableClock(SDA_CLOCK)
#define SOFT_IIC_SCL_CLOCK_EN 		    LL_IOP_GRP1_EnableClock(SCL_CLOCK)

#define SOFT_IIC_SDA_OUT    {LL_GPIO_SetPinMode(SDA_PORT, SDA_PIN, LL_GPIO_MODE_OUTPUT);}
#define SOFT_IIC_SCL_OUT    LL_GPIO_SetPinMode(SCL_PORT, SCL_PIN, LL_GPIO_MODE_OUTPUT)

#define SOFT_IIC_SDA_IN    {LL_GPIO_SetPinMode(SDA_PORT, SDA_PIN, LL_GPIO_MODE_INPUT);LL_GPIO_SetPinPull(SDA_PORT, SDA_PIN, LL_GPIO_PULL_UP);}

#define SOFT_IIC_SDA_READ   LL_GPIO_IsInputPinSet(SDA_PORT, SDA_PIN)

#define SOFT_IIC_SDA_HIGH  LL_GPIO_SetOutputPin(SDA_PORT, SDA_PIN)
#define SOFT_IIC_SCL_HIGH  LL_GPIO_SetOutputPin(SCL_PORT, SCL_PIN)

#define SOFT_IIC_SDA_LOW   LL_GPIO_ResetOutputPin(SDA_PORT, SDA_PIN)
#define SOFT_IIC_SCL_LOW   LL_GPIO_ResetOutputPin(SCL_PORT, SCL_PIN)



#define I2C_WRITE_FLAG 0x01
#define I2C_READ_FLAG  0x00


//模拟IIC逻辑
/***************************************************************************** 
* I2C 时钟延时函数 
******************************************************************************/ 
void Delay_i2c(uint16_t time) 
{ 
	uint8_t a; 
	time *= 50;
	for(a = time; a>0; a--); 
} 
/***************************************************************************** 
* I2C 启动信号函数 
******************************************************************************/ 
void I2C_Start(void)
{ 
	SOFT_IIC_SDA_OUT; 
	SOFT_IIC_SCL_OUT; 
	SOFT_IIC_SDA_HIGH; 
	SOFT_IIC_SCL_HIGH; 
	Delay_i2c(1); 
	SOFT_IIC_SDA_LOW; 
	Delay_i2c(1); 
	SOFT_IIC_SCL_LOW; 
	Delay_i2c(1); 
} 
/***************************************************************************** 
* 发送一个字节数据，并获取应答
******************************************************************************/ 
uint8_t SendByteAndGetNACK(uint8_t dataToSend) 
{ 
	uint16_t i; 
	//uint8_t ack; 
	////printf_rtt("send  %x\n", dataToSend);
	SOFT_IIC_SDA_HIGH; 
	for (i = 0; i < 8; i++)
	{ 
		SOFT_IIC_SCL_LOW; 
		Delay_i2c(1); 
		if (dataToSend>>7)
		{
			SOFT_IIC_SDA_HIGH;
		}
		else
		{
			SOFT_IIC_SDA_LOW;
		}
		Delay_i2c(1); 
		SOFT_IIC_SCL_HIGH; 
		Delay_i2c(1); 
		dataToSend <<= 1; 
	} 
	SOFT_IIC_SCL_LOW; 
	Delay_i2c(3); 
	SOFT_IIC_SDA_IN; 
	//Delay_i2c(3); 
	SOFT_IIC_SCL_HIGH; 
	//Delay_i2c(1); 
	
	i= 10; 
	while(i--) 
	{ 
		if(!SOFT_IIC_SDA_READ)
		{
			SOFT_IIC_SCL_LOW;
			//printf_rtt("Ack %d\n", i);
			SOFT_IIC_SDA_OUT; 
			return I2C_ACK;
		} 
	} 
	//printf_rtt("Nack\n");
	SOFT_IIC_SCL_LOW; 
	SOFT_IIC_SDA_OUT; 
	return I2C_NACK; 
} 
/***************************************************************************** 
* 读取一个字节信号，并下发应答命令. 
******************************************************************************/ 
void I2C_Respond(uint8_t ACKSignal) 
{ 
	SOFT_IIC_SDA_OUT; 
	//SOFT_IIC_SDA_HIGH; 
	//SOFT_IIC_SDA_LOW; 
	SOFT_IIC_SCL_LOW; 
	//SDA = ACKSignal; 
	if (ACKSignal)
	{
		SOFT_IIC_SDA_HIGH;
	}
	else
	{
		SOFT_IIC_SDA_LOW;
	}
	Delay_i2c(1); 
	SOFT_IIC_SCL_HIGH; 
	Delay_i2c(1);
	SOFT_IIC_SCL_LOW; 
} 
/***************************************************************************** 
* 停止信号 
******************************************************************************/ 
void I2C_Stop() 
{ 
	SOFT_IIC_SCL_LOW; 
	SOFT_IIC_SDA_HIGH; 
	SOFT_IIC_SDA_LOW; 
	Delay_i2c(1); 
	SOFT_IIC_SCL_HIGH; 
	Delay_i2c(1); 
	SOFT_IIC_SDA_HIGH; 
} 
/***************************************************************************** 
* 读取一个字节函数
******************************************************************************/ 
uint8_t I2C_Receive8Bit(void) 
{ 
	uint8_t i, buffer; 
	SOFT_IIC_SDA_IN; 
	SOFT_IIC_SCL_LOW; 
	for (i = 0; i < 8; i++) 
	{ 
		Delay_i2c(1); 
		SOFT_IIC_SCL_HIGH; 
		buffer = (buffer<<1)|SOFT_IIC_SDA_READ; 
		Delay_i2c(1); 
		SOFT_IIC_SCL_LOW; 
	} 
	return (buffer); 
}

/*************************************************************************************************************************
* 
**************************************************************************************************************************/ 
uint8_t SI2C_Write_Bytes(uint8_t deviceAddr, uint8_t REG, uint8_t *DAT8, uint8_t len)
{
 I2C_Start(); 
 if (SendByteAndGetNACK((deviceAddr<<1)& ~0x01))
 {
	 I2C_Stop(); 
	 return I2C_NACK;
 } 
 if (SendByteAndGetNACK(REG))
 {
	 I2C_Stop(); 
	 return I2C_NACK;
 } 
 while(len--)
 {
	 if (SendByteAndGetNACK(*DAT8))
	 {
		 I2C_Stop(); 
		 return I2C_NACK;
	 }
	 DAT8++;
 }
 
 I2C_Stop();
 return I2C_ACK;
} 

/*************************************************************************************************************************
* 
**************************************************************************************************************************/ 
uint8_t SI2C_Read_Bytes(uint8_t deviceAddr, uint8_t REG, uint8_t *DAT8, uint8_t len)
{
	//uint8_t buf1,buf2;
	I2C_Start(); 
	if (SendByteAndGetNACK((deviceAddr<<1) & ~0x01))
	{
		I2C_Stop(); 
		return I2C_NACK;
	} 
	if (SendByteAndGetNACK(REG))
	{
		I2C_Stop(); 
		return I2C_NACK;
	} 
	//I2C_Stop(); 

	
	I2C_Start();
	if (SendByteAndGetNACK((deviceAddr<<1) | 0x01))
	{
		I2C_Stop(); 
		return I2C_NACK;
	} 

	 while(len--)
	 {
		 *DAT8 = I2C_Receive8Bit();
		 if (len)
		 {
		 	I2C_Respond(0);
			DAT8++;
		 }
	}
	I2C_Respond(1);
	I2C_Stop(); 
	
	return I2C_ACK;
} 


void I2c_Init(void)
{
	SOFT_IIC_SDA_CLOCK_EN;
	SOFT_IIC_SCL_CLOCK_EN;
	
	SOFT_IIC_SDA_OUT;
	SOFT_IIC_SCL_OUT;

	SOFT_IIC_SDA_HIGH;
	SOFT_IIC_SCL_HIGH;
}
