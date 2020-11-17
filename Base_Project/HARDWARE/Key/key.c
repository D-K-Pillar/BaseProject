#include "../HARDWARE/Key/key.h"




KeyInformation Key1Information={0,0,1};	//����1��ʼ��
KeyInformation Key2Information={0,0,2};	//����2��ʼ��

KeyInformation *pKey1Information = &Key1Information ;
KeyInformation *pKey2Information = &Key2Information ;


/*
	���� �ж���һ������������
	����
	����ֵ:��Ӧ�İ���λ�����簴��1��2ͬʱ������ �򷵻�0x03  0000 0011

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

void keyStaticDetect(KeyInformation *pKeyInformation)	//�������
{
	u16 KeyStatus;
	KeyStatus = whichKeyInput();
	
	if(((KeyStatus&(pKeyInformation->cwKeyNum)) == pKeyInformation->cwKeyNum)||(pKeyInformation->cwKeyPressStatus!=0)){
		//�������»򰴼��Ѿ�����
		switch(pKeyInformation->cwKeyPressStatus)
		{
			/* δ����,����ú���*/
			case 0:
				pKeyInformation->hwKeyPressTime = systemTime.ibastTime;	//������ʱ������
				pKeyInformation->cwKeyPressStatus = 1;  //��ת������״̬
			break;
			
			/* ������ */
			case 1:
				if((systemTime.ibastTime-pKeyInformation->hwKeyPressTime)>20){	//20ms����ʱ��
					if((KeyStatus&(pKeyInformation->cwKeyNum)) == pKeyInformation->cwKeyNum){//�������£�������ɣ��ж�Ϊ�̰�
						pKeyInformation->cwKeyPressStatus = 2;  //��ת���̰�״̬
					}else{
						pKeyInformation->cwKeyPressStatus = 0;	//����� ��ת��δ����״̬
					}
				}
			break;

			/* �̰���  ��һ��ʱ�����ж϶̰����� ��Ϊ����*/
			case 2:
				if(((KeyStatus&(pKeyInformation->cwKeyNum)) == pKeyInformation->cwKeyNum)){
					if((systemTime.ibastTime-pKeyInformation->hwKeyPressTime)>1000){	//��������
						pKeyInformation->cwKeyPressStatus = 3;	//����״̬δ����
					}
					
				}else{	//���ж�Ϊ�̰�����
					pKeyInformation->cwKeyPressStatus = 4;
				}
			break;

			/* ������ */
			case 3:
				if(((KeyStatus&(pKeyInformation->cwKeyNum)) != pKeyInformation->cwKeyNum)){
					pKeyInformation->cwKeyPressStatus = 5;	//�ж�Ϊ��������
				}
			break;

			/* �̰����� */
			case 4:
				
			break;

			/* �������� */
			case 5:
				
			break;			
			
			default:
				pKeyInformation->cwKeyPressStatus = 0;//�����쳣 ת��Ϊδ����״̬
			break;
		}
		
	}else{

	}
}


void KeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU  ;   //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}









