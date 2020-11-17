#ifndef _BSP_DMA_H_
#define _BSP_DMA_H_

#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "../HARDWARE/Usart/usart.h"

typedef struct DmaDtaDescrip{
	unsigned char DmaTransNum;
	


}DmaInformation;

void DMA_USART1_RX_Init(UsartInformation *Uart1_Rx);
void DMA_USART1_TX_Init(UsartInformation *Uart1_Tx);
void DMA_RX_Enable(DMA_Channel_TypeDef *DMA_Channel,u16 ndtr);
void DMA_TX_Enable(DMA_Channel_TypeDef *DMA_Channel,u16 ndtr);



#endif
