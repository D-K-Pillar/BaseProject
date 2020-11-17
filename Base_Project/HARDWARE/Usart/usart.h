#ifndef _USART_H_
#define _USART_H_


#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"

typedef struct UsartDescrip{
	/* ���ڽ����뷢�����ݻ����� */
	unsigned char UsartRxTxData[50];
	
	/* ���ڽ������ݼ��� */
	unsigned char UsartRxCount;

	/* ���ڷ��ͼ���*/
	unsigned char UsartTxCount;
	
	
}UsartInformation;


#define RS485_1_TX_EN PAout(8)

#define RECEIVE  0;
#define SEND     1;

extern UsartInformation Usart1Descrip;
extern UsartInformation *pUsart1Inforemation;
extern SemaphoreHandle_t ComNoticeBinarySem_Handle;


/* 					ϵͳƵ�� 	������ 			�ֳ� 			ֹͣλ 		У�鷽ʽ*/ 
void USART1_Init(u32 pclk1,u32 bound,uint16_t dalen,uint16_t stopbit,uint16_t Parity);



#endif
