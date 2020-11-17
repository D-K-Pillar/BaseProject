#include "../HARDWARE/TM1621/TM1621.h"

#define WRITE PCout(15)
#define CS	  PDout(0)
#define DATA  PCout(14)


//unsigned char Smg[16]={0xad,0xad,0xad,0xad,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e,0x8e};  //0~F字型码
/* 0-0 1-1 15-15 16-空格 17-0x04-负号 18-0x0B -L																					*/
unsigned char Smg0[30]={0xeb,0x60,0xc7,0xE5,0x6C,0xAD,0xaf,0xE0,0xef,0xed,0xee,0x2f,0x8b,0x67,0x8f,0x8e,0x00,0x04,0x0B};   //0~F字型码 //正确字形
unsigned char Smg1[30]={0xbe,0x06,0x7c,0x5E,0xC6,0xDA,0xFA,0x0E,0xFE,0xDE,0xEE,0xF2,0xB8,0x76,0xf8,0xe8,0x00,0x40,0xB0};  	//0~F字型码 //反向字形
unsigned char Smg2[30]={0xeb,0x60,0xc7,0xE5,0x6C,0xAD,0xaf,0xE0,0xef,0xed,0xee,0x2f,0x8b,0x67,0x8f,0x8e};




unsigned char Tab0[30]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //清屏


/********************延时函数*************************/
void delay_nms(uint n)
{
	uint i;
	while (n--)
		for (i=0;i<500;i++);
}

/********************从高位写入数据*************************/
void Write_Data_H(uchar Data, uchar Cnt)	   //Data的高cnt位写入TM1621，高位在前
{
	uchar i;
	for (i=0;i<Cnt;i++)
	{
		WRITE=0;
		if (Data&0x80) 		                //从最高位发送
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

/********************从低位写入数据*************************/
void Write_Data_L(uchar Data,uchar Cnt)        //Data 的低cnt位写入TM1621，低位在前
{
	unsigned char i;
	for (i=0;i<Cnt;i++)
	{
		WRITE=0;
		if (Data&0x01) 	                      //从低位发送
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


/********************写入控制命令*************************/
void WriteCmd(uchar Cmd)
{
	CS=0;
	nop;
	Write_Data_H(0x80,4);     //写入命令标志100
	Write_Data_H(Cmd,8);      //写入命令数据
	CS=1;
	nop;
}


/*********指定地址写入数据，实际写入后4位************/
void WriteOneData(uchar Addr, uchar Data)
{
	CS=0;
	Write_Data_H(0xa0,3);     //写入数据标志101
	Write_Data_H(Addr<<2,6);  //写入地址数据(A5-A4-A3-A2-A1-A0)从高位开始写数据
	Write_Data_L(Data,4);     //写入数据
	CS=1;
	nop;
}

/*********连续写入方式，每次数据为8位，写入数据************/
void WriteAllData(uchar Addr,uchar *p,uchar cnt)
{
	uchar i;
	CS=0;
	Write_Data_H(0xa0,3);      //写入数据标志101
	Write_Data_H(Addr<<2,6);   //写入地址数据
	for(i=0;i<cnt;i++)	    //写入数据
	{
		Write_Data_L(*p,8);
		p++;
	}
	CS=1;
	nop;
}
/***************这个子程序暂时没用，注释掉******************/

/*******************TM1621初始化**********************/

void TM1621_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD |RCC_APB2Periph_GPIOB, ENABLE);//开C口时钟，复用时钟。
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//使能JTAGDisable，即禁用JTAG接口
	
	
	PWR_BackupAccessCmd(ENABLE);//允许修改RTC 和后备寄存器
	RCC_LSEConfig(RCC_LSE_OFF);//关闭外部低速外部时钟信号功能 后，PC13 PC14 PC15 才可以当普通IO用。
	BKP_TamperPinCmd(DISABLE);//关闭入侵检测功能，也就是 PC13，也可以当普通IO 使用
	
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14 | GPIO_Pin_15;//PC14-Data   PC15-WR
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15 | GPIO_Pin_3;//PB15 - 背光
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);/*低电平开启背光 高电平关闭背光*/
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	
	
	GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);/*映射PD01的使能*/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PD0-CS
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	PWR_BackupAccessCmd(DISABLE);/* 禁止修改RTC和后备寄存器*/
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
	WriteCmd(BIAS);		 //1/3偏压 4公共口
	WriteCmd(RC);			 //内部RC振荡
	WriteCmd(SYSDIS);		 //关系统振荡器和LCD偏压发生器
	WriteCmd(WDTDIS);		 //禁止看门狗
	WriteCmd(SYSEN);		 //打开系统振荡器
	WriteCmd(LCDON);		 //开LCD偏压
	
}
/*************TM1621清屏函数*****************/
//void Clear1621()
//{
//	uchar i;
//	for(i=9;i<22;i++)
//	{
//		WriteOneData(i, 0x00);
//	}
//}


/*******************************************************************************
* 函数名称		   ：1621显示函数显示
* 函数功能		   :
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

//void Disp1621(uchar Addr, uchar Data)
//{
//	WriteOneData(Addr, Data);
//}


void Display1621(unsigned char * bmp , unsigned char dot)
{
	//unsigned char cwDispalyAddress = 11;
	/* 地址 11*/
	/* 数组bmp第一个数据对应显示最左边数码管 第二个对应第二个数码管 以此类推 */
	
	
	/* void WriteOneData(uchar Addr, uchar Data)   */
	/* WriteAllData(uchar Addr,uchar *p,uchar cnt) */

		
	WriteAllData(18,Smg0+(0+bmp[0]),1);		//0
	
	WriteAllData(20,Smg0+(0+bmp[1]),1);		//1
	
	WriteOneData(22,Smg0[0+bmp[2]]);		//2
	WriteOneData(17,Smg0[0+bmp[2]]>>4);		//2
	
	WriteAllData(15,Smg1+(0+bmp[3]),1);		//3
	
	WriteAllData(13,Smg1+(0+bmp[4]),1);		//4
	
	WriteAllData(11,Smg1+(0+bmp[5]),1);		//5

	if(RTData[40] == 2)	//如果是标定界面  则显示小数点
		dot = 3;
	
	if(dot != 5){
		if(dot <= 1 ){	//绘制小数点
			Smg0[bmp[dot]] |= 0x10;
		}else if(dot == 2){
			Smg0[bmp[dot]] |= 0x10;
		}else if(dot >  2){
			Smg1[bmp[dot]] |= 0x01;
		}
		
		if(RTData[40] == 0 || RTData[40] == 2){	// 只有在实时值显示界面和标定界面才显示小数点
			
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
			
			
			if(dot <= 1 ){	//绘制小数点
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
 * @brief 将整型数转换为字符并显示
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
		b[5]=0;//正数
	}
	else{
		num=-num;
		//num[0]=45;
		b[5]=1;//负数
	}

	for(i=5 ; i>=0 ; i--){			//整型数转字符数
		
		if(num!=0)	//判断是否
			cwchar[i]=(num%10);
		else{		  				//无数字
			if(j==0)j=i;			//数据位数
			
			if(!b[5]){
				if((dot<5-i))		//&&(RTData[11]!=2&&RTData[11]!=3)&&RTData[13]!=2
					cwchar[i]=16;
			}else{
			
				if((dot<5-i)){		//&&(RTData[11]!=2&&RTData[11]!=3)&&RTData[13]!=2
					cwchar[i]=16;
					if(k==0)
					k=i;//负数小数点首位位数定位
				}

			}
		}
		num/=10;
	}	
	
	if(b[5]){
		if(dot<5-k)
			cwchar[k]=17;			//负号
		else{
			if(j!=0)
				cwchar[k-1]=17;		//负号
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
	if(dot >= 20){/* 闪烁 */
		cwchar[dot-20] = 16;
		i = 5;
	}
	Display1621(cwchar,i);
}



