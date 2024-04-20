#include "stm32f10x.h"  
#include "Control.h"
#include "OLED.h"
#include "stdlib.h"
#include "ADC.h"
#include "math.h"
#include "Serial.h"


//按键修改pid参数
uint8_t Refresh=1;
uint8_t Menu_Index = 4;
float Amplitude1=1,Amplitude2=1,Amplitude3=1,Amplitude4=-1,Amplitude_FB_PWM=-5;
uint16_t temp_num=0;
void Display_Menu(void){
	if(Menu_Index<=4){//清屏操作
		if(Refresh==1){
			OLED_Clear();
			Refresh=0;
		}
		
		OLED_ShowString(1,2,"B-KP:");
		OLED_ShowString(2,2,"B-KD:");
		OLED_ShowString(3,2,"P-KP:");
		OLED_ShowString(4,2,"P-KD:");
		OLED_ShowString(1,12,"ANG:");
	

		//OLED_ShowNum(2,12,cx,4);
		//OLED_ShowSignedNum(4,12,Spin_PWM,4);

	
		
		OLED_ShowNum(1,7,Balance_KP,3);
		OLED_ShowNum(2,7,Balance_KD,3);
		OLED_ShowDecimal(3,7,Position_KP,2,1);

		OLED_ShowNum(4,7,Position_KD,3);
		OLED_ShowChar((Menu_Index+2)%4+1,1,' ');
		OLED_ShowChar(Menu_Index,1,'*');
		
		OLED_ShowNum(2,12,ADC_Angle,4);
		OLED_ShowString(3,12,"BAT:");
		OLED_ShowChar(4,15,'V');
		OLED_ShowNum(4,12,round(Get_Battery_Voltage()),2);


	}
	
	else{
		if(Refresh==1){
			OLED_Clear();
			Refresh=0;
		}
		
		OLED_ShowString(1,1,"Dist:");
		OLED_ShowString(2,1,"PWM:");
		OLED_ShowString(3,1,"RX_CNT:");
		OLED_ShowString(4,1,"*FB_PWM:");
		
		OLED_ShowSignedNum(1,10,Encoder,5);
		OLED_ShowSignedNum(2,10,Motor_LR_PWM,5);
		OLED_ShowNum(3,10,Receive_Count,6);
		OLED_ShowSignedNum(4,10,Temp_FB_PWM,3);


			
	}
	
	
}
