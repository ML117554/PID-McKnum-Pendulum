#include "stm32f10x.h"
#include "OLED.h"
#include "Delay.h"

void Timer_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	TIM_InternalClockConfig(TIM1);					//失能从模式，使用内部时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//输入滤波器的采样频率(对内部时钟的分频)
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 50;					//设定自动重装载寄存器ARR(16 bit)的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1;				//设定预分频器PSC(16 bit)的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才有
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);//修改寄存器DMA/中断使能寄存器DIER，使能中断输出到NVIC
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM1,ENABLE); //使能计数器TIM_CR1_CEN位 置1
}




