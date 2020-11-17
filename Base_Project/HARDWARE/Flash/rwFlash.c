#include "../HARDWARE/Flash/rwFlash.h"
#include "./runParame/runparame.h"


/*
	����1 ����FLASH
	����2 ҳ����
	����3 д������
	
	����STM32F103CBT6   ���洢��Ϊ128ҳ  �⽫���ݴ洢�ڵ�100ҳ
	0x8000 0000 + 0x18F9C = 0x80018F9C
*/


/**
  * @brief  InternalFlash_Test,���ڲ�FLASH���ж�д����
  * @param  None
  * @retval None
  */
int InternalFlash_Test(void)
{
	uint32_t EraseCounter = 0x00; 	//��¼Ҫ��������ҳ
	uint32_t Address = 0x00;				//��¼д��ĵ�ַ
	uint32_t Data = 0x3210ABCD;			//��¼д�������
	uint32_t NbrOfPage = 0x00;			//��¼д�����ҳ

	FLASH_Status FLASHStatus = FLASH_COMPLETE; //��¼ÿ�β����Ľ��	
	TestStatus MemoryProgramStatus = PASSED;//��¼�������Խ��


	/* ���� */
	FLASH_Unlock();

	/* ����Ҫ��������ҳ */
	NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;

	/* ������б�־λ */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

	/* ��ҳ����*/
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));

	}

	/* ���ڲ�FLASHд������ */
	Address = WRITE_START_ADDR;

	while((Address < WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramWord(Address, Data);//д��һ��������
		Address = Address + 4;
	}

	FLASH_Lock();

	/* ���д��������Ƿ���ȷ */
	Address = WRITE_START_ADDR;

	while((Address < WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
	{
		if((*(__IO uint32_t*) Address) != Data)
		{
			MemoryProgramStatus = FAILED;
		}
		Address += 4;
	}
	return MemoryProgramStatus;
}


int writeToFlash(int *iData,int ilength)
{
	uint32_t EraseCounter = 0x00; 		//��¼Ҫ��������ҳ
	uint32_t Address = 0x00;				//��¼д��ĵ�ַ
	uint32_t NbrOfPage = 0x00;			//��¼д�����ҳ
	unsigned char ii = 0;					//��¼����ָ��
	FLASH_Status FLASHStatus = FLASH_COMPLETE; //��¼ÿ�β����Ľ��	
	TestStatus MemoryProgramStatus = PASSED;//��¼�������Խ��
	
	/* ���� */
	FLASH_Unlock();

	/* ����Ҫ��������ҳ */
	NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;

	/* ������б�־λ */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	/* ��ҳ����*/
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));

	}
	
	/* ���ڲ�FLASHд������ */
	Address = WRITE_START_ADDR;

	while((Address < WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramWord(Address, *(iData+(ii++)));//д��һ��������
		Address = Address + 4;
		
		if(ii>=ilength){
			ii=0;
			break;
		}
	}
	
	FLASH_Lock();	//����
	
	/* ���д��������Ƿ���ȷ */
	Address = WRITE_START_ADDR;

	while((Address < WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
	{
		if((*(__IO uint32_t*) Address) != iData[ii++])//�����ݶ�ȡ����  ��ԭ���ݽ��бȽϣ�����ȷ�򷵻�failed
		{
			MemoryProgramStatus = FAILED;
		}
		Address += 4;
		
		if(ii>=ilength){
			ii=0;
			break;
		}
	}
	return MemoryProgramStatus;
}





unsigned char readFromFlash(int *iData,int ilength)
{
	uint32_t Address = 0x00;				//��¼�����ĵ�ַ
	unsigned char ii=0,j=0;						//��¼����λ��
	unsigned char cwErrorNumber[50];
	
	Address = WRITE_START_ADDR;
	
	for( ii = 0 ; ii < ilength ; ii++)
	{
		iData[ii] = (*(__IO uint32_t*) Address);
		if(ii == 44){
			if(iData[44] != 9004)
				break;
		}
		/*
			�����Ƿ��ڲ����涨��Χ�ڣ����������¼���ݱ�ţ��ָ�Ϊȱʡֵ
		*/
		if( ((iData[ii] >= UserDataDescrip[ii].iLowerLimit) && (iData[ii] <= UserDataDescrip[ii].iUpperLimit)))
		{
			cwErrorNumber[j++] = ii;		//�����쳣   ���¼���ݱ��
			iData[ii] = UserDataDescrip[ii].iInitValue1;		//�ָ�Ϊģʽ1��ʼֵ
			
		}
		
		Address += 4;
		
	}
	
	if(iData[44] != 9004){			/*�жϴ�λ�Ƿ���ȷ ������ȷ���ָ�����*/
		for(ii = 0 ; ii < ilength ; ii++){
			
			iData[ii] = UserDataDescrip[ii].iInitValue1;
		}
		
		writeToFlash(iData,ilength);
	}
	
	
	
	
	/*
		�쳣������ʾ������
	*/
	cwErrorNumber[j]=cwErrorNumber[j];
	
	
	
	if(j == 0)	//��������  ����1
		return 1;
	else		//�����쳣 ����0
		return 0;
}











