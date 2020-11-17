#ifndef _TM1621_H_
#define _TM1621_H_

#include "stm32f10x.h"
#include "sys.h"
#include "./runParame/runparame.h"


#define uchar unsigned char				 //�������ͺ궨��
#define uint unsigned int				 //�������ͺ궨��
#define nop __nop(); //�궨��

/******************����TM1621ģ�������*********************/
#define SYSDIS   0x00	 //��ϵͳ������LCDƫѹ������
#define SYSEN    0x02    //��ϵͳ����

#define LCDOFF   0x04     //��LCDƫѹ
#define LCDON    0x06     //��LCDƫѹ

#define TONEON   0x12     //���������
#define TONEOFF  0x10     //�ر��������

#define XTAL     0x28     //�ⲿ�Ӿ���
#define RC       0x30     //�ڲ�RC��

#define BIAS     0x52     //1/3ƫѹ 4������,0X52=1/2ƫѹ

#define WDTDIS   0x0a     //��ֹ���Ź�
#define WDTEN    0x0e     //�������Ź�


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
