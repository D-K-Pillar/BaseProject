#ifndef _RWFLASH_H_
#define _RWFLASH_H_

#include "stm32f10x.h"
#include "sys.h"

/* STM32大容量产品每页大小2KByte，中、小容量产品每页大小1KByte */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//2048 // 0x800
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//1024
#endif

//写入的起始地址与结束地址
#define WRITE_START_ADDR  ((uint32_t)0x0801B800)	//第100页
#define WRITE_END_ADDR    ((uint32_t)0x0801CC00)	//第105页



typedef enum 
{
	FAILED = 0, 
	PASSED = !FAILED
} TestStatus;


int InternalFlash_Test(void);
unsigned char  readFromFlash(int *iData,int ilength);
int writeToFlash(int *iData,int ilength);





#endif
