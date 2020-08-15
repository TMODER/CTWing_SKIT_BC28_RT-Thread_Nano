/**
  ******************************************************************************
  * @file    hal_iic.c
  * @author  kyj
  * @version V1.0
  * @date    
  * @brief   IO??IIC
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
 
#include "delay.h"
#include "stm32f1xx_hal.h"
 

 
 
#define HIGH  GPIO_PIN_SET
#define LOW   GPIO_PIN_RESET

 
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(uint32_t)8<<28;}  //0000 1000
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(uint32_t)3<<28;}  //0000 0011

#define IIC_SCL(n)  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,n)
#define IIC_SDA(n)  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,n)
#define READ_SDA    HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_7)			
 
 
/***************************************************************
 @Function			void IIC_Init(void)
 @Description		IIC???
 @Input				void
 @Return			void
***************************************************************/
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();						
	
	GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;	
	GPIO_Initure.Pull = GPIO_PULLUP;					
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;	
	HAL_GPIO_Init(GPIOC, &GPIO_Initure);
	
	IIC_SDA(HIGH);
	IIC_SCL(HIGH);
}
 
/***************************************************************
 @Function			void IIC_Start(void)
 @Description		??IIC????
					SCL : HIGH
					SDA : HIGH -> LOW
 @Input				void
 @Return			void
***************************************************************/
void IIC_Start(void)
{
	SDA_OUT();														//sda???
	IIC_SDA(HIGH);
	IIC_SCL(HIGH);
	delay_us(4);
	IIC_SDA(LOW);		//START : when CLK is high,DATA change form high to low
	delay_us(4);
	IIC_SCL(LOW);													//??I2C??,????????? 
}
 
/***************************************************************
 @Function			void IIC_Stop(void)
 @Description		??IIC????
					SCL : HIGH
					SDA : LOW -> HIGH
 @Input				void
 @Return			void
***************************************************************/
void IIC_Stop(void)
{
	SDA_OUT();														//sda???
	IIC_SDA(LOW);
	IIC_SCL(HIGH);
	delay_us(4);
	IIC_SDA(HIGH);		//STOP : when CLK is high,DATA change form low to high
	delay_us(4);
}
 
/***************************************************************
 @Function			uint8_t IIC_Wait_Ack(void)
 @Description		??ACK????
 @Input				void
 @Return			1 : ??????
					0 : ??????
***************************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	
	SDA_IN();														//SDA?????
	IIC_SDA(HIGH);
	delay_us(1);
	IIC_SCL(HIGH);
	delay_us(1);
	while (READ_SDA) {
		ucErrTime++;
		if (ucErrTime > 250) {
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(LOW);													//????0
	return 0;
}
 
/***************************************************************
 @Function			void IIC_Ack(void)
 @Description		??ACK??
 @Input				void
 @Return			void
***************************************************************/
void IIC_Ack(void)
{
	SDA_OUT();														//sda???
	IIC_SCL(LOW);
	IIC_SDA(LOW);
	delay_us(2);
	IIC_SCL(HIGH);
	delay_us(2);
	IIC_SCL(LOW);
}
 
/***************************************************************
 @Function			void IIC_NAck(void)
 @Description		???ACK??
 @Input				void
 @Return			void
***************************************************************/
void IIC_NAck(void)
{
	SDA_OUT();														//sda???
	IIC_SCL(LOW);
	IIC_SDA(HIGH);
	delay_us(2);
	IIC_SCL(HIGH);
	delay_us(2);
	IIC_SCL(LOW);
}
 
/***************************************************************
 @Function			void IIC_Send_Byte(uint8_t txd)
 @Description		IIC??????
 @Input				txd : ????
 @Return			void
***************************************************************/
void IIC_Send_Byte(uint8_t txd)
{
	uint8_t t;
	
	SDA_OUT();														//sda???
	IIC_SCL(LOW);													//??????????
	for (t = 0; t < 8; t++)
	{
		IIC_SDA((txd & 0x80) >> 7);
		txd <<= 1;
		delay_us(2);
		IIC_SCL(HIGH);
		delay_us(2);
		IIC_SCL(LOW);
		delay_us(2);
	}
}
 
/***************************************************************
 @Function			uint8_t IIC_Read_Byte(unsigned char ack)
 @Description		?1???,ack=1?,??ACK,ack=0,??nACK
 @Input				ack
 @Return			????
***************************************************************/
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	
	SDA_IN();														//SDA?????
	for (i = 0; i < 8; i++)
	{
		IIC_SCL(LOW);
        delay_us(2);
		IIC_SCL(HIGH);
        receive <<= 1;
        if (READ_SDA) receive++;
		delay_us(1);
	}
	if (!ack)
		IIC_NAck();													//??nACK
	else
		IIC_Ack();													//??ACK
	
	return receive;
}
 
				 				   
void I2C_WriteByte(uint8_t device_addr, uint16_t addr, uint8_t data)
{
	IIC_Start();  
	
	IIC_Send_Byte(device_addr);	
	IIC_Wait_Ack();
	
	IIC_Send_Byte(addr&0xFF); 
	IIC_Wait_Ack(); 
	
	IIC_Send_Byte(data);  						   
	IIC_Wait_Ack(); 
	
	IIC_Stop();
	delay_us(2);
}
 
uint16_t I2C_ReadByte(uint8_t device_addr, uint16_t addr, uint8_t ByteNumToRead)
{	
	uint16_t data;
	
	IIC_Start();
  
	IIC_Send_Byte(device_addr);	
	IIC_Wait_Ack();
	
	IIC_Send_Byte(addr&0xFF);   
	IIC_Wait_Ack(); 
 
	IIC_Start();
	
	IIC_Send_Byte(device_addr+1);	   
	IIC_Wait_Ack();
	
	if(ByteNumToRead == 1)
	{
		data=IIC_Read_Byte(0);
	}
	else
	{
		data=IIC_Read_Byte(1);
		data=(data<<8)+IIC_Read_Byte(0);
	}
			
	IIC_Stop(); 
			
	return data;
}


