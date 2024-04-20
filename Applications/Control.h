#ifndef __CONTROL_H
#define __CONTROL_H

#define ZHONGZHI 2045
#define Max_PWM 1000
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160
extern int16_t Motor_LR_PWM,Motor_FB_PWM;

extern uint16_t ADC_Angle;
extern int32_t Encoder;
extern float Position_PWM,Last_Position_Error,Position_Error,Position_Differential;
extern float Balance_KP,Balance_KD,Position_KP,Position_KD;

extern uint8_t Shut_Down_Flag;
extern int16_t Temp_FB_PWM;
int16_t Position(int32_t Encoder);
int16_t Balance(uint16_t Angle);
void Control_Motor(int16_t LR,int16_t FB,int16_t Spin);
int16_t Spin_PWM_Calculate(int16_t x);
extern int16_t Spin_PWM;
#endif
