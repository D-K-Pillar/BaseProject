#include "../HARDWARE/TM1621/TM1621.h"

#define WRITE PCout(15)
#define CS	  PDout(0)
#define DATA  PCout(14)


//unsigned char Smg[16]={0xad,0xad,0xad,0xad,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e};  //0~F������
/* 0-0 1-1 15-15 16-�ո� 17-0x04-���� 18-0x0B -L																					*/
unsigned char Smg0[30]={0xeb,0x60,0xc7,0xE5,0x6C,0xAD,0xaf,0xE0,0xef,0xed,0xee,0x2f,0x8b,0x67,0x8f,0x8e,0x00,0x04,0x0B};   //0~F������ //��ȷ����
unsigned char Smg1[30]={0xbe,0x06,0x7c,0x5E,0xC6,0xDA,0xFA,0x0E,0xFE,0xDE,0xEE,0xF2,0xB8,0x76,0xf8,0xe8,0x00,0x40,0xB0};  	//0~F������ //��������
unsigned char Smg2[30]={0xeb,0x60,0xc7,0xE5,0x6C,0xAD,0xaf,0xE0,0xef,0xed,0xee,0x2f,0x8b,0x67,0x8f,0x8e};




unsigned char Tab0[30]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //����


/********************��ʱ����*************************/
void delay_nms(uint n)
{
	uint i;
	while (n--)
		for (i=0;i<500;i++);
}

/********************�Ӹ�λд������*************************/
void Write_Data_H(uchar Data, uchar Cnt)	   //Data�ĸ�cntλд��TM1621����λ��ǰ
{
	uchar i;
	for (i=0;i<Cnt;i++)
	{
		WRITE=0;
		if (Data&0x80) 		                //�����λ����
			DATA=1;
		else
			DATA=0;
		nop;
		nop;
		WRITE=1;
		Data<<=1;
	}
	WRITE=0;
	DATA=0;
}

/********************�ӵ�λд������*************************/
void Write_Data_L(uchar Data,uchar Cnt)        //Data �ĵ�cntλд��TM1621����λ��ǰ
{
	unsigned char i;
	for (i=0;i<Cnt;i++)
	{
		WRITE=0;
		if (Data&0x01) 	                      //�ӵ�λ����
			DATA=1;
		else
			DATA=0;
		nop;
		nop;
		WRITE=1;
		Data>>=1;
	}
	WRITE=0;
	DATA=0;
}


/********************д���������*************************/
void WriteCmd(uchar Cmd)
{
	CS=0;
	nop;
	Write_Data_H(0x80,4);     //д�������־100
	Write_Data_H(Cmd,8);      //д����������
	CS=1;
	nop;
}


/*********ָ����ַд�����ݣ�ʵ��д���4λ************/
void WriteOneData(uchar Addr, uchar Data)
{
	CS=0;
	Write_Data_H(0xa0,3);     //д�����ݱ�־101
	Write_Data_H(Addr<<2,6);  //д���ַ����(A5-A4-A3-A2-A1-A0)�Ӹ�λ��ʼд����
	Write_Data_L(Data,4);     //д������
	CS=1;
	nop;
}

/*********����д�뷽ʽ��ÿ������Ϊ8λ��д������************/
void WriteAllData(uchar Addr,uchar *p,uchar cnt)
{
	uchar i;
	CS=0;
	Write_Data_H(0xa0,3);      //д�����ݱ�־101
	Write_Data_H(Addr<<2,6);   //д���ַ����
	for(i=0;i<cnt;i++)	    //д������
	{
		Write_Data_L(*p,8);
		p++;
	}
	CS=1;
	nop;
}
/***************����ӳ�����ʱû�ã�ע�͵�******************/

/*******************TM1621��ʼ��**********************/

void TM1621_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD |RCC_APB2Periph_GPIOB, ENABLE);//��C��ʱ�ӣ�����ʱ�ӡ�
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//ʹ��JTAGDisable��������JTAG�ӿ�
	
	
	PWR_BackupAccessCmd(ENABLE);//�����޸�RTC �ͺ󱸼Ĵ���
	RCC_LSEConfig(RCC_LSE_OFF);//�ر��ⲿ�����ⲿʱ���źŹ��� ��PC13 PC14 PC15 �ſ��Ե���ͨIO�á�
	BKP_TamperPinCmd(DISABLE);//�ر����ּ�⹦�ܣ�Ҳ���� PC13��Ҳ���Ե���ͨIO ʹ��
	
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14 | GPIO_Pin_15;//PC14-Data   PC15-WR
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //���ó����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15 | GPIO_Pin_3;//PB15 - ����
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);/*�͵�ƽ�������� �ߵ�ƽ�رձ���*/
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	
	
	GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);/*ӳ��PD01��ʹ��*/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PD0-CS
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	PWR_BackupAccessCmd(DISABLE);/* ��ֹ�޸�RTC�ͺ󱸼Ĵ���*/
	BKP_ITConfig(DISABLE);	
}

void TM1621_init(void)
{
	TM1621_Gpio_Init();
	
	CS=1;
	WRITE=1;
	DATA=1;	
	
	nop;
	delay_nms(1);
	WriteCmd(BIAS);		 //1/3ƫѹ 4������
	WriteCmd(RC);			 //�ڲ�RC��
	WriteCmd(SYSDIS);		 //��ϵͳ������LCDƫѹ������
	WriteCmd(WDTDIS);		 //��ֹ���Ź�
	WriteCmd(SYSEN);		 //��ϵͳ����
	WriteCmd(LCDON);		 //��LCDƫѹ
	
}
/*************TM1621��������*****************/
//void Clear1621()
//{
//	uchar i;
//	for(i=9;i<22;i++)
//	{
//		WriteOneData(i, 0x00);
//	}
//}


/*******************************************************************************
* ��������		   ��1621��ʾ������ʾ
* ��������		   :
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

//void Disp1621(uchar Addr, uchar Data)
//{
//	WriteOneData(Addr, Data);
//}


void Display1621(unsigned char * bmp , unsigned char dot)
{
	//unsigned char cwDispalyAddress = 11;
	/* ��ַ 11*/
	/* ����bmp��һ�����ݶ�Ӧ��ʾ���������� �ڶ�����Ӧ�ڶ�������� �Դ����� */
	
	
	/* void WriteOneData(uchar Addr, uchar Data)   */
	/* WriteAllData(uchar Addr,uchar *p,uchar cnt) */

		
	WriteAllData(18,Smg0+(0+bmp[0]),1);		//0
	
	WriteAllData(20,Smg0+(0+bmp[1]),1);		//1
	
	WriteOneData(22,Smg0[0+bmp[2]]);		//2
	WriteOneData(17,Smg0[0+bmp[2]]>>4);		//2
	
	WriteAllData(15,Smg1+(0+bmp[3]),1);		//3
	
	WriteAllData(13,Smg1+(0+bmp[4]),1);		//4
	
	WriteAllData(11,Smg1+(0+bmp[5]),1);		//5

	if(RTData[40] == 2)	//����Ǳ궨����  ����ʾС����
		dot = 3;
	
	if(dot != 5){
		if(dot <= 1 ){	//����С����
			Smg0[bmp[dot]] |= 0x10;
		}else if(dot == 2){
			Smg0[bmp[dot]] |= 0x10;
		}else if(dot >  2){
			Smg1[bmp[dot]] |= 0x01;
		}
		
		if(RTData[40] == 0 || RTData[40] == 2){	// ֻ����ʵʱֵ��ʾ����ͱ궨�������ʾС����
			
			switch(dot)
			{
				case 0:
					WriteAllData(18,Smg0+(0+bmp[0]),1);		//0
				
				break;
				
				case 1:
					WriteAllData(20,Smg0+(0+bmp[1]),1);		//1
				
				break;
				
				case 2:
					WriteOneData(22,Smg0[0+bmp[2]]);		//2
					WriteOneData(17,Smg0[0+bmp[2]]>>4);		//2
				 
				break;
				
				case 3:
					WriteAllData(15,Smg1+(0+bmp[3]),1);		//3
				
				break;
				
				case 4:
					WriteAllData(13,Smg1+(0+bmp[4]),1);		//4
				
				break;
			}
			
			
			if(dot <= 1 ){	//����С����
				Smg0[bmp[dot]] &= 0xef;
			}else if(dot == 2){
				Smg0[bmp[dot]] &= 0xef;
			}else if(dot > 2){
				Smg1[bmp[dot]] &= 0xfe;
			}
		}
	}
}


/*
 * @brief ��������ת��Ϊ�ַ�����ʾ
 *
 *
 */

void DisplayNum(int num , unsigned dot)
{
	unsigned char cwchar[6]={0,0,0,0,0,0};
	u8 b[6];
	u8 j=0,k=0,m;
	int i;
	
	if(num>999999)
		num=999999;
	if(num<-99999)
		num=-99999;	
	
	if(num>=0){
		b[5]=0;//����
	}
	else{
		num=-num;
		//num[0]=45;
		b[5]=1;//����
	}

	for(i=5 ; i>=0 ; i--){			//������ת�ַ���
		
		if(num!=0)	//�ж��Ƿ�
			cwchar[i]=(num%10);
		else{		  				//������
			if(j==0)j=i;			//����λ��
			
			if(!b[5]){
				if((dot<5-i))		//&&(RTData[11]!=2&&RTData[11]!=3)&&RTData[13]!=2
					cwchar[i]=16;
			}else{
			
				if((dot<5-i)){		//&&(RTData[11]!=2&&RTData[11]!=3)&&RTData[13]!=2
					cwchar[i]=16;
					if(k==0)
					k=i;//����С������λλ����λ
				}

			}
		}
		num/=10;
	}	
	
	if(b[5]){
		if(dot<5-k)
			cwchar[k]=17;			//����
		else{
			if(j!=0)
				cwchar[k-1]=17;		//����
			else
				cwchar[k]=17;
		}
	}	

	for(i=0;i<5;i++)  
	{  
		if(i==(5-dot)&&dot!=0){
			break;
		}
		//TM1640WriteByte(b[i]);
	}
	if(dot >= 20){/* ��˸ */
		cwchar[dot-20] = 16;
		i = 5;
	}
	Display1621(cwchar,i);
}



