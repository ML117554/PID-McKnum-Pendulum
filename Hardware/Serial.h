#ifndef __SERIAL_H
#define __SERIAL_H
#include "stdio.h"
#include "math.h"
#include "stdarg.h"
#include "OLED.h"
#include "string.h"

extern uint16_t cx,cy;
extern uint8_t Receive_Flag;
extern uint8_t Detected_Flag;

extern uint16_t Receive_Count;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array,uint16_t Length);
void Serial_SendString(uint8_t *Str);
void Serial_SendNumber(uint32_t Number,uint8_t Length);
void Serial_SendPacket(void);
void Serial_Printf(char* format,...);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

#endif 
