#include "../HARDWARE/CS1237/cs1237.h"


int   ADn[60],P_ADn;
int FADNew;

/*
 *
 *		GPIO初始化
 *
 */
 
 void Delayus(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=10;  //自己定义
      while(i--) ;    
   }
}
 
static void adGPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP  ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU  ;   //上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void adOut(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
//	

//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP  ;   //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->CRH&=0XFFF0FFFF;	
	GPIOB->CRH|=0X00030000;	 //PB5、PB6、PB7推挽输出  PC2 WP  
	
}

static void adIn(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
//	

//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU  ;   //上拉输入
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->CRH&=0XFFF0FFFF;	
	GPIOB->CRH|=0X00080000;	 //PB5、PB6、PB7推挽输出  PC2 WP  
}


/*       
        Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
        R:测量噪声，R增大，动态响应变慢，收敛稳定性变好     

		R固定，Q越大，代表越信任侧量值，Q无穷代表只用测量值；
		反之，Q越小代表越信任模型预测值，Q为零则是只用模型预测。
*/

#define KALMAN_Q 0.60	//0.02

#define KALMAN_R 1.0000	//5.0000

/* 卡尔曼滤波处理 */

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
    p_mid=p_last+Q;                     //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声

    /*
     *  卡尔曼滤波的五个重要公式
     */
    kg=p_mid/(p_mid+R);                 //kg为kalman filter，R 为噪声
    x_now=x_mid+kg*(ResrcData-x_mid);   //估计出的最优值
    p_now=(1-kg)*p_mid;                 //最优值对应的covariance
    p_last = p_now;                     //更新covariance 值
    x_last = x_now;                     //更新系统状态值

    return x_now;

}

//读取当前AD值//

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
		
		FADNew=a/5;// 净重AD	
		
		/**/
		//FADNew = KalmanFilter(FADNew,KALMAN_Q,KALMAN_R);
		
		P_ADn++;
		if(P_ADn>=10)P_ADn=0;
//*/
		
		FADNew-=UserData[6];	//计算出零点
		
		if(UserData[5] <= 0)UserData[5] = 500;
		RTData[0] = ((float)FADNew)*UserData[5]/50000;
	}
}

/*=================================================
AD初始化
===================================================*/


void Ini_AD(void)
{
	unsigned char i;
	unsigned char   R_Temp;
	unsigned char   R_AdReg=0; 
	R_AdReg=0x0C;   //速度40次
	
	adGPIOInit();
	
	//R_AdReg=0x0C;   //速度10次
//	if(UserData[0] == 1)
//		R_AdReg=0x1C; //40次 
//	else if(UserData[0] == 2)
//		R_AdReg=0x0C; //10次
//	else if(UserData[0] == 3)
//		R_AdReg=0x2C; //64次
//	else if(UserData[0] == 4)
//		R_AdReg=0x3C; //1280次
	

	adIn();//输入模式
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
	adOut();//输出模式
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
	adIn();//输入模式

}


//void INT1_Isr() interrupt 2
//{
//	P12^=1;
////	if(INIT1)
//		Read_AD();
//}




