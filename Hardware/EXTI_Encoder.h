#ifndef __EXTI_ENCODER_H
#define __EXTI_ENCODER_H


extern int16_t velocity_x;
extern int16_t velocity_y;

void EXTI_Encoder_Init(void);
int32_t EXTI_Get_Distance_X(void);
int32_t EXTI_Get_Distance_Y(void);
extern int16_t Encoder_Count_M0;
extern int16_t Encoder_Count_M1;
extern int16_t Encoder_Count_M2;
extern int16_t Encoder_Count_M3;
extern int16_t temp0,temp1,temp2,temp3;
#endif
