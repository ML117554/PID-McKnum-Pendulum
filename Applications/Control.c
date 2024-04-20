#include "stm32f10x.h"
#include "control.h"		
#include "ADC.h"
#include "EXTI_Encoder.h"
#include "PWM.h"
#include "stdlib.h"
#include "Delay.h"
#include "Serial.h"

/*
大致调参过程:
1.极性:先判断各参数极性，在该过程中，也可体现各参数的作用含义
2.大小:B-KP:在其他为0的情况下，增大至大幅低频震荡，得到最大的KP
			 B-KD:在B-KP的前提下，增大KD以减小震荡幅度，直至小幅低频振荡,得到最大的KD
			 -----(B-KP,B-KD)*0.6得到适宜的角度环参数,在此角度环参数下，可保持直立，但没有小车的速度和距离限制------------
				
			 P-KP:调至小车在某一范围来回移动，增大P-KP可以限制距离，但是过大很容易造成倾角的过大，导致发散
			 因此主要靠P-KD限制移动距离
			 
			 
			 
			 P-KD:限制小车的移动范围，直至较小,但是不可过大，会导致抖动
				
				注意:P-KP,P-KD的过大都容易造成小车减速至0时的倾角过大，造成系统不稳定
				正常情况:小车平稳无抖动，在小范围内优雅移动，若出现较大抖动，适当减小P-KD，减小小车移动的限制，以增加平稳


较平稳，但抗干扰弱:
float Balance_KP=18,Balance_KD=36,Position_KP=1,Position_KD=20;

几乎完美，但鲁棒性有些差:
float Balance_KP=18,Balance_KD=36,Position_KP=1,Position_KD=30;


*/

//Kp=30*0.6 Kd=60*0.6  
int32_t Position_Zero=0; 
//float Balance_KP=0,Balance_KD=0,Position_KP=0,Position_KD=0;
float Balance_KP=18,Balance_KD=36,Position_KP=1,Position_KD=25;

int16_t Balance_PWM,Distance_PWM,Motor_LR_PWM,Motor_FB_PWM; //目标角度PWM、目标位置PWM
uint8_t Angle_Ctrl_Count=0;//用于标记位置控制的时间
uint16_t ADC_Angle;
//倾角PD控制所用到的参数
float Angle_Error;                       //倾角偏差
float Angle_PWM,Last_Angle_Error,D_Angle_Eorror;   


float Position_PWM,Last_Position_Error,Position_Error,Position_Differential;//
float Position_Error;
int32_t Encoder;
uint8_t Shut_Down_Count=0;
uint8_t Shut_Down_Flag=0;

float sum,ratio;
//旋转速度
int16_t Spin_PWM=0,Spin_Error=0,Spin_Last_Error=0;
float Spin_KD=0.6;
int16_t Temp_FB_PWM=-20;


void TIM1_UP_IRQHandler(void)  
{    
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 //5ms定时中断
	{   
		  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);             //===清除定时器1中断标志位	                     
			
			Angle_Ctrl_Count++;
			Encoder=EXTI_Get_Distance_X();  //获得移动距离

      ADC_Angle=Get_Angle();                    //===更新ADC角度
		
			if(ADC_Angle>2300||ADC_Angle<1700||Encoder>10000||Encoder<-10000){
				Shut_Down_Count++;
				if(Shut_Down_Count>10)
					Shut_Down_Flag=1;
			}
			else
				Shut_Down_Count=0;
			
			Balance_PWM =Balance(ADC_Angle);        //===角度PD控制	
			if(Angle_Ctrl_Count==5){			//25ms进行一次位置环控制，减少对角度环的干扰
				Distance_PWM=Position(Encoder);
				Angle_Ctrl_Count=0;
			
			//计算Spin_PWM
			if(Detected_Flag==1){
				
				Spin_PWM=Spin_PWM_Calculate(cx);
				Motor_FB_PWM=Temp_FB_PWM;
			}
			else{
				Spin_PWM=0;
				Motor_FB_PWM=0;
			}
			
			}
			


			Motor_LR_PWM=Balance_PWM+Distance_PWM; 			//===计算电机最终PWM
			if(Shut_Down_Flag==0)
				Control_Motor(Motor_LR_PWM,Motor_FB_PWM,Spin_PWM); 		//Motor_FB_PWM_MAX=200;Spin_PWM_MAX=100;
			else
				Control_Motor(0,0,0);
				

} 
	}

int16_t Balance(uint16_t Angle)
{  
	 Angle_Error=ZHONGZHI-Angle;              //求出平衡的角度中值 和机械相关
	 D_Angle_Eorror=Angle_Error-Last_Angle_Error;            //求出偏差的微分 进行微分控制
	 Angle_PWM=Balance_KP*Angle_Error+D_Angle_Eorror*Balance_KD;   //===计算倾角控制的电机PWM  PD控制
   Last_Angle_Error=Angle_Error;                   //保持上一次的偏差
	 return Angle_PWM;
}

int16_t Position(int32_t Encoder)
{  
  	Position_Error=Encoder-Position_Zero;             //===
    Position_Error=Position_Error*0.2+Last_Position_Error*0.8;        //===一阶低通滤波器  
	  Position_Differential=Position_Error-Last_Position_Error;
	  Last_Position_Error=Position_Error;
		Position_PWM=Position_Error*Position_KP+Position_Differential*Position_KD; //===速度控制		
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

