#include "./RunPage/runPage.h"

int This_Dn;
u8 Len_Data;
u8 This_Dnn;
u32 This_Dnx;

const uint32_t MAXNN[]={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};

/*
	У׼���ܽ���
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
	switch(RTData[20])//����˸����
	{
		case 0:{
			if(WT(Tms_glint,300,systemTime.ibastTime)){
				Tms_Num=systemTime.ibastTime;
				Tms_glint=systemTime.ibastTime;
				RTData[20]++;
				DisplayNum(RTData[21],5);//��ʾʵʱֵ
			}
			break;
		}
		case 1:{
			if(WT(Tms_Num,300,systemTime.ibastTime)){
				Tms_glint=systemTime.ibastTime;
				Tms_Num=systemTime.ibastTime;
				DisplayNum(RTData[21],20+This_Dn);//��˸ѡ��λ��
				RTData[20]=0;
			}
			break;
		}
		default:
			RTData[20]=0;
	}
	
	switch(RTData[22])//�������
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
		case 1:{//��ʾ
			m=dot;		
			a=MAXNN[5-This_Dn];
			This_Dnx=a;
			if(RTData[21]>=0)This_Dnn=(RTData[21]%(a*10))/This_Dnx;
			else This_Dnn=((-RTData[21])%a)/This_Dnx;
			
			RTData[22]++;
			break;
		}
		case 2:{
			if(pKey2Information->cwKeyPressStatus == 4){		//+��		
				if(RTData[21]>=0)RTData[21]-=This_Dnn*This_Dnx;
				else RTData[21]+=This_Dnn*This_Dnx;
				This_Dnn++;
				
				
				
				if(This_Dn==0&&Len_Data!=0){ //���λ���⴦��
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
			
			if(pKey1Information->cwKeyPressStatus == 4){//��λ
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
				//������λ���������沢�˳�
				pKey1Information->cwKeyPressStatus = 0;
				
				fa = (float)RTData[21]/RTData[0];

				RTData[49] = UserData[5];
				RTData[49]*=fa;
				
				if(RTData[49]>=100){		//��ֹ�����
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








