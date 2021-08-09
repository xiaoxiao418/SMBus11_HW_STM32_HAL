#include "smbus_i2c.h"
#include "delay.h"
#include "usart.h"

/********************************
*备注：使用I2C1
       PB8————SCL，PB9————SDA
********************************/
void SMBus_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

	//GPIOB8,B9初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式  
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	SMBus_SCL_H;
	SMBus_SDA_H;
}
 
 
//产生IIC起始信号
void SMBus_Start(void)
{
    SDA_OUT();
	SMBus_SDA_H; 
    delay_us(1);
    SMBus_SCL_H;
    delay_us(5);
    SMBus_SDA_L;//START:when CLK is high,DATA change form high to low
    delay_us(5);
    SMBus_SCL_L;//钳住I2C总线，准备发送或接收数据
	delay_us(2);
}  
  

//产生IIC停止信号
void SMBus_Stop(void)
{
    SDA_OUT();
    SMBus_SCL_L;
    delay_us(1);
    SMBus_SDA_L;
    delay_us(5);
    SMBus_SCL_H;
    delay_us(5);   
    SMBus_SDA_H;//发送I2C总线结束信号
//    delay_us(5);                                       
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t SMBus_Wait_Ack(void)
{
    uint8_t ucErrTime=0;
    SDA_IN();      //SDA设置为输入
    SMBus_SDA_H;
	delay_us(6);    
    SMBus_SCL_H;
	delay_us(6);  
    while(SMBus_READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            SMBus_Stop();
            return 1;
        }
    }
    SMBus_SCL_L;//时钟输出0  
    delay_us(2);
    return 0; 
}
  
   
//产生ACK应答
void SMBus_Ack(void)
{
    SMBus_SCL_L;
    SDA_OUT();
    SMBus_SDA_L;
    delay_us(6);
    SMBus_SCL_H;
    delay_us(6);
    SMBus_SCL_L;
}


//不产生ACK应答         
void SMBus_NAck(void)
{
    SMBus_SCL_L;
    SDA_OUT();
    SMBus_SDA_H;
    delay_us(6);
    SMBus_SCL_H;
    delay_us(6);
    SMBus_SCL_L;
}                                       
  
  
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答          
void SMBus_Send_Byte(uint8_t txd)
{                       
    uint8_t t;  
    SDA_OUT();       
    SMBus_SCL_L;	//拉低时钟开始数据传输
    for(t=0;t<8;t++){
		
        if((txd&0x80)>>7)
        {
            SMBus_SDA_H;
        }
        else
        {
            SMBus_SDA_L;
        }
        txd<<=1; 
        delay_us(2);  
        SMBus_SCL_H;
        delay_us(5);
        SMBus_SCL_L; 
        delay_us(5);
    }
}
  
  
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK  
uint8_t SMBus_Read_Byte(uint8_t ack)
{
    unsigned char i,receive=0;
    SDA_IN();		//SDA设置为输入
	for(i=0;i<8;i++ ){
		
		SMBus_SCL_L;
		delay_us(8);
		SMBus_SCL_H;
		receive<<=1;
		if(SMBus_READ_SDA)
			receive++;  
		delay_us(5);
    }                   
	if (!ack)
        SMBus_NAck();//发送nACK
    else
        SMBus_Ack(); //发送ACK  
	
    return receive;
}


uint8_t SMBus_WriteOneByte(uint8_t WriteAddr, uint8_t DataToWrite)
{
	uint8_t status = 0;
	
    SMBus_Start();  
	SMBus_Send_Byte(WriteAddr);   //发送器件地址0XA0,写数据
	status = SMBus_Wait_Ack();	   
//	if(status == 1)
//	{
//		printf("第一个应答fail，%d\n", WriteAddr);
//		return status;
//	}
	
    SMBus_Send_Byte(0);   		//发送低地址
	delay_us(90);
	status = SMBus_Wait_Ack(); 	 
//	if(status == 1)
//	{
//		printf("第二个应答fail\n");
//		return status;
//	}
	
	SMBus_Send_Byte(DataToWrite);     //发送字节							   
	status = SMBus_Wait_Ack(); 
//	if(status == 1)
//	{
//		printf("第三个应答fail\n");
//		return status;
//	}
	
    SMBus_Stop();//产生一个停止条件 
	delay_ms(10);	 
	
	return status;
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 time=10;	
uint8_t SMBus_ReadOneByte(uint16_t ReadAddr)
{			  
	u8 temp=0;		  	    																 
    SMBus_Start();  
	SMBus_Send_Byte(ReadAddr+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据
	if(SMBus_Wait_Ack()==1)
	{
       printf(" 1 fail!\r\n");
       return -1;
    }
    SMBus_Send_Byte(ReadAddr%256);   //发送低地址
	delay_us(time);
	if(SMBus_Wait_Ack()==1)
	{
       printf(" 2 fail!\r\n");
       return -1;
    }
	SMBus_Start();  	 	   
	SMBus_Send_Byte(ReadAddr|0x01);           //进入接收模式			   
	if(SMBus_Wait_Ack()==1)
	{
       printf(" 3 fail!\r\n");
       return -1;
    }
    temp=SMBus_Read_Byte(0);		   
    SMBus_Stop();//产生一个停止条件	    
	return temp;
}

int16_t Get_Battery_Info(uint8_t slaveAddr,uint8_t slaveAddr1, uint8_t Comcode)
{
     int16_t Value;
     uint8_t data[2] = {0};
     SMBus_Start(); 
 
     SMBus_Send_Byte(slaveAddr);	//发送地址
	 if(SMBus_Wait_Ack() == 1)
     {
       printf("SlaveAddr wait ack fail!\r\n");
         return -1;
     }
 
     SMBus_Send_Byte(Comcode);
     delay_us(90);
     if(SMBus_Wait_Ack() == 1)
     {
       printf("Comcode wait ack fail!\r\n");
         return -1;
     }
     
	 SMBus_Start();
     SMBus_Send_Byte(slaveAddr1);//发送地址
     if(SMBus_Wait_Ack() == 1)
     {
       printf("slaveAddr+1 wait ack fail!\r\n");
         return -1;
     }
     delay_us(60);
 
     data[0] = SMBus_Read_Byte(1);
     SMBus_Ack();
     delay_us(125);
     data[1] = SMBus_Read_Byte(0);
     SMBus_NAck();
     delay_us(58);
     SMBus_Stop();
//     printf("data[0]:%x,data[1]:%x\r\n",data[0],data[1]);
     Value = (data[0] |(data[1]<<8));
     delay_us(100);
     return Value;
	 
 }





