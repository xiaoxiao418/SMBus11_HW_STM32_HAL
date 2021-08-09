#include "smbus11_hw.h"
#include "delay.h"
#include "usart.h"
#include <string.h> 



void SMBus_HW_init()
{
    I2C_InitTypeDef SMBus_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable I2C and GPIO clocks */
    RCC_APB1PeriphClockCmd(SMBus_RCC_Periph, ENABLE);
    RCC_AHB1PeriphClockCmd(SMBus_RCC_Port, ENABLE);

	GPIO_PinAFConfig(SMBus_Port, SMBus_SCL_PinSource, SMBus_GPIO_AF);
    GPIO_PinAFConfig(SMBus_Port, SMBus_SDA_PinSource, SMBus_GPIO_AF);
	
    /* Configure SMBus pins: SCL and SDA */
    GPIO_InitStructure.GPIO_Pin = SMBus_SCL_Pin | SMBus_SDA_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(SMBus_Port, &GPIO_InitStructure);

    /* SMBus configuration */
    SMBus_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
    SMBus_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    SMBus_InitStructure.I2C_OwnAddress1 = 0x01;
    SMBus_InitStructure.I2C_Ack = I2C_Ack_Enable;
    SMBus_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    SMBus_InitStructure.I2C_ClockSpeed = SMBus_Speed;

    /* Apply SMBus configuration after enabling it */
    I2C_Init(SMBus_NAME, &SMBus_InitStructure);
    I2C_Cmd(SMBus_NAME, ENABLE);
}

/**
 * @brief  Writes one byte to the peripheral device.
 * @param  slaveAddr : slave address.
 * @param  pBuffer : pointer to the buffer  containing the data to be written to the peripheral device.
 * @param  WriteAddr : address of the register in which the data will be written.
 * @retval None
 */
uint8_t SMBus_Write_Word(uint8_t slaveAddr, uint16_t data, uint8_t WriteCmd)
{
	uint32_t counter = SMBus_Max_Delay_Cycles;
    I2C_GenerateSTART(SMBus_NAME, ENABLE);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT) && counter) --counter;
    if(counter == 0) {
		printf(" 01 \n");
	    return 0;
    }

    I2C_Send7bitAddress(SMBus_NAME, slaveAddr, I2C_Direction_Transmitter);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && counter) --counter;
    if(counter == 0) {
		printf(" 02 \n");
	    return 0;
    }

    I2C_SendData(SMBus_NAME, WriteCmd);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
    if(counter == 0) {
		printf(" 03 \n");
	    return 0;
    }

    I2C_SendData(SMBus_NAME, data & 0xFF);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
    if(counter == 0) {
		printf(" 04 \n");
	    return 0;
    }
	
    I2C_SendData(SMBus_NAME, data >> 8);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
    if(counter == 0) {
		printf(" 05 \n");
	    return 0;
    }
	
    I2C_GenerateSTOP(SMBus_NAME, ENABLE);
	return 1;
}

/* 写块 */
uint8_t SMBus_Write_Block(uint8_t slaveAddr, uint8_t* data, uint8_t size, uint8_t WriteCmd)
{
	uint8_t i;
	uint32_t counter = SMBus_Max_Delay_Cycles;

    I2C_GenerateSTART(SMBus_NAME, ENABLE);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT) && counter) --counter;
    if(counter == 0) {
		printf(" 01 \n");
	    return 0;
    }

    I2C_Send7bitAddress(SMBus_NAME, slaveAddr, I2C_Direction_Transmitter);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && counter) --counter;
    if(counter == 0) {
		printf(" 02 \n");
	    return 0;
    }

    I2C_SendData(SMBus_NAME, WriteCmd);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
    if(counter == 0) {
		printf(" 03 \n");
	    return 0;
    }

    I2C_SendData(SMBus_NAME, size);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
    if(counter == 0) {
		printf(" 04 \n");
	    return 0;
    }
	
	for(i=0; i<size; i++)
	{
		I2C_SendData(SMBus_NAME, data[i]);
		counter = SMBus_Max_Delay_Cycles;
		while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
		if(counter == 0) {
			return 0;
		}
	}

    I2C_GenerateSTOP(SMBus_NAME, ENABLE);
	return 1;
}

/* 写字节 */
uint8_t SMBus_Write_Byte(uint8_t slaveAddr, uint8_t WriteCmd)
{
	uint32_t counter = SMBus_Max_Delay_Cycles;
    I2C_GenerateSTART(SMBus_NAME, ENABLE);
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT) && counter) --counter;
    if(counter == 0) {
		printf(" 01 \n");
	    return 0;
    }

    I2C_Send7bitAddress(SMBus_NAME, slaveAddr, I2C_Direction_Transmitter);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && counter) --counter;
    if(counter == 0) {
		printf(" 02 \n");
	    return 0;
    }

    I2C_SendData(SMBus_NAME, WriteCmd);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
    if(counter == 0) {
		printf(" 03 \n");
	    return 0;
    }
	
    I2C_GenerateSTOP(SMBus_NAME, ENABLE);
	printf("写字节完成");
	return 1;
}

/**
 * @brief  Reads a block of data from the peripheral device.
 * @param  slaveAddr  : slave address.
 * @param  pBuffer : pointer to the buffer that receives the data read from the peripheral device.
 * @param  ReadAddr : Peripheral device's internal address to read from.
 * @param  NumByteToRead : number of bytes to read from the peripheral device.
 * @retval None
 */
void SMBus_Read_Word(uint8_t slaveAddr, uint16_t* data, uint8_t cmd)
{
	uint8_t buff[2];
	uint32_t counter = SMBus_Max_Delay_Cycles;
	
    while (I2C_GetFlagStatus(SMBus_NAME, I2C_FLAG_BUSY) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    I2C_GenerateSTART(SMBus_NAME, ENABLE);

    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT) && counter) --counter;
    if(counter == 0) {
	    return;
    }
    I2C_Send7bitAddress(SMBus_NAME, slaveAddr, I2C_Direction_Transmitter);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    I2C_Cmd(SMBus_NAME, ENABLE);

    I2C_SendData(SMBus_NAME, cmd);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    I2C_GenerateSTART(SMBus_NAME, ENABLE);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    I2C_Send7bitAddress(SMBus_NAME, slaveAddr|0x01, I2C_Direction_Receiver);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED) && counter) --counter;
    if(counter == 0) {
	    return;
    }
    buff[0] = I2C_ReceiveData(SMBus_NAME);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED) && counter) --counter;
    if(counter == 0) {
	    return;
    }
    buff[1] = I2C_ReceiveData(SMBus_NAME);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED) && counter) --counter;
    if(counter == 0) {
	    return;
    }
	
    I2C_GenerateSTOP(SMBus_NAME, ENABLE);

    *data = ((uint16_t)buff[1] << 8) | buff[0];  //MSB
	printf("info=%d\n", *data);
}

/* 读块 */
void SMBus_Read_Block(uint8_t slaveAddr, uint8_t* data, uint8_t cmd)
{
	uint8_t i,size = 0;
	uint8_t buff[20] = {0};
	uint32_t counter = SMBus_Max_Delay_Cycles;

    while (I2C_GetFlagStatus(SMBus_NAME, I2C_FLAG_BUSY) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    I2C_GenerateSTART(SMBus_NAME, ENABLE);

    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT) && counter) --counter;
    if(counter == 0) {
	    return;
    }
    I2C_Send7bitAddress(SMBus_NAME, slaveAddr, I2C_Direction_Transmitter);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    I2C_Cmd(SMBus_NAME, ENABLE);

    I2C_SendData(SMBus_NAME, cmd);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    I2C_GenerateSTART(SMBus_NAME, ENABLE);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_MODE_SELECT) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    I2C_Send7bitAddress(SMBus_NAME, slaveAddr|0x01, I2C_Direction_Receiver);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && counter) --counter;
    if(counter == 0) {
	    return;
    }

    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED) && counter) --counter;
    if(counter == 0) {
	    return;
    }
	
	/* 有多少个字节 */
	buff[0] = I2C_ReceiveData(SMBus_NAME);
    counter = SMBus_Max_Delay_Cycles;
    while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED) && counter) --counter;
    if(counter == 0) {
	    return;
    }
	
	for(i=1; i<buff[0]+1; i++)
	{
		buff[i] = I2C_ReceiveData(SMBus_NAME);
		counter = SMBus_Max_Delay_Cycles;
		while (!I2C_CheckEvent(SMBus_NAME, I2C_EVENT_MASTER_BYTE_RECEIVED) && counter) --counter;
		if(counter == 0) {
			return;
		}
	}
	
    I2C_GenerateSTOP(SMBus_NAME, ENABLE);
	
	memcpy(data, buff+1, buff[0]);
	printf("str=%s,size=%d\n", buff+1, strlen((char*)buff)-1);
	
	for(i=1; i<buff[0]+1; i++)
		printf("%c\n", buff[i]);
}

/* SMBus读字信息 */
uint16_t SMBus_Word_Get_Info(uint8_t slaveAddr, uint8_t slaveCmd)
{
	uint16_t res;
	SMBus_Read_Word(slaveAddr, &res, slaveCmd);
	return res;
}

/* SMBus读块信息 */
void SMBus_Block_Get_Info(uint8_t slaveAddr, uint8_t* RData, uint8_t slaveCmd)
{
	SMBus_Read_Block(slaveAddr, RData, slaveCmd);
}


/* 读取电池电压值 */
uint16_t SMBus_Get_Voltage(void)
{
	uint16_t Value = 0;
	
	Value = SMBus_Word_Get_Info(SLAVE_ADDE, Voltage);
	printf("Vol=%d\n", Value);
	return Value;		
}

/* 读取电池内部温度（单位是K，不是摄氏度），实际温度范围是 -20°C to +75°C */
uint16_t SMBus_Get_Temp(void)
{
	uint16_t Value = 0;
	
	Value = SMBus_Word_Get_Info(SLAVE_ADDE, Temperature);
	printf("Temp=%d\n", Value);
	return Value;		
}

/* 读取制造商名称 */
void SMBus_Get_ManuName(void)
{
	uint8_t Value[20] = {0};
	SMBus_Block_Get_Info(SLAVE_ADDE, Value, ManufacturerName);	
}

/* 测试从设备地址，这里的地址是设备写地址（8bit） */
void SMBus_FindAddr_Test(void)
{
	uint8_t i;
	int8_t Value = 0;

	for(i=0; i<255; i++)
	{
		Value = SMBus_Write_Byte(i, 0x09);

		if(Value != 0)
		{
			printf(" i = %#x\n", i);
		}

		delay_ms(10);
	}
	
	printf("测试结束");
}











