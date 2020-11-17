#ifndef _RUNPARAME_H_
#define _RUNPARAME_H_

#include "stm32f10x.h"
#include "sys.h"


typedef struct DescripData // ���������ṹ��
{
	int iUpperLimit;//��������ֵ
	int iLowerLimit;//��������ֵ
	
	unsigned char cUnit;//��λ
	unsigned char cDot; //С����
	
	/* ��Ч����   
		�涨�˲���Ϊ0����Ч���ã�������ʾ
		��һλΪ1��ģʽ1�˲�����Ч
		�ڶ�λΪ1��ģʽ2�˲�����Ч
		����λΪ1��ģʽ3�˲�����Ч
		����λΪ1��ģʽ4�˲�����Ч	
		.
		.
		.
		�ھ�λ��Ч��Ϊ�߼����� - ��ϵ�в���������Ҫ���� 
	*/	
	unsigned short int nwSet;

	unsigned char cwName[10];  /* ��������  Ӣ��ֻ��һ���ֽ�  ����һ����ռ�������ռ� �˳���ʹ��Ӣ�Ĳ��� */
	
	
	/*
		�ò�����ʼֵ�����ռ����ޣ�������ٳ�ֵ
	*/
	int iInitValue1;		   //��ֵ1 ģʽ1��ֵ
	int iInitValue2;		   //��ֵ2 ģʽ1��ֵ
	int iInitValue3;		   //��ֵ3 ģʽ1��ֵ
	int iInitValue4;		   //��ֵ4 ģʽ1��ֵ
									
}DescripData,*pDescripData;



extern const DescripData UserDataDescrip[50];
extern int UserData[50];
extern int RTData[50];


#endif


