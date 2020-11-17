#ifndef __DELAY_H
#define __DELAY_H 	

#include "sys.h"  
#include "../HARDWARE/Key/key.h"
#include "stm32f10x.h"



void delay_init(u8 SYSCLK);
void delay_ms(u8 Tms);
void delay_us(u8 Tus);
void Stm32_Clock_Init(u8 PLL);
u8 WT(u32 CTn,u32 LT,u32 CT0);



typedef struct timeStruct
{
	unsigned int  ibastTime;//基准时间   1ms加1
	unsigned char cwKeyCount; //按键处理时间  
	
}timeStruct;


extern timeStruct systemTime;
#endif





























