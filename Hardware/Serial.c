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
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;       //一位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位字长，包含校验位
	USART_Init(USART1,&USART_InitStructure);
	
	//IT USART->CR1_RXNEIE 接收缓冲区非空中断使能
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//USART->SR_RXNE一旦置一，向NVIC申请中断
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
	/*USART->SR_TXE发送寄存器空标志位，
	等待发送寄存器全部转移到移位寄存器，
	对USART_DR的写操作，将该位清零，无需手动清零
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
//长度校验和校验码校验
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
//串口中断优先级最低，考虑可能接收到拼接的异常数据
void USART1_IRQHandler(void){
	/*1.若串口中断耗时过久，导致在RXNE=1时RDR已有数据，移位寄存器将数据传送至RDR寄存器时，
	硬件ORE置一(Overrun error) ,并当RXNEIE=1时也会触发中断，若不清除，串口中断函数一直执行，
	无法返回主函数
	2.Receive_Flag很重要，有效防止拼接的错误数据或被强行中断取走的错误数据被调用*/
	static uint8_t RxState = 0;
	//printf("%d",Receive_Count++);

//ORE触发的中断，否则无法退出中断
	
  if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET) // 检查 ORE 标志
  {
      USART_ClearFlag(USART1,USART_FLAG_ORE);
      USART_ReceiveData(USART1);
  }

//RXNE触发的中断
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET){

		uint8_t RxData = USART_ReceiveData(USART1);
	  //uint8_t	RxData=0;
		//printf(&RxData);
		Receive_Count++;
		//printf("%d",RxState);
		if(RxState==0){ //接收包头只有当读出完毕才进行写
			if(RxData == '@'){
				CX_Index=0;
				CY_Index=0;
				
				RxState=1;

			}
		}
		else if(RxState==1){ //接收4位数据
			if(RxData=='*'){
				RxState=2;
			}
			else{
			CX_String[CX_Index++]=RxData;
			}
		}
		
		else if(RxState==2){//接收包尾
			if(RxData=='$'){//校验结束符和数据长度
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

    //USART->SR_RXNE写0清空，(或读DR清零)
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}
