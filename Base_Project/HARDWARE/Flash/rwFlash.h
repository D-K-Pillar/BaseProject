#ifndef _RWFLASH_H_
#define _RWFLASH_H_

#include "stm32f10x.h"
#include "sys.h"

/* STM32��������Ʒÿҳ��С2KByte���С�С������Ʒÿҳ��С1KByte */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//2048 // 0x800
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//1024
#endif

//д�����ʼ��ַ�������ַ
#define WRITE_START_ADDR  ((uint32_t)0x0801B800)	//��100ҳ
#define WRITE_END_ADDR    ((uint32_t)0x0801CC00)	//��105ҳ



typedef enum 
{
	FAILED = 0, 
	PASSED = !FAILED
} TestStatus;


int InternalFlash_Test(void);
unsigned char  readFromFlash(int *iData,int ilength);
int writeToFlash(int *iData,int ilength);





#endif
