#ifndef __RUNPAGE_H_
#define __RUNPAGE_H_

#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "./runParame/runparame.h"
#include "../HARDWARE/TM1621/TM1621.h"
#include "../HARDWARE/Flash/rwFlash.h"


void Num_Input(int32_t InDmin,u32 InDmax,u8 dot);

#endif


