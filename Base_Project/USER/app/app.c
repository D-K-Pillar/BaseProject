#include "./app/app.h"


/*
 *	此文件用来存放运行任务函数
 *  任务1 - RunPage_Task  - 优先级 5  - 用于界面切换与显示
 *  任务2 - DealData_Task - 优先级 10 - 用于数据处理与存储
 *	任务3 - KeyDeal_Task  - 优先级 4  - 按键数据处理
 *	任务4 - ComDeal_Task  - 优先级 9  - 通讯处理任务	
 *
 *
 */








/*
 *	运行画面任务 优先级5  数字越大优先级越高
 *  configUSE_PORT_OPTIMISED_TASK_SELECTION为1，故最大优先级支持达到32个 0是最小优先级
 *
 */
void RunPage_Task(void* parameter)
{	
	unsigned int wWaitTime;
	unsigned char cwchar[6]={16,16,16,16,16,16};
    while (1)
    { 
		switch(RTData[40])
		{
			case 0:
				DisplayNum(RTData[0],2);	//显示实时力值
			
			
				if( HowToUseKey(pKey1Information  , NOKEY) ){	//长按K2清零
					if( HowToUseKey(pKey2Information  , KEY_LONG) ){
						UserData[6] += FADNew;
						writeToFlash(UserData,50);
					}
				}
				
				if( HowToUseKey(pKey2Information  , NOKEY) ){	//长按K1进入校准界面
					if( HowToUseKey(pKey1Information  , KEY_LONG) ){
						RTData[40] = 1; 						
						wWaitTime = systemTime.ibastTime; 
						cwchar[3] = 0x0C; cwchar[4] = 0x0A; cwchar[5] = 0x12;
						RTData[21] = RTData[0];
						Display1621(cwchar,5);
					}
					
				}
				
				if( HowToUseKey(pKey1Information  , KEY_LONG) ){
					if(HowToUseKey(pKey2Information  , KEY_LONG)){
						//K1  K2同时长按
						PBout(15)^=1;
					}
				}
				
			break;
				
			case 1:
				/* 显示校准界面 */
				if(WT(wWaitTime,1500,systemTime.ibastTime)){
					RTData[40] = 2; 
					pKey2Information->cwKeyPressStatus = 0;
					pKey1Information->cwKeyPressStatus = 0;
				}
			break;
				
			case 2:
				Num_Input(-99999,999999,5);
			break;
				
			
		}
        vTaskDelay(70);   /* 延时100个tick */		 
    }
}

/*
 *	数据处理任务 优先级10  数字越大优先级越高
 *
 *
 */
void DealData_Task(void* parameter)
{	
    while (1)
    {
		Read_AD();		 
        vTaskDelay(9);   /* 延时10个tick */		
    }
}



/*
 *	按键处理任务 优先级4  数字越大优先级越高
 *
 *
 */
void KeyDeal_Task(void* parameter)
{	
	
    while (1)
    {
		keyStaticDetect(pKey1Information);	//按键检测
		keyStaticDetect(pKey2Information);
		
		if(systemTime.ibastTime>=1000000000)//连续运行十一天，置零，防止数据溢出
			systemTime.ibastTime=0;//11 天	
		
        vTaskDelay(9);   /* 延时100个tick */		
    }
}

 

/*
 *	通讯处理任务 优先级9  数字越大优先级越高
 *	通讯发送完毕，挂起该任务
 *  通讯接收到该任务，就绪该任务
 *
 */
void ComDeal_Task(void* parameter)
{	
    while (1)
    {	
		/* 二值信号量  阻塞在此处 */
		xSemaphoreTake(ComNoticeBinarySem_Handle,portMAX_DELAY); /* 等待时间 */
		
		modbusDeal(pUsart1Inforemation);
		DMA_TX_Enable(DMA1_Channel4 , 8);
		
		
        vTaskDelay(1);   /* 延时1个tick */		
    }
}






#if 0


void CPU_Task(void* parameter)
{
 uint8_t CPU_RunInfo[400]; //保存任务运行时间信息

 while(1){
	memset(CPU_RunInfo,0,400); //信息缓冲区清零

	vTaskList((char *)&CPU_RunInfo); //获取任务运行时间信息

	//printf("---------------------------------------------\r\n");
	//printf("任务名 任务状态 优先级 剩余栈 任务序号\r\n");
	//printf("%s", CPU_RunInfo);
	//printf("---------------------------------------------\r\n");

	memset(CPU_RunInfo,0,400); //信息缓冲区清零

	vTaskGetRunTimeStats((char *)&CPU_RunInfo);

	//printf("任务名 运行计数 使用率\r\n");
	//printf("%s", CPU_RunInfo);
	////printf("---------------------------------------------\r\n\n");
	vTaskDelay(1000); /* 延时 500 个 tick */
	}	
}	


#endif




/*		*/







