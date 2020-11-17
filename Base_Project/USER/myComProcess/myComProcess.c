#include "./myComProcess/myComprocess.h"

void modbusDeal(UsartInformation *pUsart1Inforemation)
{
	unsigned short int nCrc;
	/* �ж�ͨѶ�����Ƿ�һ�� */
	if(pUsart1Inforemation->UsartRxTxData[0] == UserData[14]){
		if(pUsart1Inforemation->UsartRxTxData[1] == 0x03){
			/* 03����� */
			
			nCrc =  pUsart1Inforemation->UsartRxTxData[pUsart1Inforemation->UsartRxCount-1];
			nCrc<<=8;
			nCrc |= pUsart1Inforemation->UsartRxTxData[pUsart1Inforemation->UsartRxCount-2];
			
			if(nCrc == CRC16_MODBUS(pUsart1Inforemation->UsartRxTxData,(pUsart1Inforemation->UsartRxCount-2))){
				
			}
			
		}else if(pUsart1Inforemation->UsartRxTxData[1] == 0x10){
			/* 10����� */
			nCrc =  pUsart1Inforemation->UsartRxTxData[pUsart1Inforemation->UsartRxCount-1];
			nCrc<<=8;
			nCrc |= pUsart1Inforemation->UsartRxTxData[pUsart1Inforemation->UsartRxCount-2];
			if(nCrc == CRC16_MODBUS(pUsart1Inforemation->UsartRxTxData,(pUsart1Inforemation->UsartRxCount-2))){
				
			}
			
		}
	}

}
/* 
 * @brief �����ݴ�����
 * @prame address ��ȡ��ַ
 * @prame pUsartRcveData �洢ָ�룬����ȡ�������ݴ��ڴ˴�
 * @prame cwBufAddress �洢��ַ
 * @prame instruct ָ���0x10ָ�0x03ָ��
 *
 */
static void readDataDeal(unsigned short address,UsartInformation  *pUsartRcveData,unsigned char *cwBufAddress,unsigned char instruct)
{
	/*
	 *  ��ַ����˵��
	 *  0-600 	  Ϊ�����ò�����ַ 601-999��ΪԤ��λ�� 	R/W
	 *  1000-1999 ��Ϊ�Ǳ�����״̬��ַ					R
	 *  2000-2600 Ϊ�����ò����ĸ�������ַ				R/W
	 *  3000-3999 ��Ϊ�Ǳ�����״̬��������ַ				R
	 *  4000-5000 Ϊ�Զ������ݵ�ַ						R
	 *  5000-6000 Ϊ�Ǳ�̶���Ϣ��ַ						R
	 *  6000-6500 Ϊ�Ǳ������������						W
	 */
	unsigned short int cwArryPnter;/* ����ָ�� */
	unsigned char  wStype;	/* ���������� */
	int iReaddata;			/* �����ݣ�������*/
	if(address<=600){
	/* 0-600 	  Ϊ�����ò�����ַ 601-999��ΪԤ��λ�� 	R/W */
		cwArryPnter = address/2;
		
		if(instruct&0x03){
			iReaddata = UserData[cwArryPnter];
			if(UserDataDescrip[cwArryPnter].iLowerLimit<0)
				wStype = 2;
			else 
				wStype = 3;
			
		}else if(instruct&0x10){
		
		}
	}else if(address>=1000&&address<2000){
	/* 1000-1999 ��Ϊ�Ǳ�����״̬��ַ					R   */
	
	}else if(address>=2000&&address<2600){
	/* 2000-2600 Ϊ�����ò����ĸ�������ַ				R/W */
		
	}else if(address>=3000&&address<4000){
	/* 3000-3999 ��Ϊ�Ǳ�����״̬��������ַ				R   */
		
	}else if(address>=4000&&address<5000){
	/* 4000-5000 Ϊ�Զ������ݵ�ַ						R   */	
		
	}else if(address>=5000&&address<6000){
	/* 5000-6000 Ϊ�Ǳ�̶���Ϣ��ַ						R   */
		
	}else if(address>=6000&&address<6500){
	/* 6000-6500 Ϊ�Ǳ������������						W   */
		
	}
	
}

void RTU03_Process(UsartInformation  *pUsartRcveData)
{
	unsigned short int nStartAddress,nEndAddress;
	/* ��¼ָ��д��λ�� */
	unsigned char  cwBufAddress=3;/* 0-���� 1-���� */
	
	/* ��ȡ����ʼ��ַ */
	nStartAddress = (pUsartRcveData->UsartRxTxData[2]<<8) + pUsartRcveData->UsartRxTxData[3];
	
	/* ��ȡ��ȡ���� */
	nEndAddress = (pUsartRcveData->UsartRxTxData[4]<<8) + pUsartRcveData->UsartRxTxData[5];
	
	/* ����ʼ��ַ�����õ�ַֹͣ */
	nEndAddress+=nStartAddress;
	
	/* ��������һ������nStartAddress�Լ� */
	while(nStartAddress<nEndAddress)
	{
		readDataDeal(nStartAddress,pUsartRcveData,&cwBufAddress,pUsartRcveData->UsartRxTxData[1]);
	}
}
















void InvertUint8(unsigned char *dBuf,unsigned char *srcBuf)  
{  
    int i;  
    unsigned char tmp[4];  
    tmp[0] = 0;  
    for(i=0;i< 8;i++)  
    {  
      if(srcBuf[0]& (1 << i))  
        tmp[0]|=1<<(7-i);  
    }  
    dBuf[0] = tmp[0];  
      
}  

void InvertUint16(unsigned short *dBuf,unsigned short *srcBuf)  
{  
    int i;  
    unsigned short tmp[4];  
    tmp[0] = 0;  
    for(i=0;i< 16;i++)  
    {  
      if(srcBuf[0]& (1 << i))  
        tmp[0]|=1<<(15 - i);  
    }  
    dBuf[0] = tmp[0];  
}  

unsigned short CRC16_MODBUS(unsigned char *puchMsg, unsigned int usDataLen)  
{  
	unsigned short wCRCin = 0xFFFF;  
	unsigned short wCPoly = 0x8005;  
	unsigned char  wChar = 0;  
	unsigned char  i =0;
	while (usDataLen--)     
	{  
		wChar = *(puchMsg++);  
		InvertUint8(&wChar,&wChar);  
		wCRCin ^= (wChar << 8);  
		for( i = 0;i < 8;i++)  
		{  
			if(wCRCin & 0x8000)  
				wCRCin = (wCRCin << 1) ^ wCPoly;  
			else  
				wCRCin = wCRCin << 1;  
		}  
	}  
	InvertUint16(&wCRCin,&wCRCin);  
	return (wCRCin) ;  
} 





