#ifndef _USART_H_
#define _USART_H_


#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"

typedef struct UsartDescrip{
	/* 串口接收与发送数据缓冲区 */
	unsigned char UsartRxTxData[50];
	
	/* 串口接收数据计数 */
	unsigned char UsartRxCount;

	/* 串口发送计数*/
	unsigned char UsartTxCount;
	
	
}UsartInformation;


#define RS485_1_TX_EN PAout(8)

#define RECEIVE  0;
#define SEND     1;

extern UsartInformation Usart1Descrip;
extern UsartInformation *pUsart1Inforemation;
extern SemaphoreHandle_t ComNoticeBinarySem_Handle;


/* 					系统频率 	波特率 			字长 			停止位 		校验方式*/ 
void USART1_Init(u32 pclk1,u32 bound,uint16_t dalen,uint16_t stopbit,uint16_t Parity);



#endif
