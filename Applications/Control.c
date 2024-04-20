#include "stm32f10x.h"
#include "control.h"		
#include "ADC.h"
#include "EXTI_Encoder.h"
#include "PWM.h"
#include "stdlib.h"
#include "Delay.h"
#include "Serial.h"

/*
���µ��ι���:
1.����:���жϸ��������ԣ��ڸù����У�Ҳ�����ָ����������ú���
2.��С:B-KP:������Ϊ0������£������������Ƶ�𵴣��õ�����KP
			 B-KD:��B-KP��ǰ���£�����KD�Լ�С�𵴷��ȣ�ֱ��С����Ƶ��,�õ�����KD
			 -----(B-KP,B-KD)*0.6�õ����˵ĽǶȻ�����,�ڴ˽ǶȻ������£��ɱ���ֱ������û��С�����ٶȺ;�������------------
				
			 P-KP:����С����ĳһ��Χ�����ƶ�������P-KP�������ƾ��룬���ǹ�������������ǵĹ��󣬵��·�ɢ
			 �����Ҫ��P-KD�����ƶ�����
			 
			 
			 
			 P-KD:����С�����ƶ���Χ��ֱ����С,���ǲ��ɹ��󣬻ᵼ�¶���
				
				ע��:P-KP,P-KD�Ĺ����������С��������0ʱ����ǹ������ϵͳ���ȶ�
				�������:С��ƽ���޶�������С��Χ�������ƶ��������ֽϴ󶶶����ʵ���СP-KD����СС���ƶ������ƣ�������ƽ��


��ƽ�ȣ�����������:
float Balance_KP=18,Balance_KD=36,Position_KP=1,Position_KD=20;

������������³������Щ��:
float Balance_KP=18,Balance_KD=36,Position_KP=1,Position_KD=30;


*/

//Kp=30*0.6 Kd=60*0.6  
int32_t Position_Zero=0; 
//float Balance_KP=0,Balance_KD=0,Position_KP=0,Position_KD=0;
float Balance_KP=18,Balance_KD=36,Position_KP=1,Position_KD=25;

int16_t Balance_PWM,Distance_PWM,Motor_LR_PWM,Motor_FB_PWM; //Ŀ��Ƕ�PWM��Ŀ��λ��PWM
uint8_t Angle_Ctrl_Count=0;//���ڱ��λ�ÿ��Ƶ�ʱ��
uint16_t ADC_Angle;
//���PD�������õ��Ĳ���
float Angle_Error;                       //���ƫ��
float Angle_PWM,Last_Angle_Error,D_Angle_Eorror;   


float Position_PWM,Last_Position_Error,Position_Error,Position_Differential;//
float Position_Error;
int32_t Encoder;
uint8_t Shut_Down_Count=0;
uint8_t Shut_Down_Flag=0;

float sum,ratio;
//��ת�ٶ�
int16_t Spin_PWM=0,Spin_Error=0,Spin_Last_Error=0;
float Spin_KD=0.6;
int16_t Temp_FB_PWM=-20;


void TIM1_UP_IRQHandler(void)  
{    
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ //5ms��ʱ�ж�
	{   
		  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);             //===�����ʱ��1�жϱ�־λ	                     
			
			Angle_Ctrl_Count++;
			Encoder=EXTI_Get_Distance_X();  //����ƶ�����

      ADC_Angle=Get_Angle();                    //===����ADC�Ƕ�
		
			if(ADC_Angle>2300||ADC_Angle<1700||Encoder>10000||Encoder<-10000){
				Shut_Down_Count++;
				if(Shut_Down_Count>10)
					Shut_Down_Flag=1;
			}
			else
				Shut_Down_Count=0;
			
			Balance_PWM =Balance(ADC_Angle);        //===�Ƕ�PD����	
			if(Angle_Ctrl_Count==5){			//25ms����һ��λ�û����ƣ����ٶԽǶȻ��ĸ���
				Distance_PWM=Position(Encoder);
				Angle_Ctrl_Count=0;
			
			//����Spin_PWM
			if(Detected_Flag==1){
				
				Spin_PWM=Spin_PWM_Calculate(cx);
				Motor_FB_PWM=Temp_FB_PWM;
			}
			else{
				Spin_PWM=0;
				Motor_FB_PWM=0;
			}
			
			}
			


			Motor_LR_PWM=Balance_PWM+Distance_PWM; 			//===����������PWM
			if(Shut_Down_Flag==0)
				Control_Motor(Motor_LR_PWM,Motor_FB_PWM,Spin_PWM); 		//Motor_FB_PWM_MAX=200;Spin_PWM_MAX=100;
			else
				Control_Motor(0,0,0);
				

} 
	}

int16_t Balance(uint16_t Angle)
{  
	 Angle_Error=ZHONGZHI-Angle;              //���ƽ��ĽǶ���ֵ �ͻ�е���
	 D_Angle_Eorror=Angle_Error-Last_Angle_Error;            //���ƫ���΢�� ����΢�ֿ���
	 Angle_PWM=Balance_KP*Angle_Error+D_Angle_Eorror*Balance_KD;   //===������ǿ��Ƶĵ��PWM  PD����
   Last_Angle_Error=Angle_Error;                   //������һ�ε�ƫ��
	 return Angle_PWM;
}

int16_t Position(int32_t Encoder)
{  
  	Position_Error=Encoder-Position_Zero;             //===
    Position_Error=Position_Error*0.2+Last_Position_Error*0.8;        //===һ�׵�ͨ�˲���  
	  Position_Differential=Position_Error-Last_Position_Error;
	  Last_Position_Error=Position_Error;
		Position_PWM=Position_Error*Position_KP+Position_Differential*Position_KD; //===�ٶȿ���		
	  return Position_PWM;
}

int16_t Spin_PWM_Calculate(int16_t x){
	Spin_Error=SCREEN_WIDTH/2-x;
	//Spin_Error=0.8*Spin_Last_Error+0.2*Spin_Error;
	Spin_Last_Error=Spin_Error;
	return (int16_t)(Spin_KD*Spin_Error);

	
}



void Control_Motor(int16_t LR,int16_t FB,int16_t Spin){
	FB=(FB>200)?200:FB;
	FB=(FB<-200)?-200:FB;
	Spin=(Spin>100)?100:Spin;
	Spin=(Spin<-100)?-100:Spin;
	
	sum=abs(LR)+abs(FB)+abs(Spin);
	if(sum>Max_PWM){
		ratio=(float)Max_PWM/sum;
		LR=LR*ratio;
		FB=FB*ratio;
		Spin=Spin*ratio;
	}
	Motor_SetSpeed(FB+LR-Spin,FB-LR-Spin,FB-LR+Spin,FB+LR+Spin);
}

