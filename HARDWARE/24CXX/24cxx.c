#include "24cxx.h" 
#include "delay.h" 	
#include "smbus11_hw.h"
#include "usart.h"
#include "smbus_i2c.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//24CXX 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化IIC接口
void AT24CXX_Init(void)
{
//	IIC_Init();//IIC初始化
//	SMBus_init();
//	SMBus_Init();
	SMBus_HW_init();
}
//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址	    
	}
	else 
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据
	
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}
	else 
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据
	
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

/*---------------------------------------------------------*/
uint8_t Write_Data1[2] = {0};

void Delay_ms(void)
{
	uint8_t i,j,z;
	for(i=0; i<50; i++)
//		for(j=0; j<250; j++)
			for(z=0; z<50; z++)
				{}
}

uint8_t AT24C02_WriteOneByte(u16 WriteAddr, u8 DataToWrite)
{
	uint8_t status = 0;
	
    IIC_Start();  
	IIC_Send_Byte(WriteAddr);   //发送器件地址0XA0,写数据
	status = IIC_Wait_Ack();	   
//	if(status == 1)
//	{
//		printf("第一个应答fail，%d\n", WriteAddr);
//		return status;
//	}
	
    IIC_Send_Byte(0);   		//发送低地址
	status = IIC_Wait_Ack(); 	 
	if(status == 1)
	{
		printf("第二个应答fail\n");
		return status;
	}
	
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	status = IIC_Wait_Ack(); 
//	if(status == 1)
//	{
//		printf("第三个应答fail\n");
//		return status;
//	}
	
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
	
	return status;
}

int16_t Get2_Battery_Info(uint8_t slaveAddr, uint8_t Comcode)
{
     int16_t Value;
     uint8_t data[2] = {0};
     IIC_Start(); 
 
     IIC_Send_Byte(slaveAddr);	//发送地址
	 if(IIC_Wait_Ack() == 1)
     {
//       printf("SlaveAddr wait ack fail!\r\n");
         return -1;
     }
 
     IIC_Send_Byte(Comcode);
     delay_us(90);
     if(IIC_Wait_Ack() == 1)
     {
//       printf("Comcode wait ack fail!\r\n");
         return -1;
     }
     
	 IIC_Start();
     IIC_Send_Byte(slaveAddr+0x01);//发送地址
     if(IIC_Wait_Ack() == 1)
     {
//       printf("slaveAddr+1 wait ack fail!\r\n");
         return -1;
     }
     delay_us(60);
 
     data[0] = IIC_Read_Byte(1);
     IIC_Ack();
     delay_us(125);
     data[1] = IIC_Read_Byte(1);
     IIC_NAck();
     delay_us(58);
     IIC_Stop();
//     printf("data[0]:%x,data[1]:%x\r\n",data[0],data[1]);
     Value = (data[1] |(data[0]<<8));
     delay_us(100);
     return Value;
	 
 }

void SMBus_Test(void)
{
	uint8_t i;
	uint8_t* data;
	int16_t Value = 0;

	for(i=21; i<22; i++)
	{
//		Value = AT24C02_WriteOneByte(0x16, 0x09);
//		Value = SMBus_Write_Test(i);
//		Value = SMBus_WriteOneByte(i, 0x09);
//		Value = SMBus_ReadOneByte(i);
//		Value = Get_Battery_Info(0x16, 0x17, 0x09);
//		Value = SMBus_ReadOneByte(0x09);
//		Value = SMBus_WriteWord(i, 0x09, 0x09);
//		Value = SMBus_Word_Get_Info(0x16, 0x03);
		
//		SMBus_Block_Get_Info(0x16, data, 0x020);

//		if(Value != 0)
//		{
//			printf("   i=%c,%c,%d,%#x\n", Value&0xFF, (Value&0xFF00)>>8,Value, i);
//		}

		Delay_ms();
	}
	
//	status = SMBus_ReadOneByte(0x08);
	
	
	printf("结束：status=%d\n", Value);
}















