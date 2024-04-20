#include "stm32f10x.h"
/*
      CH1   CH2
TIM3  PA6   PA7
TIM4  PB6   PB7
TIM5  PA0   PA1
TIM8  PC6   PC7
*/

void TIM_Encoder_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//时基单元配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//输入滤波器的采样频率(对内部时钟的分频)
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 65536-1;					//ARR  
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1-1;				//PSC   fc=72MHz/(PSC+1)
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才有
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStructure);	
	
	//输入捕获通道(滤波器，极性选择[可两处地方配置])
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);//初始化默认参数，防止参数不全
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿代表极性不翻转
	//TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;信号不用传到后面的分频器和CCR，因此无需配置
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	TIM_ICInit(TIM4,&TIM_ICInitStructure);
	TIM_ICInit(TIM5,&TIM_ICInitStructure);
	TIM_ICInit(TIM8,&TIM_ICInitStructure);


	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿代表极性不翻转
	//TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;信号不用传到后面的分频器和CCR，因此无需配置
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	TIM_ICInit(TIM4,&TIM_ICInitStructure);
	TIM_ICInit(TIM5,&TIM_ICInitStructure);
	TIM_ICInit(TIM8,&TIM_ICInitStructure);
	
	/*编码器接口配置(根据编码器模式配置TIM->SMCR->SMS[2:0]Slave Mode Selection
	则CNT根据TI1FP1和TI2FP2的边沿向上/下计数*/
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Falling,TIM_ICPolarity_Rising);//重复配置极性反转
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Falling,TIM_ICPolarity_Rising);//重复配置极性反转
	TIM_EncoderInterfaceConfig(TIM5,TIM_EncoderMode_TI12,TIM_ICPolarity_Falling,TIM_ICPolarity_Rising);//重复配置极性反转
	TIM_EncoderInterfaceConfig(TIM8,TIM_EncoderMode_TI12,TIM_ICPolarity_Falling,TIM_ICPolarity_Rising);//重复配置极性反转



	TIM_Cmd(TIM3,ENABLE);
	TIM_Cmd(TIM4,ENABLE);
	TIM_Cmd(TIM5,ENABLE);
	TIM_Cmd(TIM8,ENABLE);

	
}

/*左前编码器*/
int32_t TIM_Encoder1_Get(void){
	static int16_t temp1 =0;
	static int32_t outcome1=0;
	temp1=TIM_GetCounter(TIM3);
	TIM_SetCounter(TIM3,0);
	outcome1+=temp1;
	return outcome1;
}

/*左后编码器*/
int32_t TIM_Encoder2_Get(void){
	static int16_t temp2 =0;
	static int32_t outcome2=0;
	temp2=TIM_GetCounter(TIM4);
	TIM_SetCounter(TIM4,0);
	outcome2+=temp2;
	return outcome2;
}

/*右前编码器*/
int32_t TIM_Encoder3_Get(void){
	static int16_t temp3 =0;
	static int32_t outcome3=0;
	temp3=TIM_GetCounter(TIM5);
	TIM_SetCounter(TIM5,0);
	outcome3+=temp3;
	return outcome3;
}

/*右后编码器*/
int32_t TIM_Encoder4_Get(void){
	static int16_t temp4 =0;
	static int32_t outcome4=0;
	temp4=TIM_GetCounter(TIM8);
	TIM_SetCounter(TIM8,0);
	outcome4+=temp4;
	return outcome4;
}

int32_t Get_Distance_X(void){
	return TIM_Encoder1_Get()-TIM_Encoder2_Get()-TIM_Encoder3_Get()+TIM_Encoder4_Get();
	
}

int32_t Get_Distance_Y(void){
	return TIM_Encoder1_Get()+TIM_Encoder2_Get()+TIM_Encoder3_Get()+TIM_Encoder4_Get();
	
}





