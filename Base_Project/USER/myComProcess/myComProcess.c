#include "./myComProcess/myComprocess.h"

void modbusDeal(UsartInformation *pUsart1Inforemation)
{
	unsigned short int nCrc;
	/* 判断通讯机码是否一致 */
	if(pUsart1Inforemation->UsartRxTxData[0] == UserData[14]){
		if(pUsart1Inforemation->UsartRxTxData[1] == 0x03){
			/* 03命令处理 */
			
			nCrc =  pUsart1Inforemation->UsartRxTxData[pUsart1Inforemation->UsartRxCount-1];
			nCrc<<=8;
			nCrc |= pUsart1Inforemation->UsartRxTxData[pUsart1Inforemation->UsartRxCount-2];
			
			if(nCrc == CRC16_MODBUS(pUsart1Inforemation->UsartRxTxData,(pUsart1Inforemation->UsartRxCount-2))){
				
			}
			
		}else if(pUsart1Inforemation->UsartRxTxData[1] == 0x10){
			/* 10命令处理 */
			nCrc =  pUsart1Inforemation->UsartRxTxData[pUsart1Inforemation->UsartRxCount-1];
			nCrc<<=8;
			nCrc |= pUsart1Inforemation->UsartRxTxData[pUsart1Inforemation->UsartRxCount-2];
			if(nCrc == CRC16_MODBUS(pUsart1Inforemation->UsartRxTxData,(pUsart1Inforemation->UsartRxCount-2))){
				
			}
			
		}
	}

}
/* 
 * @brief 读数据处理函数
 * @prame address 读取地址
 * @prame pUsartRcveData 存储指针，将读取到的数据存在此处
 * @prame cwBufAddress 存储地址
 * @prame instruct 指令，如0x10指令，0x03指令
 *
 */
static void readDataDeal(unsigned short address,UsartInformation  *pUsartRcveData,unsigned char *cwBufAddress,unsigned char instruct)
{
	/*
	 *  地址分区说明
	 *  0-600 	  为可设置参数地址 601-999作为预留位置 	R/W
	 *  1000-1999 作为仪表运行状态地址					R
	 *  2000-2600 为可设置参数的浮点数地址				R/W
	 *  3000-3999 作为仪表运行状态浮点数地址				R
	 *  4000-5000 为自定义数据地址						R
	 *  5000-6000 为仪表固定信息地址						R
	 *  6000-6500 为仪表控制命令区域						W
	 */
	unsigned short int cwArryPnter;/* 数组指针 */
	unsigned char  wStype;	/* 读数据类型 */
	int iReaddata;			/* 读数据（待处理）*/
	if(address<=600){
	/* 0-600 	  为可设置参数地址 601-999作为预留位置 	R/W */
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
	/* 1000-1999 作为仪表运行状态地址					R   */
	
	}else if(address>=2000&&address<2600){
	/* 2000-2600 为可设置参数的浮点数地址				R/W */
		
	}else if(address>=3000&&address<4000){
	/* 3000-3999 作为仪表运行状态浮点数地址				R   */
		
	}else if(address>=4000&&address<5000){
	/* 4000-5000 为自定义数据地址						R   */	
		
	}else if(address>=5000&&address<6000){
	/* 5000-6000 为仪表固定信息地址						R   */
		
	}else if(address>=6000&&address<6500){
	/* 6000-6500 为仪表控制命令区域						W   */
		
	}
	
}

void RTU03_Process(UsartInformation  *pUsartRcveData)
{
	unsigned short int nStartAddress,nEndAddress;
	/* 记录指针写入位置 */
	unsigned char  cwBufAddress=3;/* 0-机码 1-命令 */
	
	/* 获取读起始地址 */
	nStartAddress = (pUsartRcveData->UsartRxTxData[2]<<8) + pUsartRcveData->UsartRxTxData[3];
	
	/* 获取读取长度 */
	nEndAddress = (pUsartRcveData->UsartRxTxData[4]<<8) + pUsartRcveData->UsartRxTxData[5];
	
	/* 从起始地址读到该地址停止 */
	nEndAddress+=nStartAddress;
	
	/* 当处理完一个数据nStartAddress自加 */
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





