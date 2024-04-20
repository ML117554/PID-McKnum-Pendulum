#include "stm32f10x.h"  
#include "OLED.h"
#include "Timer.h"
#include "Serial.h"
#include "Delay.h"
#include "PWM.h"
#include "Control.h"
#include "math.h"
#include "EXTI_Encoder.h"
#include "ADC.h"
#include "Menu.h"
#include "EXTI_Key.h"

/*尽量不要在main函数中使用delay函数，因为中断中已经使用

********************麦克纳姆轮运动学原理****************************
1.受力分析:
单个麦轮模型，在轮子与地面接触的瞬间，辊子未发生转动时，辊子受到摩擦力向上，该摩擦力分解在
辊子的平行和垂直方向，垂直的分力使辊子发生转动而无效，对轮轴无力的作用，而平行的分力由于辊
子平行方向上的固定，该力(静摩擦)有效作用于轮轴，综上地面对单麦轮的作用力为平行于辊子方向，
驱使轮轴向该力的方向移动。

2.运动分析(四麦轮小车模型)
	2-1.单麦轮(轮轴)对地速度=小车结构中心的对地速度+该麦轮相对于中心的速度(自转) 
	2-2.该麦轮(轮轴)对地速度分解至平行于辊子和垂直于辊子方向的速度Vv,Vp,即该麦轮(轮轴)
	在此两方向上的对地移速
	2-3.分析除该麦轮外的麦轮于地无摩擦的情况(作用在该滚轮上平行于该辊子的静摩擦力穿过质心，即无自转)
	麦轮的转速Vr(常规轮在地面转动时，轮轴对地向前的速度，接触点静摩擦无滑动，故地对轴的速度为轮胎接触点
	对轴的速度)，该转速即原本常规轮向前的运动速度，当麦轮在地面运动时，该向前的速度分解为平行和垂直于
	辊子的两个分速度，分别表现为辊子的空转和麦轮沿平行于辊子方向上的移动速度V(静摩擦),若辊子倾角45度
	有:V=Vr/sqrt(2)
	2-4.麦轮小车在地面正常移动时，该麦轮在平行于辊子方向仍为静摩擦，表现为该方向上无打滑。故单位时间内
	该麦轮空转转动距离投影在该方向上表现为该麦轮在该方向上的移动距离，即(45度)Vr/srqt(2)=Vp

公式:(Vr轮子转速,Vx,Vy为小车对地速度的xy轴投影,w小车自转角速度,Rxy为小车中心距车轮转动中心的xy向距离)
左前:Vr=Vy+Vx-w(Rx+Ry)
左后:Vr=Vy-Vx-w(Rx+Ry)
右前:Vr=Vy-Vx+w(Rx+Ry)
右后:Vr=Vy+Vx+w(Rx+Ry)





TIPS：
	1.在小车模型运动中，单麦轮在垂直于辊子方向上的对地速度是其他轮的静摩擦力通过车架结构影响的结果
	



1.公式
左前:Vr=Vy+Vx-w(Rx+Ry)
左后:Vr=Vy-Vx-w(Rx+Ry)
右前:Vr=Vy-Vx+w(Rx+Ry)
右后:Vr=Vy+Vx+w(Rx+Ry)


2.PWM输出
CH1-4:PA15,PB3,PA2,PA3

3.方向控制
左前m0: PA4,PA5
左后m1: PA11,PA12
右前m2: PB8,PB9
右后m3: PB10,PB11

4.uart1:
TX,RX：PA9，PA10



5.EXTI_Encoder:(C8T6外部中断模拟编码器计数)
m0   PB4(EXTI)       PB5
m1   PB6             PB7(EXTI) 
m2   PA0(EXTI)       PA1
m3   PB0             PB1(EXTI) 

6.ADC_Angle:PA6  ADC_Battery_Voltage:PA7

7.EXTI_Key:PB12 PB13

8.OLED: SCL-SDA:PB14-PB15

*/

int main(void){
	uint16_t Off_Count=0;
	Timer_Init();
	
	Adc_Init();
	OLED_Init();
	PWM_Motor_Init();
	Motor_DirectPin_Init();
	EXTI_Encoder_Init();
	EXTI_Key_Init();
	OLED_Clear();
	Serial_Init();
	while(1){
		Display_Menu();
		//OLED_ShowNum(1,1,Off_Count++,10);
		/*
		if(Shut_Down_Flag==1){
			Control_Motor(0,0,0);
			
			//if(++Off_Count>20)
			//break;
		}*/
		
	

}
	
	
}

