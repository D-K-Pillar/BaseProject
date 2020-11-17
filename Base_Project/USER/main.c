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

/* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* 运行画面任务句柄 */
static TaskHandle_t RunPage_Task_Handle = NULL;
/* 数据处理任务句柄 */
static TaskHandle_t DealData_Task_Handle = NULL;
/* 按键处理任务句柄 */
static TaskHandle_t KeyDeal_Task_Handle = NULL;
/* 通讯处理任务句柄 */
static TaskHandle_t ComDeal_Task_Handle = NULL;
/* CPU利用率统计任务句柄 */
//static TaskHandle_t CPU_Task_Handle = NULL;

/* 创建任务函数声明 */
void AppTaskCreate(void);

/* 创建通讯控制二值信号量 */
SemaphoreHandle_t ComNoticeBinarySem_Handle =NULL;

/* 创建按键控制二值信号量 */
SemaphoreHandle_t KeyNoticeBinarySem_Handle =NULL;
RCC_ClocksTypeDef get_rcc_clock; 
/* 硬件初始化函数 */
void BSP_Init(void)
{
	/* 接收错误参数，0为错误，1位正常 */
	unsigned char readErrorNum=0; 
	
	const u32 BTL[]={2400,4800,9600,19200,38400,57600,115200};//波特率

	const u8 COMData[2][3]={	USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,	//8N1
								USART_WordLength_8b,(u8)USART_StopBits_2,USART_Parity_No//8N2
							};
	
							
	/* 使用内部晶振，使用内部晶振-36M */	
	Stm32_Clock_Init(9);
	
	/* 延时函数初始化 */
	delay_init(36);	  

	/* CS1237初始化   */
	Ini_AD();	
	
	
	/* 显示函数初始化 */
	TM1621_init();
	WriteAllData(10,Tab0,7);	/* 上电清屏 */
	
	//DisplayNum(-5000,3);
	
	//Display1621(bmp,1);	/* 显示测试 */

	
	/* 读取存储器数值	ok */
	readErrorNum = readFromFlash(UserData,50); 
	
	if(readErrorNum == 0){
		/*	
		 *	数据异常处理，在读取时已经将错误数据恢复为初始值
		 *	此处可添加错误显示
		 */
	}
	
	/* 中断分组最好使用4 全配置为抢占优先级 0-15 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  
	/* USART1的DMA 接收与发送初始化*/
	DMA_USART1_TX_Init(pUsart1Inforemation);
	DMA_USART1_RX_Init(pUsart1Inforemation);

	/* 串口1初始化 */
	/* 	系统频率 	波特率 	字长 		停止位 			校验方式*/ 
	USART1_Init(36,BTL[3],COMData[0][0],COMData[0][1],COMData[0][2]);
	
	/* 按键初始化     */
	KeyInit();			
	

}


int main(void)
{	
	/* 定义一个创建信息返回值，默认为pdPASS */
	BaseType_t xReturn = pdPASS;

	/* 硬件初始化 */
	BSP_Init();
	
	//	Read_AD();
	/*  创建AppTaskCreate任务 */
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
						 (const char*     )"AppTaskCreate",/* 任务名字 */
						 (uint16_t        )128,  /* 任务栈大小 */
						 (void*           )NULL,/* 任务入口函数参数 */
						 (UBaseType_t     )1, /* 任务的优先级 */
						 (TaskHandle_t*   )&AppTaskCreate_Handle);/* 任务控制块指针 */ 	
	
	/* 启动任务调度 */           
	if(pdPASS == xReturn)
		vTaskStartScheduler();   /* 启动任务，开启调度 */
	else
		return -1;  
	
	while(1);
}

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区
  
	/* 创建通讯二值信号量 */
  ComNoticeBinarySem_Handle = xSemaphoreCreateBinary();	 
//  if(NULL != ComBinarySem_Handle)
//    printf("BinarySem_Handle二值信号量创建成功!\r\n");	
	
  /* 创建任务 */
  xReturn = xTaskCreate((TaskFunction_t )RunPage_Task, /* 任务入口函数 */
                        (const char*    )"RunPage_Task",/* 任务名字 */
                        (uint16_t       )256,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )5,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&RunPage_Task_Handle);/* 任务控制块指针 */
//  if(pdPASS == xReturn)
//    printf("创建成功!\r\n");
  
	/* 创建任务 */
  xReturn = xTaskCreate((TaskFunction_t )DealData_Task, /* 任务入口函数 */
                        (const char*    )"DealData_Task",/* 任务名字 */
                        (uint16_t       )256,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )10,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&DealData_Task_Handle);/* 任务控制块指针 */
//  if(pdPASS == xReturn)
//    printf("创建成功!\r\n");
						
	/* 创建任务 */
  xReturn = xTaskCreate((TaskFunction_t )KeyDeal_Task, /* 任务入口函数 */
                        (const char*    )"KeyDeal_Task",/* 任务名字 */
                        (uint16_t       )256,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )4,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&KeyDeal_Task_Handle);/* 任务控制块指针 */
//  if(pdPASS == xReturn)
//    printf("创建成功!\r\n");

	/* 创建任务 */
  xReturn = xTaskCreate((TaskFunction_t )ComDeal_Task, /* 任务入口函数 */
                        (const char*    )"ComDeal_Task",/* 任务名字 */
                        (uint16_t       )256,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )9,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&ComDeal_Task_Handle);/* 任务控制块指针 */
//  if(pdPASS == xReturn)
//    printf("创建成功!\r\n");
						
						
						
//	xReturn = xTaskCreate((TaskFunction_t )CPU_Task, /* 任务入口函数 */
//						  (const char* )"CPU_Task",/* 任务名字 */
//						  (uint16_t )512, /* 任务栈大小 */
//						  (void* )NULL, /* 任务入口函数参数*/
//						  (UBaseType_t )4, /* 任务的优先级 */
//						  (TaskHandle_t* )&CPU_Task_Handle);						
						
			
						
						
  xReturn = xReturn;
						
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}


