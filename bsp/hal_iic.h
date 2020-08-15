/******************************** END OF FLEE **********************************/
#ifndef __HAL_IIC_H
#define   __HAL_IIC_H


#define WRITE_ADDRESS 0xBE
#define READ_ADDRESS  0xBF

void IIC_Init(void);										
void IIC_Start(void);										
void IIC_Stop(void);										
uint8_t   IIC_Wait_Ack(void);									
void IIC_Ack(void);										
void IIC_NAck(void);										
void IIC_Send_Byte(uint8_t txd);									
uint8_t   IIC_Read_Byte(unsigned char ack);		

void I2C_WriteByte(uint8_t device_addr, uint16_t addr, uint8_t data);
uint16_t I2C_ReadByte(uint8_t device_addr, uint16_t addr, uint8_t ByteNumToRead);

#endif
 
