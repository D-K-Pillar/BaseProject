#ifndef __CS1237_H_
#define __CS1237_H_
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "./runParame/runparame.h"



//#define AD_Do   PBout(12)
//#define AD_Clk	PBout(13)
#define AD_In 	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)



#define H_CLK GPIOB->ODR |= 1<<13;
#define L_CLK GPIOB->ODR &= 0xdfff;

#define H_Do GPIOB->ODR |= 1<<12;
#define L_Do GPIOB->ODR &= 0xefff;

void Read_AD(void);     //读取当前AD值
void Ini_AD(void);      //写入初始化数据
unsigned char RTU03_Process(char *buffer);


extern int FADNew;
#endif

