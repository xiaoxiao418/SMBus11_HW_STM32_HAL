#ifndef _SMBus11_HW_H_
#define _SMBus11_HW_H_

#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include <inttypes.h>

#define SMBus_NAME             I2C1
#define SMBus_RCC_Periph       RCC_APB1Periph_I2C1
#define SMBus_Port             GPIOB
#define SMBus_SCL_Pin          GPIO_Pin_8
#define SMBus_SDA_Pin          GPIO_Pin_9
#define SMBus_SCL_PinSource    GPIO_PinSource8
#define SMBus_SDA_PinSource    GPIO_PinSource9
#define SMBus_RCC_Port         RCC_AHB1Periph_GPIOB
#define SMBus_Speed            100000
#define SMBus_GPIO_AF          GPIO_AF_I2C1
#define SMBus_Max_Delay_Cycles 10000


/* 从机设备地址，设备写地址0x16,读地址0x17 */
#define SLAVE_ADDE			0x16


/* Smart Battery Slave Functions */
#define	ManufacturerAccess			    0x00
#define	RemainingCapacityAlarm			0x01
#define	RemainingTimeAlarm			    0x02
#define	BatteryMode			    		0x03
#define	AtRate			    			0x04
#define	AtRateTimeToFull			    0x05
#define	AtRateTimeToEmpty			    0x06
#define	AtRateOK			    		0x07
#define	Temperature			    		0x08
#define	Voltage			    			0x09
#define	Current			    			0x0a
#define	AverageCurrent			    	0x0b

#define	ChargingCurrent			    	0x14
#define	ChargingVoltage			    	0x15
#define	BatteryStatus			    	0x16
#define	CycleCount			    		0x17
#define	DesignCapacity			    	0x18
#define	DesignVoltage			    	0x19
#define	SpecificationInfo			    0x1a
#define	ManufactureDate			    	0x1b
#define	SerialNumber			    	0x1c

#define	ManufacturerName			    0x20
#define	DeviceName			    		0x21
#define	DeviceChemistry			    	0x22
#define	ManufacturerData			    0x23
#define	OptionalMfgFunction5			0x2f



void SMBus_HW_init(void);
uint8_t SMBus_Write_Word(uint8_t slaveAddr, uint16_t data, uint8_t WriteCmd);
void SMBus_Read_Word(uint8_t slaveAddr, uint16_t* data, uint8_t ReadAddr);
uint8_t SMBus_Write_Byte(uint8_t slaveAddr, uint8_t WriteCmd);

uint16_t SMBus_Get_Voltage(void);
uint16_t SMBus_Get_Temp(void);
void SMBus_Get_ManuName(void);
void SMBus_FindAddr_Test(void);



#endif  /* _SMBus11_HW_H_ */
