#include "stm32f10x.h"
/*

EXTI_Encoder:(C8T6外部中断模拟编码器计数)
m0   PB4(EXTI)       PB5
m1   PB6             PB7(EXTI) 
m2   PA0(EXTI)       PA1
m3   PB0             PB1(EXTI) 

*/
int16_t Encoder_Count_M0=0;
int16_t Encoder_Count_M1=0;
int16_t Encoder_Count_M2=0;
int16_t Encoder_Count_M3=0;
int16_t temp0=0,temp1=0,temp2=0,temp3=0;
int16_t velocity_x=0;
int16_t velocity_y=0;

void EXTI_Encoder_Init(void){
	//1.RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//2.GPIO
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;// enum GPIOMode_TypeDef
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;   //enum GPIO_Pins_define
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //enum GPIOSpeed_TypeDef
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	

	//3.AFIO
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource7);
	//4.EXTI
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0|EXTI_Line1|EXTI_Line4|EXTI_Line7;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	//5.NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	
}

void EXTI4_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line4)==SET){
		
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==0){//pin_0下降沿触发中断读取pin_1引脚电平
			Encoder_Count_M0++;
		}
		else{
			Encoder_Count_M0--;
		}
		
		EXTI_ClearITPendingBit(EXTI_Line4);//挂起寄存器PR写入1清零该位，中断线触发时自动置1
	

		}
}


void EXTI9_5_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line7)==SET){
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==0){//pin_0下降沿触发中断读取pin_1引脚电平
			Encoder_Count_M1--;
		}
		else{
			Encoder_Count_M1++;
		}
		
		EXTI_ClearITPendingBit(EXTI_Line7);//挂起寄存器PR写入1清零该位，中断线触发时自动置1
	

	}

}


void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0)==SET){
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0){//pin_0下降沿触发中断读取pin_1引脚电平
			Encoder_Count_M2--;
		}
		else{
			Encoder_Count_M2++;
		}
		
		EXTI_ClearITPendingBit(EXTI_Line0);//挂起寄存器PR写入1清零该位，中断线触发时自动置1
	}
}
void EXTI1_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line1)==SET){
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==0){//pin_0下降沿触发中断读取pin_1引脚电平
			Encoder_Count_M3++;
		}
		else{
			Encoder_Count_M3--;
		}
		
		EXTI_ClearITPendingBit(EXTI_Line1);//挂起寄存器PR写入1清零该位，中断线触发时自动置1
	}
}



int32_t EXTI_Get_Distance_X(void){
	return Encoder_Count_M0-Encoder_Count_M1-Encoder_Count_M2+Encoder_Count_M3;
	
}

int32_t EXTI_Get_Distance_Y(void){
	return Encoder_Count_M0+Encoder_Count_M1+Encoder_Count_M2+Encoder_Count_M3;
	
}



