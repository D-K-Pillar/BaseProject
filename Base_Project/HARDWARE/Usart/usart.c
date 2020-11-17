#include "../HARDWARE/Usart/usart.h"
#include "../HARDWARE/Dma/bsp_dma.h"

/*
 *	串口空闲中断 + DMA接收  DMA发送
 *
 *
 *
 */
 
 
UsartInformation  Usart1Descrip = {{0},20,0};
UsartInformation  *pUsart1Inforemation = &Usart1Descrip;


/*
 *	串口1中断服务函数
 *
 *
 */
void USART1_IRQHandler(void)                                 
{    
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  /* 判断是否为总线空闲中断 */
    {  
        pUsart1Inforemation->UsartRxCount = USART1->SR;  
        pUsart1Inforemation->UsartRxCount = USART1->DR; //软件序列清除IDLE标志位
        DMA_Cmd(DMA1_Channel5,DISABLE);  
		
		DMA_ClearITPendingBit(DMA1_IT_TC5);	// Clear Transfer Complete flag
		DMA_ClearITPendingBit(DMA1_IT_TE5);	// Clear Transfer error flag	
		
		pUsart1Inforemation->UsartRxCount = 32 - DMA_GetCurrDataCounter(DMA1_Channel5);//计算接收数据长度
		
        DMA_Cmd(DMA1_Channel5,ENABLE); 
		
		xSemaphoreGive( ComNoticeBinarySem_Handle );//给出二值信号量
    }   
	DMA_RX_Enable(DMA1_Channel5,32); /* DMA要接收数据个数 32 可修改*/
	
    __nop();  	
}

/**
  * @brief 485初始化函数
  * @param pclk1 时钟频率
  * @param bound 波特率
  * @param dalen 字长
  * @param stopbit 停止位
  * @param Parity 校验方式
  */
void USART1_Init(u32 pclk1,u32 bound,uint16_t dalen,uint16_t stopbit,uint16_t Parity)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	
	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	

	USART_InitStructure.USART_BaudRate = bound;		
	USART_InitStructure.USART_WordLength = dalen;	
	USART_InitStructure.USART_StopBits = stopbit;	
	USART_InitStructure.USART_Parity = Parity;		
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	USART_Init(USART1, &USART_InitStructure); 		
	
    USART_ITConfig(USART1,USART_IT_TC,DISABLE);  
    USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); //检测到空闲线路
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	USART_Cmd(USART1, ENABLE); 

	RS485_1_TX_EN=RECEIVE;			//默认为接收模式	
	
}





