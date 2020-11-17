#include "../HARDWARE/Dma/bsp_dma.h"



DmaInformation Dma1Chanel4Information;

void DMA_USART1_RX_Init(UsartInformation *Uart1_Rx)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    DMA_DeInit(DMA1_Channel5);  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart1_Rx->UsartRxTxData;  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
    DMA_InitStructure.DMA_BufferSize = 32;  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);  	
    DMA_Cmd(DMA1_Channel5,ENABLE);  
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;     
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          
	NVIC_Init(&NVIC_InitStructure);
}

void DMA_USART1_TX_Init(UsartInformation *Uart1_Tx)
{
	NVIC_InitTypeDef NVIC_InitStructure;   
	RCC->AHBENR|=1<<0;			
	delay_ms(5);				
	DMA1_Channel4->CPAR=(u32)&USART1->DR; 	 	 
	DMA1_Channel4->CMAR=(u32)Uart1_Tx->UsartRxTxData; 	
	//Dma1Chanel4Information.DmaTransNum=9;      	
	DMA1_Channel4->CNDTR=0;    	
	DMA1_Channel4->CCR=0X00000000;	
	DMA1_Channel4->CCR|=1<<4;  		
	DMA1_Channel4->CCR|=0<<5;  					
	DMA1_Channel4->CCR|=0<<6; 		
	DMA1_Channel4->CCR|=1<<7; 	 	
	DMA1_Channel4->CCR|=0<<8; 	 	
	DMA1_Channel4->CCR|=0<<10; 		
	DMA1_Channel4->CCR|=1<<12; 		
	DMA1_Channel4->CCR|=0<<14; 			
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);  	
	DMA_Cmd(DMA1_Channel4 , ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;     
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          
	NVIC_Init(&NVIC_InitStructure);
}


/*
 * @brief 开启一次DMA传输
 * @param DMA通道
 * @param 传输数据量
 */
void DMA_TX_Enable(DMA_Channel_TypeDef *DMA_Channel,u16 ndtr)
{
 
	DMA_Cmd(DMA_Channel, DISABLE);                      //先关闭DMA,才能设置它
	
	DMA_Channel->CNDTR	=	ndtr;         //设置传输数据长度 
 
	DMA_Cmd(DMA_Channel, ENABLE);                      //开启DMA
}	  

void DMA_RX_Enable(DMA_Channel_TypeDef *DMA_Channel,u16 ndtr)
{
 
	DMA_Cmd(DMA_Channel, DISABLE);                      //先关闭DMA,才能设置它
	
	DMA_SetCurrDataCounter(DMA_Channel,ndtr);          //设置传输数据长度 
 
	DMA_Cmd(DMA_Channel, ENABLE);                      //开启DMA
}	


void DMA1_Channel4_IRQHandler(void)		
{
    if(DMA_GetITStatus(DMA1_IT_TC4)){
        DMA_ClearITPendingBit(DMA1_IT_GL4); 

    }
}

void DMA1_Channel5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC5)==SET){
         DMA_ClearFlag(DMA1_FLAG_TC5); 
    }	
}



