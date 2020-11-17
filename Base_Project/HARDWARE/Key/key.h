#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"


typedef struct DescripKeyInformation
{
	unsigned char cwKeyPressStatus;	/*  按键状态 0-未按下   1-消抖中
									2-判断为短按    3-判断为长按 	
									4-短按松手		5-长按松手
	*/
	unsigned int hwKeyPressTime;	/* 按键时长 */
	unsigned char cwKeyNum	/* 按键编号 此参数建议在初始化时修改 */;
}KeyInformation,*pKeyInformation;


#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//GPIO_Pin_9
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//GPIO_Pin_4



extern KeyInformation *pKey1Information ;
extern KeyInformation *pKey2Information ;

void keyStaticDetect(KeyInformation *pKeyInformation);	//按键检测
void KeyInit(void);






#endif

