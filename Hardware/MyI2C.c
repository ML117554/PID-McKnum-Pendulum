#include "stm32f10x.h"
#include "Delay.h"
//因为SDA一条线作为双方的发送接收线路，因此避免双方万一都输出高低电平造成，因此选择开漏模式，配合上拉电阻使用

#define SCL_PORT    GPIOB
#define SCL_PIN     GPIO_Pin

void MyI2C_W_SCL(uint8_t BitValue){
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,(BitAction)BitValue);
	Delay_us(10);
	
}
void MyI2C_W_SDA(uint8_t BitValue){
	GPIO_WriteBit(GPIOB,GPIO_Pin_11,(BitAction)BitValue);
	Delay_us(10);
	
}
uint8_t MyI2C_R_SDA(){
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
	Delay_us(10);
	return BitValue;	
}


void MyI2C_Init(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//I2C总线需开漏模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_InitStructure.GPIO_Pin);
	
}

void MyI2C_Start(void){//SDA先下降沿
	
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
	
}

void MyI2C_Stop(void){//SCL先上升沿
	
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
	
}
void MyI2C_SendByte(uint8_t Byte){
	uint8_t i;
	for(i=0;i<=7;i++){//高位优先
		MyI2C_W_SDA(Byte & (0x80>>i));  //(BitAction)0x00 或0x80
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);//在SCL高电平时取SDA的有效数据
	}
	
	
}

uint8_t MyI2C_ReceiveByte(void){
	uint8_t Byte=0x00,i;
	MyI2C_W_SDA(1); //主机释放SDA控制权(高阻态),准备读入数据
	for(i=0;i<=7;i++){
		MyI2C_W_SCL(1); //拉高电平，在SCL高电平读数据
		if (MyI2C_R_SDA()==1){Byte |= (0x80>>i);}
		MyI2C_W_SCL(0);
	}
	return Byte;
	
}

void MyI2C_SendAck(uint8_t AckBit){
/*
AckBit=1为非应答，从机停止继续传输数据
AckBit=0为应答，从机继续传输数据
	*/
		MyI2C_W_SDA(AckBit);  
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);

}

uint8_t MyI2C_ReceiveAck(void){
	uint8_t AckBit;
	MyI2C_W_SDA(1); 
	MyI2C_W_SCL(1); 
	/*若无设备响应:
	则由于主从输出开漏和上拉电阻的作用，返回结果为1
	若设备有响应:
	从设备主动写SDA为0，返回结果为0
	*/
	AckBit = MyI2C_R_SDA(); 
	MyI2C_W_SCL(0);
	
	return AckBit;
	
}
