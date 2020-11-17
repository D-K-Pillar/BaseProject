#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

timeStruct systemTime={0};


//��ʼ���ӳٺ���
//��ʹ��OS��ʱ��,�˺������ʼ��OS��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��

//ϵͳʱ�ӳ�ʼ������
//pll:ѡ��ı�Ƶ������2��ʼ�����ֵΪ16		 
void Stm32_Clock_Init(u8 PLL)
{
	unsigned char temp=0;   
	MYRCC_DeInit();		  //��λ������������
 	//RCC->CR|=0x00010000;  //�ⲿ����ʱ��ʹ��HSEON
	//while(!(RCC->CR>>17));//�ȴ��ⲿʱ�Ӿ���
	RCC->CR|=0x00000001;	//ʹ���ڲ�ʱ��  8M/2=4M!!
	while(!(RCC->CR>>1));	//�ڲ�ʱ�Ӿ���
	
	//RCC->CFGR=0X00000400; //APB1=DIV 2;APB2=DIV 1;AHB=DIV 1;
	RCC->CFGR=0X00000000; //APB1=DIV 1;APB2=DIV 1;AHB=DIV 1;  �ڲ�����ʱ��ƵΪ36M
	
	PLL-=2;				  //����2����λ����Ϊ�Ǵ�2��ʼ�ģ�����0����2��
	RCC->CFGR|=PLL<<18;   //����PLLMULֵ 2~16 
	//RCC->CFGR|=1<<16;	  //PLLSRC ON ʹ���ڲ�����ʱ���θ���� 
	
	FLASH->ACR|=0x32;	  //FLASH 2����ʱ����
	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//�ȴ�PLL����
	RCC->CFGR|=0x00000002;//PLL��Ϊϵͳʱ��	 
	while(temp!=0x02)     //�ȴ�PLL��Ϊϵͳʱ�����óɹ�
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	} 

   	
}	



////SYSCLK==72M ʱ�� 1/8��Ƶ  9M ����װֵΪ9000ʱΪ1ms�ж� 
void delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=~(1<<2);//  8��ƵΪ4.5M(ʹ���ڲ����� 4*9=36M��

	SysTick->CTRL|=1<<1;	//����SYSTICK�ж�	
	SysTick->LOAD=4500;		//4500������Ϊ1ms  ÿms�ж�һ��
	SysTick->CTRL|=1<<0; 	//����SYSTICK 	
	SysTick->VAL=0;
}	
//CT0λms���� SysTick->VAL  9������Ϊ1us �������ʹ�����Ϊ1ms



void SysTick_Handler(void)
{	
	
	systemTime.ibastTime++;//��׼ʱ�� 

	#if (INCLUDE_xTaskGetSchedulerState == 1 )
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
	#endif /* INCLUDE_xTaskGetSchedulerState */
		xPortSysTickHandler();
	#if (INCLUDE_xTaskGetSchedulerState == 1 )
	}
	#endif /* INCLUDE_xTaskGetSchedulerState */
	
	

	
}

//// 0-1000ms��ʱ
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



