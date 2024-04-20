#include "stm32f10x.h"
#include "Delay.h"
//��ΪSDAһ������Ϊ˫���ķ��ͽ�����·����˱���˫����һ������ߵ͵�ƽ��ɣ����ѡ��©ģʽ�������������ʹ��

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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//I2C�����迪©ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_InitStructure.GPIO_Pin);
	
}

void MyI2C_Start(void){//SDA���½���
	
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
	
}

void MyI2C_Stop(void){//SCL��������
	
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
	
}
void MyI2C_SendByte(uint8_t Byte){
	uint8_t i;
	for(i=0;i<=7;i++){//��λ����
		MyI2C_W_SDA(Byte & (0x80>>i));  //(BitAction)0x00 ��0x80
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);//��SCL�ߵ�ƽʱȡSDA����Ч����
	}
	
	
}

uint8_t MyI2C_ReceiveByte(void){
	uint8_t Byte=0x00,i;
	MyI2C_W_SDA(1); //�����ͷ�SDA����Ȩ(����̬),׼����������
	for(i=0;i<=7;i++){
		MyI2C_W_SCL(1); //���ߵ�ƽ����SCL�ߵ�ƽ������
		if (MyI2C_R_SDA()==1){Byte |= (0x80>>i);}
		MyI2C_W_SCL(0);
	}
	return Byte;
	
}

void MyI2C_SendAck(uint8_t AckBit){
/*
AckBit=1Ϊ��Ӧ�𣬴ӻ�ֹͣ������������
AckBit=0ΪӦ�𣬴ӻ�������������
	*/
		MyI2C_W_SDA(AckBit);  
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);

}

uint8_t MyI2C_ReceiveAck(void){
	uint8_t AckBit;
	MyI2C_W_SDA(1); 
	MyI2C_W_SCL(1); 
	/*�����豸��Ӧ:
	���������������©��������������ã����ؽ��Ϊ1
	���豸����Ӧ:
	���豸����дSDAΪ0�����ؽ��Ϊ0
	*/
	AckBit = MyI2C_R_SDA(); 
	MyI2C_W_SCL(0);
	
	return AckBit;
	
}
