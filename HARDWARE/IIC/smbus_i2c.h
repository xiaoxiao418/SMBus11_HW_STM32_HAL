#ifndef _SMBUS_I2C_H_
#define _SMBUS_I2C_H_

#include "stm32f4xx.h" 

#define SMBus_GPIO_Port           GPIOB
#define SMBus_SCL_Pin             (uint16_t)GPIO_Pin_8		//PB8-时钟引脚
#define SMBus_SDA_Pin             (uint16_t)GPIO_Pin_9		//PB9-数据引脚

#define SMBus_SCL_H               GPIO_WriteBit(SMBus_GPIO_Port,SMBus_SCL_Pin,Bit_SET)
#define SMBus_SCL_L               GPIO_WriteBit(SMBus_GPIO_Port,SMBus_SCL_Pin,Bit_RESET)
#define SMBus_SDA_H               GPIO_WriteBit(SMBus_GPIO_Port,SMBus_SDA_Pin,Bit_SET)
#define SMBus_SDA_L               GPIO_WriteBit(SMBus_GPIO_Port,SMBus_SDA_Pin,Bit_RESET)
#define SMBus_READ_SDA            GPIO_ReadInputDataBit(SMBus_GPIO_Port,SMBus_SDA_Pin)

#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}		//SDA输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} 	//SDA输出模式

void SMBus_Init(void);
uint8_t SMBus_WriteOneByte(uint8_t WriteAddr, uint8_t DataToWrite);
uint8_t SMBus_ReadOneByte(uint16_t ReadAddr);
int16_t Get_Battery_Info(uint8_t slaveAddr,uint8_t slaveAddr1, uint8_t Comcode);


#endif  /* _SMBUS_I2C_H_ */

