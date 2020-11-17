#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

timeStruct systemTime={0};


//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟

//系统时钟初始化函数
//pll:选择的倍频数，从2开始，最大值为16		 
void Stm32_Clock_Init(u8 PLL)
{
	unsigned char temp=0;   
	MYRCC_DeInit();		  //复位并配置向量表
 	//RCC->CR|=0x00010000;  //外部高速时钟使能HSEON
	//while(!(RCC->CR>>17));//等待外部时钟就绪
	RCC->CR|=0x00000001;	//使能内部时钟  8M/2=4M!!
	while(!(RCC->CR>>1));	//内部时钟就绪
	
	//RCC->CFGR=0X00000400; //APB1=DIV 2;APB2=DIV 1;AHB=DIV 1;
	RCC->CFGR=0X00000000; //APB1=DIV 1;APB2=DIV 1;AHB=DIV 1;  内部晶振时主频为36M
	
	PLL-=2;				  //抵消2个单位（因为是从2开始的，设置0就是2）
	RCC->CFGR|=PLL<<18;   //设置PLLMUL值 2~16 
	//RCC->CFGR|=1<<16;	  //PLLSRC ON 使用内部晶振时屏蔽该语句 
	
	FLASH->ACR|=0x32;	  //FLASH 2个延时周期
	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//等待PLL锁定
	RCC->CFGR|=0x00000002;//PLL作为系统时钟	 
	while(temp!=0x02)     //等待PLL作为系统时钟设置成功
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	} 

   	
}	



////SYSCLK==72M 时钟 1/8分频  9M ，重装值为9000时为1ms中断 
void delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=~(1<<2);//  8分频为4.5M(使用内部晶振 4*9=36M）

	SysTick->CTRL|=1<<1;	//开启SYSTICK中断	
	SysTick->LOAD=4500;		//4500个脉冲为1ms  每ms中断一次
	SysTick->CTRL|=1<<0; 	//开启SYSTICK 	
	SysTick->VAL=0;
}	
//CT0位ms计数 SysTick->VAL  9个计数为1us 可以配合使用最大为1ms



void SysTick_Handler(void)
{	
	
	systemTime.ibastTime++;//基准时基 

	#if (INCLUDE_xTaskGetSchedulerState == 1 )
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
	#endif /* INCLUDE_xTaskGetSchedulerState */
		xPortSysTickHandler();
	#if (INCLUDE_xTaskGetSchedulerState == 1 )
	}
	#endif /* INCLUDE_xTaskGetSchedulerState */
	
	

	
}

//// 0-1000ms延时
void delay_ms(u8 Tms)//
{
	u32 a,b;
	a=systemTime.ibastTime;
	while(1)
	{
		b=systemTime.ibastTime;
		if(b>=a)b-=a;
		else b=(1000000000+b)-a;
		if(b>=Tms)break;			
	}	
}

void delay_us(u8 Tus)//
{
	u32 a,bb=0;
	
	a=SysTick->VAL;
	while(1)
	{
		bb=SysTick->VAL;
		if(bb<=a)bb=a-bb;
		else bb=4500+a-bb;
		if(bb>=Tus)break;
	}

}



/**
  * @brief  Provision system through reference time
  * @param  CTn   Your Time to record number
  *	@param	LT	  Timing duration
  * @param  CT0   Base Time
  * @retval Time up return 1
			Not time return 0
			
  */
u8 WT(u32 CTn,u32 LT,u32 CT0)//
{
	u32 m;
	if(CT0>=CTn) //Judgment the num is legal
		m=CT0-CTn;
	else 
		m=1000000000+CT0+1-CTn;
	if(m>=LT) // Time up 
		return 1;
	else      // Not time
		return 0;
}



