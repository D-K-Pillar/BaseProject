#ifndef _APP_H_
#define _APP_H_ 	

#include "sys.h"  
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "queue.h"
#include "semphr.h"
#include "../HARDWARE/Key/key.h"
#include "../HARDWARE/Usart/usart.h"
#include "../HARDWARE/Dma/bsp_dma.h"
#include "../HARDWARE/CS1237/cs1237.h"
#include "../HARDWARE/TM1621/TM1621.h"
#include "./RunPage/runPage.h"
#include "./myComProcess/myComprocess.h"

void RunPage_Task(void* parameter);
void DealData_Task(void* parameter);
void KeyDeal_Task(void* parameter);
void ComDeal_Task(void* parameter);

//void CPU_Task(void* parameter);
#endif


