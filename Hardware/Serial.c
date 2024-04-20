#include "stm32f10x.h"
#include "Serial.h"
#include "OLED.h"
#include "stdlib.h"
#include "string.h"
//PA9=TX   PA10=RX
uint16_t cx=0,cy=0;
uint8_t Receive_Flag = 0;
uint8_t Detected_Flag=0;
uint16_t time_period;

uint16_t Receive_Count=0;
uint8_t CX_String[20]={0},CY_String[20]={0};
uint8_t CX_Index=0,CY_Index=0;
uint8_t Right_Data=0;
uint8_t Judge_Data=0;
void Serial_Init(void){
	//RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//USART
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//������
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;       //һλֹͣλ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ�ֳ�������У��λ
	USART_Init(USART1,&USART_InitStructure);
	
	//IT USART->CR1_RXNEIE ���ջ������ǿ��ж�ʹ��
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//USART->SR_RXNEһ����һ����NVIC�����ж�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
}

void Serial_SendByte(uint8_t Byte){
	USART_SendData(USART1,Byte);
	/*USART->SR_TXE���ͼĴ����ձ�־λ��
	�ȴ����ͼĴ���ȫ��ת�Ƶ���λ�Ĵ�����
	��USART_DR��д����������λ���㣬�����ֶ�����
	*/
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
}

void Serial_SendString(uint8_t *Str){
	
	uint16_t i;
	for(i=0;Str[i]!='\0';i++){
		USART_SendData(USART1,*(Str+i));
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	}

	
}




int fputc(int ch,FILE *f){
	Serial_SendByte(ch);
	return ch;
}


void Detect_Data(void){
//����У���У����У��
	if(strlen(CX_String)==3&&strlen(CY_String)==3){
		if(Judge_Data==(CX_String[--CX_Index]+CY_String[--CY_Index]-'0'-'0')%10+'0'){
		//Receive_Flag=1;
		
		cx=atoi(CX_String);
		cy=atoi(CY_String);
			//printf("%x#%x#%p",(uint32_t)&cx,(uint32_t)&cy,cx);
		if(cx==0&&cy==0){
			Detected_Flag=0;
			
		}
		else{
			Detected_Flag=1;
			
		}
		
		
	}
}
	
	
}
//�����ж����ȼ���ͣ����ǿ��ܽ��յ�ƴ�ӵ��쳣����
void USART1_IRQHandler(void){
	/*1.�������жϺ�ʱ���ã�������RXNE=1ʱRDR�������ݣ���λ�Ĵ��������ݴ�����RDR�Ĵ���ʱ��
	Ӳ��ORE��һ(Overrun error) ,����RXNEIE=1ʱҲ�ᴥ���жϣ���������������жϺ���һֱִ�У�
	�޷�����������
	2.Receive_Flag����Ҫ����Ч��ֹƴ�ӵĴ������ݻ�ǿ���ж�ȡ�ߵĴ������ݱ�����*/
	static uint8_t RxState = 0;
	//printf("%d",Receive_Count++);

//ORE�������жϣ������޷��˳��ж�
	
  if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) // ��� ORE ��־
  {
      USART_ClearFlag(USART1,USART_FLAG_ORE);
      USART_ReceiveData(USART1);
  }

//RXNE�������ж�
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET){

		uint8_t RxData = USART_ReceiveData(USART1);
	  //uint8_t	RxData=0;
		//printf(&RxData);
		Receive_Count++;
		//printf("%d",RxState);
		if(RxState==0){ //���հ�ͷֻ�е�������ϲŽ���д
			if(RxData == '@'){
				CX_Index=0;
				CY_Index=0;
				
				RxState=1;

			}
		}
		else if(RxState==1){ //����4λ����
			if(RxData=='*'){
				RxState=2;
			}
			else{
			CX_String[CX_Index++]=RxData;
			}
		}
		
		else if(RxState==2){//���հ�β
			if(RxData=='$'){//У������������ݳ���
				RxState=3;
		}
			else{
				CY_String[CY_Index++]=RxData;
			}
	}
		
	else if(RxState==3){
		if(RxData=='#'){
			RxState=0;

			CX_String[CX_Index]='\0';
			CY_String[CY_Index]='\0';
			Detect_Data();
		}
		else{
			Judge_Data=RxData;
		}
	}
		/*
		Serial_SendByte(RxData);
		printf("\n%d\n",Receive_Count++);
	*/

    //USART->SR_RXNEд0��գ�(���DR����)
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}
