#include "./RunPage/runPage.h"

int This_Dn;
u8 Len_Data;
u8 This_Dnn;
u32 This_Dnx;

const uint32_t MAXNN[]={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};

/*
	校准功能界面
*/
void calPage(void)
{
	
}

void Num_Input(int32_t InDmin,u32 InDmax,u8 dot)
{
	u8 m;
	u32 a;
//	u8 dot2=0;
	float fa;
	static u32 Tms_glint,Tms_Num;
	//static int32_t OldData,YesDot;
	switch(RTData[20])//仅闪烁控制
	{
		case 0:{
			if(WT(Tms_glint,300,systemTime.ibastTime)){
				Tms_Num=systemTime.ibastTime;
				Tms_glint=systemTime.ibastTime;
				RTData[20]++;
				DisplayNum(RTData[21],5);//显示实时值
			}
			break;
		}
		case 1:{
			if(WT(Tms_Num,300,systemTime.ibastTime)){
				Tms_glint=systemTime.ibastTime;
				Tms_Num=systemTime.ibastTime;
				DisplayNum(RTData[21],20+This_Dn);//闪烁选中位置
				RTData[20]=0;
			}
			break;
		}
		default:
			RTData[20]=0;
	}
	
	switch(RTData[22])//输入控制
	{
		case 0:{
			m=0;
			while(m<10)
			{
				if(InDmax>=MAXNN[m])m+=1;
				else break;
			}
			Len_Data=m;
			This_Dn=5;
			This_Dnn=0;
			//OldData=RTData[21];
			RTData[22]=1;
//			if(dot!=0)YesDot=1;
//			else YesDot=0;
			break;
		}
		case 1:{//显示
			m=dot;		
			a=MAXNN[5-This_Dn];
			This_Dnx=a;
			if(RTData[21]>=0)This_Dnn=(RTData[21]%(a*10))/This_Dnx;
			else This_Dnn=((-RTData[21])%a)/This_Dnx;
			
			RTData[22]++;
			break;
		}
		case 2:{
			if(pKey2Information->cwKeyPressStatus == 4){		//+键		
				if(RTData[21]>=0)RTData[21]-=This_Dnn*This_Dnx;
				else RTData[21]+=This_Dnn*This_Dnx;
				This_Dnn++;
				
				
				
				if(This_Dn==0&&Len_Data!=0){ //最高位特殊处理
					a=This_Dnn*This_Dnx;
					if(RTData[21]<0){
						RTData[21]=-RTData[21];
						This_Dnn=0;
						a=0;						
					}
					
					if(a>InDmax){
						if(InDmin>=0){a=0;This_Dnn=0;}
						else {a=0;RTData[21]=-RTData[21];This_Dnn=0;}
					}
					
				}
				else{
					This_Dnn%=10;
					a=This_Dnn*This_Dnx;
				}			
				
				if(RTData[21]>=0)RTData[21]+=a;
				else RTData[21]-=a;
				

				RTData[22]=2;
				pKey2Information->cwKeyPressStatus = 0;
				//TM1640_THI_NUM(RTData[21],dot2);
				//USEKEY
			}
			
			if(pKey2Information->cwKeyPressStatus == 5){
				pKey2Information->cwKeyPressStatus = 0;
			}
			
			if(pKey1Information->cwKeyPressStatus == 4){//移位
				This_Dn--;
				if(This_Dn<=(5-Len_Data))
					This_Dn=5;
				RTData[22]=1;
				if(This_Dn<0)
					This_Dn=5;
				RTData[22]=1;
				pKey1Information->cwKeyPressStatus = 0;
			}
			
			if(pKey1Information->cwKeyPressStatus == 3){
				//长按移位按键，保存并退出
				pKey1Information->cwKeyPressStatus = 0;
				
				fa = (float)RTData[21]/RTData[0];

				RTData[49] = UserData[5];
				RTData[49]*=fa;
				
				if(RTData[49]>=100){		//防止误操作
					UserData[5]*=fa;
					writeToFlash(UserData,50);
				}
				RTData[40]  = 0 ;
				This_Dn = 0; 
				Len_Data= 0;
				This_Dnn= 0;
				This_Dnx= 0;
				RTData[22] = 0;
			}
			if(pKey1Information->cwKeyPressStatus == 5){
				pKey1Information->cwKeyPressStatus = 0;
			}
			break;	
		}
	}
}




/*
 * @brief 数字输入函数
 * @param 
 * @retval None
 */
//const uint32_t MAXNN[]={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};

void InputNum(int Min , int Max , unsigned char len , unsigned char dot , int InputNumber)
{
	static int LChangeNum   = 0;	//要修改的数字
	static int LOperateStep = 0;	//操作步数 0：接收输入数字 1:开始修改数字
	static unsigned char cwCursor    = 0;   	//光标位置-当前修改位
	static unsigned char cwCursorNum = 0;   	//当前修改位值
	static unsigned char cwDataLen   = 0;		//数据长度
	static unsigned int  cwDataMultiple = 0;	//当前位数据倍率
	
	int i = 0;

	//DisplayNumber()

	switch(LOperateStep)
	{
		case 0:
		{
			LChangeNum = InputNumber;
			cwDataLen = len;

			cwCursor		= 0;	//光标在最左边
			cwCursorNum		= 0;	//光标处值为0

			while(i < 10){	//计算数据最大位数
				if(Max >= MAXNN[i])
					i++;
				else 
					break;
			}

			cwDataLen = i;

			LOperateStep++;	
			break;
		}

		case 1:
		{
			cwDataMultiple = (MAXNN[cwDataLen-cwCursor]);	/* 计算当前位置倍率 */
			if(LChangeNum >= 0)
				cwCursorNum = (LChangeNum % cwDataMultiple)/(cwDataMultiple / 10);
			else
				cwCursorNum = ((-LChangeNum) % cwDataMultiple)/(cwDataMultiple / 10);

			cwDataMultiple /= 10;

			LOperateStep++;	
			break;	
		}

		case 2:
		{
			if(cwCursor == 0 && LChangeNum < 0){
				//在此位置显示负号
			}else{


			}
			
			LOperateStep++;	
		}

		case 3:		//按键处理
		{
			if(1){	//+键按下
			
				if(LChangeNum > 0) 
					LChangeNum -= cwDataMultiple*cwCursorNum;
				else
					LChangeNum += cwDataMultiple*cwCursorNum;

				cwCursorNum++;

				if(cwDataLen != 0 && cwCursor == 0){	//最高位处理
					
					i =  cwDataMultiple*cwCursorNum;

					if( LChangeNum < 0){
						LChangeNum = -LChangeNum;
						cwCursorNum = 0;
						i = 0;
					}

					if(i > Max){
						if(Min >= 0){
							i = 0;
							cwCursorNum = 0;
						}else{
							i = 0;
							LChangeNum = -LChangeNum;
							cwCursorNum = 0;	
						}
					}
				}else{
					cwCursorNum %= 10;
					i = cwDataMultiple*cwCursorNum;
				}

				if(LChangeNum >= 0)
					LChangeNum += i;
				else
					LChangeNum -= i;

				LOperateStep = 2;	//跳转至第二步进行显示
				
			
			}

			if(1){			//右移按键按下
				cwCursor++;
				if(cwCursor>=cwDataLen)
					cwCursor=0;
				
			}

			break;
		}
	}
}












