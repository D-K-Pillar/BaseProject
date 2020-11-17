#include "./app/app.h"


/*
 *	���ļ������������������
 *  ����1 - RunPage_Task  - ���ȼ� 5  - ���ڽ����л�����ʾ
 *  ����2 - DealData_Task - ���ȼ� 10 - �������ݴ�����洢
 *	����3 - KeyDeal_Task  - ���ȼ� 4  - �������ݴ���
 *	����4 - ComDeal_Task  - ���ȼ� 9  - ͨѶ��������	
 *
 *
 */








/*
 *	���л������� ���ȼ�5  ����Խ�����ȼ�Խ��
 *  configUSE_PORT_OPTIMISED_TASK_SELECTIONΪ1����������ȼ�֧�ִﵽ32�� 0����С���ȼ�
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
				DisplayNum(RTData[0],2);	//��ʾʵʱ��ֵ
			
			
				if( HowToUseKey(pKey1Information  , NOKEY) ){	//����K2����
					if( HowToUseKey(pKey2Information  , KEY_LONG) ){
						UserData[6] += FADNew;
						writeToFlash(UserData,50);
					}
				}
				
				if( HowToUseKey(pKey2Information  , NOKEY) ){	//����K1����У׼����
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
						//K1  K2ͬʱ����
						PBout(15)^=1;
					}
				}
				
			break;
				
			case 1:
				/* ��ʾУ׼���� */
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
        vTaskDelay(70);   /* ��ʱ100��tick */		 
    }
}

/*
 *	���ݴ������� ���ȼ�10  ����Խ�����ȼ�Խ��
 *
 *
 */
void DealData_Task(void* parameter)
{	
    while (1)
    {
		Read_AD();		 
        vTaskDelay(9);   /* ��ʱ10��tick */		
    }
}



/*
 *	������������ ���ȼ�4  ����Խ�����ȼ�Խ��
 *
 *
 */
void KeyDeal_Task(void* parameter)
{	
	
    while (1)
    {
		keyStaticDetect(pKey1Information);	//�������
		keyStaticDetect(pKey2Information);
		
		if(systemTime.ibastTime>=1000000000)//��������ʮһ�죬���㣬��ֹ�������
			systemTime.ibastTime=0;//11 ��	
		
        vTaskDelay(9);   /* ��ʱ100��tick */		
    }
}

 

/*
 *	ͨѶ�������� ���ȼ�9  ����Խ�����ȼ�Խ��
 *	ͨѶ������ϣ����������
 *  ͨѶ���յ������񣬾���������
 *
 */
void ComDeal_Task(void* parameter)
{	
    while (1)
    {	
		/* ��ֵ�ź���  �����ڴ˴� */
		xSemaphoreTake(ComNoticeBinarySem_Handle,portMAX_DELAY); /* �ȴ�ʱ�� */
		
		modbusDeal(pUsart1Inforemation);
		DMA_TX_Enable(DMA1_Channel4 , 8);
		
		
        vTaskDelay(1);   /* ��ʱ1��tick */		
    }
}






#if 0


void CPU_Task(void* parameter)
{
 uint8_t CPU_RunInfo[400]; //������������ʱ����Ϣ

 while(1){
	memset(CPU_RunInfo,0,400); //��Ϣ����������

	vTaskList((char *)&CPU_RunInfo); //��ȡ��������ʱ����Ϣ

	//printf("---------------------------------------------\r\n");
	//printf("������ ����״̬ ���ȼ� ʣ��ջ �������\r\n");
	//printf("%s", CPU_RunInfo);
	//printf("---------------------------------------------\r\n");

	memset(CPU_RunInfo,0,400); //��Ϣ����������

	vTaskGetRunTimeStats((char *)&CPU_RunInfo);

	//printf("������ ���м��� ʹ����\r\n");
	//printf("%s", CPU_RunInfo);
	////printf("---------------------------------------------\r\n\n");
	vTaskDelay(1000); /* ��ʱ 500 �� tick */
	}	
}	


#endif




/*		*/







