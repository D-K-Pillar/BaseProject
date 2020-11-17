#ifndef _RUNPARAME_H_
#define _RUNPARAME_H_

#include "stm32f10x.h"
#include "sys.h"


typedef struct DescripData // 描述参数结构体
{
	int iUpperLimit;//参数上限值
	int iLowerLimit;//参数下限值
	
	unsigned char cUnit;//单位
	unsigned char cDot; //小数点
	
	/* 有效设置   
		规定此参数为0则无效设置，不予显示
		第一位为1则模式1此参数有效
		第二位为1则模式2此参数有效
		第三位为1则模式3此参数有效
		第四位为1则模式4此参数有效	
		.
		.
		.
		第九位有效则为高级参数 - 此系列参数进入需要密码 
	*/	
	unsigned short int nwSet;

	unsigned char cwName[10];  /* 参数名称  英文只需一个字节  中文一个字占用两个空间 此程序使用英文参数 */
	
	
	/*
		该参数初始值，若空间有限，建议减少初值
	*/
	int iInitValue1;		   //初值1 模式1初值
	int iInitValue2;		   //初值2 模式1初值
	int iInitValue3;		   //初值3 模式1初值
	int iInitValue4;		   //初值4 模式1初值
									
}DescripData,*pDescripData;



extern const DescripData UserDataDescrip[50];
extern int UserData[50];
extern int RTData[50];


#endif


