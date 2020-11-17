#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"


typedef struct DescripKeyInformation
{
	unsigned char cwKeyPressStatus;	/*  ����״̬ 0-δ����        1-������
									2-�ж�Ϊ�̰�      3-�ж�Ϊ���� 	
									4-�̰�����		5-��������
	*/
	unsigned int hwKeyPressTime;	/* ����ʱ�� */
	unsigned char cwKeyNum	/* ������� �˲��������ڳ�ʼ��ʱ�޸� */;
}KeyInformation,*pKeyInformation;



/*
	KeyPressStatus = 1 �̰� 
					 2 ����
					 4 �̰�����
					 8 ��������

*/


#define NOKEY				0
#define KEY_SHORT  			1
#define KEY_LONG   			2
#define KEY_SHORTFREED		4
#define KEY_LONGFREED		8


#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//GPIO_Pin_9
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//GPIO_Pin_4



extern KeyInformation *pKey1Information ;
extern KeyInformation *pKey2Information ;

void keyStaticDetect(KeyInformation *pKeyInformation);	//�������
void KeyInit(void);

unsigned char  HowToUseKey(KeyInformation *pKeyInformation , unsigned char KeyPressStatus);






#endif

