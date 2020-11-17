#ifndef _TM1621_H_
#define _TM1621_H_

#include "stm32f10x.h"
#include "sys.h"
#include "./runParame/runparame.h"


#define uchar unsigned char				 //数据类型宏定义
#define uint unsigned int				 //数据类型宏定义
#define nop __nop(); //宏定义

/******************以下TM1621模块命令定义*********************/
#define SYSDIS   0x00	 //关系统振荡器和LCD偏压发生器
#define SYSEN    0x02    //打开系统振荡器

#define LCDOFF   0x04     //关LCD偏压
#define LCDON    0x06     //开LCD偏压

#define TONEON   0x12     //打开声音输出
#define TONEOFF  0x10     //关闭声音输出

#define XTAL     0x28     //外部接晶振
#define RC       0x30     //内部RC振荡

#define BIAS     0x52     //1/3偏压 4公共口,0X52=1/2偏压

#define WDTDIS   0x0a     //禁止看门狗
#define WDTEN    0x0e     //开启看门狗


extern unsigned char Smg0[30];
extern unsigned char Smg1[30];
extern unsigned char Tab0[30];


void TM1621_init(void);
//void Clear1621(void);
//void Disp1621(uchar Addr,uchar Data);
void WriteAllData(uchar Addr,uchar *p,uchar cnt);
void Display1621(unsigned char * bmp , unsigned char dot);
void DisplayNum(int num , unsigned dot);







#endif
