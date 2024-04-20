#include "stm32f10x.h"
#include "OLED.h"
#include "Control.h"
//**********************PWM MOTOR***************************************
//PWM ����Ƚ�
void PWM_Motor_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//TIM2ͨ�ö�ʱ��ƥ��APB1����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	/*
	GPIO_PartialRemap1_TIM2������ӳ��:
	CH1-4:PA15,PB3,PA2,PA3
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//��AFIO��ӳ��ʱ��
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);//TIM2������ӳ�䣬TIM2��ch1 ��PA0��PA15
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //������������������ſ���Ȩ����Ƭ������(TIM2_CH1)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_15;//CH3|CH4|CH1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//CH2
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
	
	
	TIM_InternalClockConfig(TIM2);					//ʧ�ܴ�ģʽ��ʹ���ڲ�ʱ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//�����˲����Ĳ���Ƶ��(���ڲ�ʱ�ӵķ�Ƶ)
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//����ģʽ
	TIM_TimeBaseInitStructure.TIM_Period = 1200-1;					//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720-1;				//PSC  PWMƵ��20KHz����ʱ�������ת�˶�����������
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//�ظ����������߼���ʱ������
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);//Ĭ�ϳ�ʼ��ȫ����������ֹ������ȷ���Ĵ���
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;								//CCR
  TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM2,ENABLE);
	
}


/*
3.�������
m0: PA4,PA5
m1: PA11,PA12
m2: PB8,PB9
m3: PB10,PB11
*/
void Motor_DirectPin_Init(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}



void Motor_SetSpeed(int16_t Speed1,int16_t Speed2,int16_t Speed3,int16_t Speed4){
	/*-----------------------m0--------------------------*/
	if(Speed1>=0){
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
		TIM_SetCompare1(TIM2,Speed1);
		
	}
	else{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
		TIM_SetCompare1(TIM2,-Speed1);
	}	
	
	/*-----------------------m1--------------------------*/
		if(Speed2>=0){
		GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_RESET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_12,Bit_SET);
		TIM_SetCompare2(TIM2,Speed2);
		
	}
	else{
		GPIO_WriteBit(GPIOA,GPIO_Pin_11,Bit_SET);
		GPIO_WriteBit(GPIOA,GPIO_Pin_12,Bit_RESET);
		TIM_SetCompare2(TIM2,-Speed2);
	}

	/*-----------------------m2--------------------------*/
		if(Speed3>=0){
		GPIO_WriteBit(GPIOB,GPIO_Pin_8,Bit_RESET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_SET);
		TIM_SetCompare3(TIM2,Speed3);
		
	}
	else{
		GPIO_WriteBit(GPIOB,GPIO_Pin_8,Bit_SET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_9,Bit_RESET);
		TIM_SetCompare3(TIM2,-Speed3);
	}

	/*-----------------------m3--------------------------*/
		if(Speed4>=0){
		GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_RESET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_11,Bit_SET);
		TIM_SetCompare4(TIM2,Speed4);
		
	}
	else{
		GPIO_WriteBit(GPIOB,GPIO_Pin_10,Bit_SET);
		GPIO_WriteBit(GPIOB,GPIO_Pin_11,Bit_RESET);
		TIM_SetCompare4(TIM2,-Speed4);
	}

	
}



