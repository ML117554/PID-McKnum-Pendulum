#ifndef __PWM_H
#define __PWM_H


void PWM_Motor_Init(void);
void Motor_DirectPin_Init(void);
void Motor_SetSpeed(int16_t Speed1,int16_t Speed2,int16_t Speed3,int16_t Speed4);

#endif
