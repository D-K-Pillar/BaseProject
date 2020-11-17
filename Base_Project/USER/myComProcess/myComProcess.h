#ifndef _MYCOMPROCESS_H_
#define _MYCOMPROCESS_H_

#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "../HARDWARE/Usart/usart.h"
#include "./runParame/runparame.h"




unsigned short CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen);
void modbusDeal(UsartInformation *pUsart1Inforemation);




#endif

