#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sys.h"
#include "delay.h"
#include "./runParame/runparame.h"
#include "../HARDWARE/TM1621/TM1621.h"
#include "../HARDWARE/Flash/rwFlash.h"
#include "../HARDWARE/CS1237/cs1237.h"
#include "../HARDWARE/Usart/usart.h"
#include "./app/app.h"
#include "../HARDWARE/Dma/bsp_dma.h"
#include "queue.h"
#include "semphr.h"

/* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* ���л��������� */
static TaskHandle_t RunPage_Task_Handle = NULL;
/* ���ݴ��������� */
static TaskHandle_t DealData_Task_Handle = NULL;
/* �������������� */
static TaskHandle_t KeyDeal_Task_Handle = NULL;
/* ͨѶ���������� */
static TaskHandle_t ComDeal_Task_Handle = NULL;
/* CPU������ͳ�������� */
//static TaskHandle_t CPU_Task_Handle = NULL;

/* �������������� */
void AppTaskCreate(void);

/* ����ͨѶ���ƶ�ֵ�ź��� */
SemaphoreHandle_t ComNoticeBinarySem_Handle =NULL;

/* �����������ƶ�ֵ�ź��� */
SemaphoreHandle_t KeyNoticeBinarySem_Handle =NULL;
RCC_ClocksTypeDef get_rcc_clock; 
/* Ӳ����ʼ������ */
void BSP_Init(void)
{
	/* ���մ��������0Ϊ����1λ���� */
	unsigned char readErrorNum=0; 
	
	const u32 BTL[]={2400,4800,9600,19200,38400,57600,115200};//������

	const u8 COMData[2][3]={	USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,	//8N1
								USART_WordLength_8b,(u8)USART_StopBits_2,USART_Parity_No//8N2
							};
	
							
	/* ʹ���ڲ�����ʹ���ڲ�����-36M */	
	Stm32_Clock_Init(9);
	
	/* ��ʱ������ʼ�� */
	delay_init(36);	  

	/* CS1237��ʼ��   */
	Ini_AD();	
	
	
	/* ��ʾ������ʼ�� */
	TM1621_init();
	WriteAllData(10,Tab0,7);	/* �ϵ����� */
	
	//DisplayNum(-5000,3);
	
	//Display1621(bmp,1);	/* ��ʾ���� */

	
	/* ��ȡ�洢����ֵ	ok */
	readErrorNum = readFromFlash(UserData,50); 
	
	if(readErrorNum == 0){
		/*	
		 *	�����쳣�����ڶ�ȡʱ�Ѿ����������ݻָ�Ϊ��ʼֵ
		 *	�˴�����Ӵ�����ʾ
		 */
	}
	
	/* �жϷ������ʹ��4 ȫ����Ϊ��ռ���ȼ� 0-15 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  
	/* USART1��DMA �����뷢�ͳ�ʼ��*/
	DMA_USART1_TX_Init(pUsart1Inforemation);
	DMA_USART1_RX_Init(pUsart1Inforemation);

	/* ����1��ʼ�� */
	/* 	ϵͳƵ�� 	������ 	�ֳ� 		ֹͣλ 			У�鷽ʽ*/ 
	USART1_Init(36,BTL[3],COMData[0][0],COMData[0][1],COMData[0][2]);
	
	/* ������ʼ��     */
	KeyInit();			
	

}


int main(void)
{	
	/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	BaseType_t xReturn = pdPASS;

	/* Ӳ����ʼ�� */
	BSP_Init();
	
	//	Read_AD();
	/*  ����AppTaskCreate���� */
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
						 (const char*     )"AppTaskCreate",/* �������� */
						 (uint16_t        )128,  /* ����ջ��С */
						 (void*           )NULL,/* ������ں������� */
						 (UBaseType_t     )1, /* ��������ȼ� */
						 (TaskHandle_t*   )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 	
	
	/* ����������� */           
	if(pdPASS == xReturn)
		vTaskStartScheduler();   /* �������񣬿������� */
	else
		return -1;  
	
	while(1);
}

/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
	/* ����ͨѶ��ֵ�ź��� */
  ComNoticeBinarySem_Handle = xSemaphoreCreateBinary();	 
//  if(NULL != ComBinarySem_Handle)
//    printf("BinarySem_Handle��ֵ�ź��������ɹ�!\r\n");	
	
  /* �������� */
  xReturn = xTaskCreate((TaskFunction_t )RunPage_Task, /* ������ں��� */
                        (const char*    )"RunPage_Task",/* �������� */
                        (uint16_t       )256,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )5,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&RunPage_Task_Handle);/* ������ƿ�ָ�� */
//  if(pdPASS == xReturn)
//    printf("�����ɹ�!\r\n");
  
	/* �������� */
  xReturn = xTaskCreate((TaskFunction_t )DealData_Task, /* ������ں��� */
                        (const char*    )"DealData_Task",/* �������� */
                        (uint16_t       )256,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )10,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&DealData_Task_Handle);/* ������ƿ�ָ�� */
//  if(pdPASS == xReturn)
//    printf("�����ɹ�!\r\n");
						
	/* �������� */
  xReturn = xTaskCreate((TaskFunction_t )KeyDeal_Task, /* ������ں��� */
                        (const char*    )"KeyDeal_Task",/* �������� */
                        (uint16_t       )256,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )4,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&KeyDeal_Task_Handle);/* ������ƿ�ָ�� */
//  if(pdPASS == xReturn)
//    printf("�����ɹ�!\r\n");

	/* �������� */
  xReturn = xTaskCreate((TaskFunction_t )ComDeal_Task, /* ������ں��� */
                        (const char*    )"ComDeal_Task",/* �������� */
                        (uint16_t       )256,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )9,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&ComDeal_Task_Handle);/* ������ƿ�ָ�� */
//  if(pdPASS == xReturn)
//    printf("�����ɹ�!\r\n");
						
						
						
//	xReturn = xTaskCreate((TaskFunction_t )CPU_Task, /* ������ں��� */
//						  (const char* )"CPU_Task",/* �������� */
//						  (uint16_t )512, /* ����ջ��С */
//						  (void* )NULL, /* ������ں�������*/
//						  (UBaseType_t )4, /* ��������ȼ� */
//						  (TaskHandle_t* )&CPU_Task_Handle);						
						
			
						
						
  xReturn = xReturn;
						
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}


