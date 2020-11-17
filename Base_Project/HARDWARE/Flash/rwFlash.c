#include "../HARDWARE/Flash/rwFlash.h"
#include "./runParame/runparame.h"


/*
	步骤1 解锁FLASH
	步骤2 页擦除
	步骤3 写入数据
	
	采用STM32F103CBT6   主存储器为128页  拟将数据存储于第100页
	0x8000 0000 + 0x18F9C = 0x80018F9C
*/


/**
  * @brief  InternalFlash_Test,对内部FLASH进行读写测试
  * @param  None
  * @retval None
  */
int InternalFlash_Test(void)
{
	uint32_t EraseCounter = 0x00; 	//记录要擦除多少页
	uint32_t Address = 0x00;				//记录写入的地址
	uint32_t Data = 0x3210ABCD;			//记录写入的数据
	uint32_t NbrOfPage = 0x00;			//记录写入多少页

	FLASH_Status FLASHStatus = FLASH_COMPLETE; //记录每次擦除的结果	
	TestStatus MemoryProgramStatus = PASSED;//记录整个测试结果


	/* 解锁 */
	FLASH_Unlock();

	/* 计算要擦除多少页 */
	NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;

	/* 清空所有标志位 */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

	/* 按页擦除*/
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));

	}

	/* 向内部FLASH写入数据 */
	Address = WRITE_START_ADDR;

	while((Address < WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramWord(Address, Data);//写入一个整型数
		Address = Address + 4;
	}

	FLASH_Lock();

	/* 检查写入的数据是否正确 */
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
	uint32_t EraseCounter = 0x00; 		//记录要擦除多少页
	uint32_t Address = 0x00;				//记录写入的地址
	uint32_t NbrOfPage = 0x00;			//记录写入多少页
	unsigned char ii = 0;					//记录数组指针
	FLASH_Status FLASHStatus = FLASH_COMPLETE; //记录每次擦除的结果	
	TestStatus MemoryProgramStatus = PASSED;//记录整个测试结果
	
	/* 解锁 */
	FLASH_Unlock();

	/* 计算要擦除多少页 */
	NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;

	/* 清空所有标志位 */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	/* 按页擦除*/
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));

	}
	
	/* 向内部FLASH写入数据 */
	Address = WRITE_START_ADDR;

	while((Address < WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramWord(Address, *(iData+(ii++)));//写入一个整型数
		Address = Address + 4;
		
		if(ii>=ilength){
			ii=0;
			break;
		}
	}
	
	FLASH_Lock();	//上锁
	
	/* 检查写入的数据是否正确 */
	Address = WRITE_START_ADDR;

	while((Address < WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
	{
		if((*(__IO uint32_t*) Address) != iData[ii++])//将数据读取出来  与原数据进行比较，不正确则返回failed
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
	uint32_t Address = 0x00;				//记录读数的地址
	unsigned char ii=0,j=0;						//记录数组位置
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
			数据是否在参数规定范围内？若不是则记录数据编号，恢复为缺省值
		*/
		if( ((iData[ii] >= UserDataDescrip[ii].iLowerLimit) && (iData[ii] <= UserDataDescrip[ii].iUpperLimit)))
		{
			cwErrorNumber[j++] = ii;		//数据异常   这记录数据编号
			iData[ii] = UserDataDescrip[ii].iInitValue1;		//恢复为模式1初始值
			
		}
		
		Address += 4;
		
	}
	
	if(iData[44] != 9004){			/*判断此位是否正确 若不正确，恢复出厂*/
		for(ii = 0 ; ii < ilength ; ii++){
			
			iData[ii] = UserDataDescrip[ii].iInitValue1;
		}
		
		writeToFlash(iData,ilength);
	}
	
	
	
	
	/*
		异常数据显示？处理？
	*/
	cwErrorNumber[j]=cwErrorNumber[j];
	
	
	
	if(j == 0)	//数据正常  返回1
		return 1;
	else		//数据异常 返回0
		return 0;
}











