#include "stm32f10x.h"  
#include "Menu.h"
#include "Delay.h"
#include "Control.h"
void EXTI_Key_Init(void)
{	
	//1.RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//2.GPIO
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;// enum GPIOMode_TypeDef
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;   //enum GPIO_Pins_define
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //enum GPIOSpeed_TypeDef
	GPIO_Init(GPIOB,&GPIO_InitStructure);	

	//3.AFIO

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);
	//4.EXTI
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line12|EXTI_Line13;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	//5.NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	

	
}

void EXTI15_10_IRQHandler(void)
{			
	Delay_ms(5);  //消抖

  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0)	//index+
	{
		/*
		Menu_Index++;
		if(Menu_Index==6){
			Menu_Index=1;
			Refresh=1;
		}
		if(Menu_Index==5){
			Refresh=1;
		}*/
		if(Menu_Index==4)  Position_KD-=Amplitude4;
		
	}		
	 if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==0)	//PID+ 
	{
			    if(Menu_Index==1)  Balance_KP+=Amplitude1;
	  else	if(Menu_Index==2)  Balance_KD+=Amplitude2;
		else  if(Menu_Index==3)  Position_KP+=Amplitude3;
		else  if(Menu_Index==4)  Position_KD+=Amplitude4;
		else  if(Menu_Index==5)  Temp_FB_PWM+=Amplitude_FB_PWM;
	}		

  EXTI_ClearITPendingBit(EXTI_Line12); //清除LINE11上的中断标志位  
	EXTI_ClearITPendingBit(EXTI_Line13); //清除LINE12上的中断标志位 
}






