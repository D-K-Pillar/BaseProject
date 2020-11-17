#include "../HARDWARE/Key/key.h"




KeyInformation Key1Information={0,0,1};	//按键1初始化
KeyInformation Key2Information={0,0,2};	//按键2初始化

KeyInformation *pKey1Information = &Key1Information ;
KeyInformation *pKey2Information = &Key2Information ;


/*
	功能 判断哪一个按键被按下
	参数
	返回值:对应的按键位数，如按键1、2同时被按下 则返回0x03  0000 0011

 */
static u16 whichKeyInput(void)		
{
	u16 wWhichKey=0;
	
	//pKey1Information = &Key1Information ;
	
	if(KEY1 == 0)
		wWhichKey|=0x01;
	if(KEY2 == 0)
		wWhichKey|=0x02;
	return wWhichKey;
}

void keyStaticDetect(KeyInformation *pKeyInformation)	//按键检测
{
	u16 KeyStatus;
	KeyStatus = whichKeyInput();
	
	if(((KeyStatus&(pKeyInformation->cwKeyNum)) == pKeyInformation->cwKeyNum)||(pKeyInformation->cwKeyPressStatus!=0)){
		//按键按下或按键已经按下
		switch(pKeyInformation->cwKeyPressStatus)
		{
			/* 未按下,进入该函数*/
			case 0:
				pKeyInformation->hwKeyPressTime = systemTime.ibastTime;	//将按键时间置零
				pKeyInformation->cwKeyPressStatus = 1;  //跳转至消抖状态
			break;
			
			/* 消抖中 */
			case 1:
				if((systemTime.ibastTime-pKeyInformation->hwKeyPressTime)>20){	//20ms消抖时间
					if((KeyStatus&(pKeyInformation->cwKeyNum)) == pKeyInformation->cwKeyNum){//按键按下，消抖完成，判断为短按
						pKeyInformation->cwKeyPressStatus = 2;  //跳转至短按状态
					}else{
						pKeyInformation->cwKeyPressStatus = 0;	//误操作 跳转至未按下状态
					}
				}
			break;

			/* 短按中  在一定时间内判断短按次数 则为连点*/
			case 2:
				if(((KeyStatus&(pKeyInformation->cwKeyNum)) == pKeyInformation->cwKeyNum)){
					if((systemTime.ibastTime-pKeyInformation->hwKeyPressTime)>1000){	//按键长按
						pKeyInformation->cwKeyPressStatus = 3;	//按键状态未长按
					}
					
				}else{	//可判断为短按松手
					pKeyInformation->cwKeyPressStatus = 4;
				}
			break;

			/* 长按中 */
			case 3:
				if(((KeyStatus&(pKeyInformation->cwKeyNum)) != pKeyInformation->cwKeyNum)){
					pKeyInformation->cwKeyPressStatus = 5;	//判断为长按松手
				}
			break;

			/* 短按松手 */
			case 4:
				
			break;

			/* 长按松手 */
			case 5:
				
			break;			

			case 6:
				/* 等待按键松手状态 */
				if(((KeyStatus&(pKeyInformation->cwKeyNum)) != pKeyInformation->cwKeyNum)) 
					pKeyInformation->cwKeyPressStatus = 0;
			break;
			
			default:
				pKeyInformation->cwKeyPressStatus = 0;//按键异常 转换为未按下状态
			break;
		}
		
	}else{

	}
}


/*
 *	@brief Input statue what do you need,if get it,return 1 else return 0
 *
 *  @param pKeyInformation key's struct
 *	@param KeyPressStatus  key's status 
 *
 *	@retval 1 ok , 0  none.
 */
unsigned char  HowToUseKey(KeyInformation *pKeyInformation , unsigned char KeyPressStatus)
{
	/*  按键状态 0-未按下            1-消抖中
				 2-判断为短按      3-判断为长按 	
				 4-短按松手		  5-长按松手
				 
		KeyPressStatus = 1 短按 
						 2 长按
						 4 短按松手
						 8 长按松手
		
	*/
	unsigned char status = 0x00;

	if(KeyPressStatus == NOKEY){
		/* */
		if(pKeyInformation->cwKeyPressStatus == 0){
			status = 1;
		}else{
//			if(pKeyInformation->cwKeyPressStatus != 1)
//				pKeyInformation->cwKeyPressStatus = 0;
		}
	}

	
	if((KeyPressStatus & KEY_LONGFREED) == KEY_LONGFREED){
		/* 长按松手处理 */
		if( pKeyInformation->cwKeyPressStatus == 5 ){
			pKeyInformation->cwKeyPressStatus = 0;
			status = 1;
		}else if( pKeyInformation->cwKeyPressStatus == 4 ){
			pKeyInformation->cwKeyPressStatus = 0;
		}
	} 
 
	if((KeyPressStatus & KEY_SHORTFREED) == KEY_SHORTFREED){
		/* 短按松手处理 */
		if( pKeyInformation->cwKeyPressStatus == 4 ){
			pKeyInformation->cwKeyPressStatus = 0;
			status = 1;
		}else if( pKeyInformation->cwKeyPressStatus == 5 ){
			pKeyInformation->cwKeyPressStatus = 0;
		}
	}

	
	if((KeyPressStatus & KEY_LONG) == KEY_LONG){
		/* 长按处理 */
		if( pKeyInformation->cwKeyPressStatus == 3 ){
			pKeyInformation->cwKeyPressStatus = 0;
			status = 1;
		}else if( pKeyInformation->cwKeyPressStatus > 3 ){
			pKeyInformation->cwKeyPressStatus = 0;
		}
	}

	if((KeyPressStatus & KEY_SHORT) == KEY_SHORT){
		/* 短按处理 */
		if( pKeyInformation->cwKeyPressStatus == 2 ){
			pKeyInformation->cwKeyPressStatus = 0;
			status = 1;
		}else if( pKeyInformation->cwKeyPressStatus > 2 ){
			pKeyInformation->cwKeyPressStatus = 0;
		}
	}
	
	if(status == 1)
		return 1;
	
	return 0;
	
}


/*
 *	@brief Init key's hardware
 *
 *  @param None
 *
 *	@retval None
 */


void KeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU  ;   //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}









