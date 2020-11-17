#include "../HARDWARE/Usart/usart.h"
#include "../HARDWARE/Dma/bsp_dma.h"

/*
 *	���ڿ����ж� + DMA����  DMA����
 *
 *
 *
 */
 
 
UsartInformation  Usart1Descrip = {{0},20,0};
UsartInformation  *pUsart1Inforemation = &Usart1Descrip;


/*
 *	����1�жϷ�����
 *
 *
 */
void USART1_IRQHandler(void)                                 
{    
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  /* �ж��Ƿ�Ϊ���߿����ж� */
    {  
        pUsart1Inforemation->UsartRxCount = USART1->SR;  
        pUsart1Inforemation->UsartRxCount = USART1->DR; //����������IDLE��־λ
        DMA_Cmd(DMA1_Channel5,DISABLE);  
		
		DMA_ClearITPendingBit(DMA1_IT_TC5);	// Clear Transfer Complete flag
		DMA_ClearITPendingBit(DMA1_IT_TE5);	// Clear Transfer error flag	
		
		pUsart1Inforemation->UsartRxCount = 32 - DMA_GetCurrDataCounter(DMA1_Channel5);//����������ݳ���
		
        DMA_Cmd(DMA1_Channel5,ENABLE); 
		
		xSemaphoreGive( ComNoticeBinarySem_Handle );//������ֵ�ź���
    }   
	DMA_RX_Enable(DMA1_Channel5,32); /* DMAҪ�������ݸ��� 32 ���޸�*/
	
    __nop();  	
}

/**
  * @brief 485��ʼ������
  * @param pclk1 ʱ��Ƶ��
  * @param bound ������
  * @param dalen �ֳ�
  * @param stopbit ֹͣλ
  * @param Parity У�鷽ʽ
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


	
	//USART1_RX	  GPIOA.10��ʼ��
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
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	USART_Init(USART1, &USART_InitStructure); 		
	
    USART_ITConfig(USART1,USART_IT_TC,DISABLE);  
    USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); //��⵽������·
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	USART_Cmd(USART1, ENABLE); 

	RS485_1_TX_EN=RECEIVE;			//Ĭ��Ϊ����ģʽ	
	
}





