#ifndef __ADC_H
#define __ADC_H
extern uint16_t ADC_Value[3];

void Adc_Init(void);
float Get_Battery_Voltage(void);
uint16_t Get_Angle(void);
#endif
