#include "../HARDWARE/CS1237/cs1237.h"


int   ADn[60],P_ADn;
int FADNew;

/*
 *
 *		GPIO��ʼ��
 *
 */
 
 void Delayus(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=10;  //�Լ�����
      while(i--) ;    
   }
}
 
static void adGPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP  ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU  ;   //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void adOut(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
//	

//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP  ;   //�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->CRH&=0XFFF0FFFF;	
	GPIOB->CRH|=0X00030000;	 //PB5��PB6��PB7�������  PC2 WP  
	
}

static void adIn(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
//	

//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU  ;   //��������
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->CRH&=0XFFF0FFFF;	
	GPIOB->CRH|=0X00080000;	 //PB5��PB6��PB7�������  PC2 WP  
}


/*       
        Q:����������Q���󣬶�̬��Ӧ��죬�����ȶ��Ա仵
        R:����������R���󣬶�̬��Ӧ�����������ȶ��Ա��     

		R�̶���QԽ�󣬴���Խ���β���ֵ��Q�������ֻ�ò���ֵ��
		��֮��QԽС����Խ����ģ��Ԥ��ֵ��QΪ������ֻ��ģ��Ԥ�⡣
*/

#define KALMAN_Q 0.60	//0.02

#define KALMAN_R 1.0000	//5.0000

/* �������˲����� */

static double KalmanFilter(const double ResrcData,double ProcessNiose_Q,double MeasureNoise_R)
{

    double R = MeasureNoise_R;
    double Q = ProcessNiose_Q;

    static double x_last;
    double x_mid = x_last;
    double x_now;

    static double p_last;
    double p_mid ;
    double p_now;

    double kg;

    x_mid=x_last;                       //x_last=x(k-1|k-1),x_mid=x(k|k-1)
    p_mid=p_last+Q;                     //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����

    /*
     *  �������˲��������Ҫ��ʽ
     */
    kg=p_mid/(p_mid+R);                 //kgΪkalman filter��R Ϊ����
    x_now=x_mid+kg*(ResrcData-x_mid);   //���Ƴ�������ֵ
    p_now=(1-kg)*p_mid;                 //����ֵ��Ӧ��covariance
    p_last = p_now;                     //����covariance ֵ
    x_last = x_now;                     //����ϵͳ״ֵ̬

    return x_now;

}

//��ȡ��ǰADֵ//

void Read_AD(void)
{

	int a,b;
	int   c=0;
	char   i=0;
	//static int test,num;
	if(AD_In==0)
	{

		for(i = 0; i < 24; i++)				//send 24 CLK,for recive data 
		{
			c <<= 1;
			H_CLK;delay_us(20);
			L_CLK;delay_us(20);
			if(AD_In)c |=1;
		}
		H_CLK;delay_us(20);
		L_CLK;delay_us(20);
		H_CLK;delay_us(20);
		L_CLK;delay_us(20);
		H_CLK;delay_us(20);
		L_CLK;delay_us(20);
		//EA = 1;
		c>>=4;
		//RTData[0]=c;
		
		if((c&0x80000) == 0x80000)
			c|=0xFFF00000;
		
//		{
//			c = KalmanFilter(c,KALMAN_Q,KALMAN_R);
//			FADNew=c;
//		}
		
//*		
		ADn[P_ADn]=c;	
		a=0;
		b=P_ADn;
		for(i=0;i<10;i++)
		{
			a+=ADn[b];
			b--;
			if(b<=0)b=10;
		} 
		
		FADNew=a/5;// ����AD	
		
		/**/
		//FADNew = KalmanFilter(FADNew,KALMAN_Q,KALMAN_R);
		
		P_ADn++;
		if(P_ADn>=10)P_ADn=0;
//*/
		
		FADNew-=UserData[6];	//��������
		
		if(UserData[5] <= 0)UserData[5] = 500;
		RTData[0] = ((float)FADNew)*UserData[5]/50000;
	}
}

/*=================================================
AD��ʼ��
===================================================*/


void Ini_AD(void)
{
	unsigned char i;
	unsigned char   R_Temp;
	unsigned char   R_AdReg=0; 
	R_AdReg=0x0C;   //�ٶ�40��
	
	adGPIOInit();
	
	//R_AdReg=0x0C;   //�ٶ�10��
//	if(UserData[0] == 1)
//		R_AdReg=0x1C; //40�� 
//	else if(UserData[0] == 2)
//		R_AdReg=0x0C; //10��
//	else if(UserData[0] == 3)
//		R_AdReg=0x2C; //64��
//	else if(UserData[0] == 4)
//		R_AdReg=0x3C; //1280��
	

	adIn();//����ģʽ
	L_CLK;
	delay_ms(250);
	delay_ms(250);
	L_CLK;
	while(AD_In==0);
	for(i = 0; i < 29; i++)
	{
		H_CLK;Delayus(20);
		L_CLK;Delayus(20);
	}
	
	//----------------------------------
	//5:clk30-clk36(send write command)
	//----------------------------------		
	R_Temp =0XCA;					//write command (0x65); left move 1 bit
	adOut();//���ģʽ
	for(i = 0; i < 7; i ++)
	{
		H_CLK;Delayus(20);
		if(R_Temp&0x80)				//MSB
		{
			H_Do;
		}
		else
		{
			L_Do;
		}
		R_Temp = R_Temp << 1;
		L_CLK;Delayus(20);
	}

	//----------------------------------
	//6:clk37
	//----------------------------------
	H_CLK;Delayus(20);
	L_CLK;Delayus(20);
	
	//----------------------------------
	//7:clk38-clk45(write to register)
	//----------------------------------
	R_Temp = R_AdReg;	
	for(i = 0; i < 8; i ++)
	{
		H_CLK;Delayus(20);
		if(R_Temp&0x80){		//MSB
			H_Do;
		}
		else{
			L_Do;
		}
		R_Temp = R_Temp << 1;
		
		L_CLK;Delayus(20);
	}
	
	//----------------------------------
	//8:clk46
	//----------------------------------	
	H_Do;
	H_CLK;Delayus(20);
	L_CLK;Delayus(20);
	adIn();//����ģʽ

}


//void INT1_Isr() interrupt 2
//{
//	P12^=1;
////	if(INIT1)
//		Read_AD();
//}




