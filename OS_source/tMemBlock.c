#include "tMemBlock.h"

#if TINYOS_ENABLE_MEMBLOCK == 1

void tMemBlockInit(tMemBlock * memBlock,uint8_t * memStart,uint32_t blockSize,uint32_t blockCnt)
{
	uint8_t * memBlockStart = (uint8_t*)memStart;
	uint8_t * memBlockEnd   = memBlockStart + blockSize * blockCnt; 
	
	if(blockSize < sizeof(listNode))	//如果需要分配的内存块空间小于节点空间，分配失败
	{
		return ;
	}
	
	tEventInit(&memBlock->event,tEventTypeMemBlock);
	
	memBlock->memStart = memStart;
	memBlock->blockSize = blockSize;
	memBlock->maxCount = blockCnt;
	
	tListInit(&memBlock->blockList);
	
	//依次划分每个存储块的位置
	while(memBlockStart < memBlockEnd)
	{
		//确定每个内存块头节点
		listNodeInit((listNode*)memBlockStart);
		//插入列表
		tListAddLast(&memBlock->blockList,(listNode*)memBlockStart);
		//内存块地址偏移
		memBlockStart += blockSize;
	}	
}

//等待获取存储块
uint32_t tMemBlockWait(tMemBlock * memBlock,uint8_t ** mem,uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();
		if(tListCount(&memBlock->blockList) > 0)
		{
			*mem = (uint8_t*)tListRemoveFirst(&memBlock->blockList);
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		else
		{
			tEventWait(&memBlock->event,currTask,(void*)0,tEventTypeMemBlock,waitTicks);
			tTaskExitCritical(status);
			
			tTaskSchedule();
			*mem = currTask->eventMsg;
			return currTask->waitEventRes;
		}	
}

uint32_t tMemBlockNoWait(tMemBlock * memBlock,uint8_t **mem)
{
	uint32_t status = tTaskEnterCritical();
	if(tListCount(&memBlock->blockList) > 0)
	{
		*mem = (uint8_t*)tListRemoveFirst(&memBlock->blockList);
		tTaskExitCritical(status);
		return tErrorCodeNone;
	}
	else
	{
		tTaskExitCritical(status);
		return tErrorCodeNA;
	}
}


void tMemBlockNotify(tMemBlock * memBlock,uint8_t * mem)
{
	uint32_t status = tTaskEnterCritical();
		if(tEventWaitTaskCount(&memBlock->event) > 0)
		{
			tTask * task = tEventWkUp(&memBlock->event,(void*)mem,tErrorCodeNone);
			if(currTask->priority > task->priority)
			{
				tTaskSchedule();
			}
		}
		else
		{
			tListAddLast(&memBlock->blockList,(listNode*)mem);
		}
	tTaskExitCritical(status);
}


void tMemBlockGetInfo(tMemBlock * memBlock,tMemBlockInfo * info)
{
	uint32_t status = tTaskEnterCritical();
	{
		info->blockSize = memBlock->blockSize;
		info->maxCount = memBlock->maxCount;
		info->count = tListCount(&memBlock->blockList);
		info->taksCount = tEventWaitTaskCount(&memBlock->event);
	}
	tTaskExitCritical(status);
}


uint32_t tMemBlockDelete(tMemBlock * memBlock)
{
	uint32_t count;
	uint32_t status = tTaskEnterCritical();
	{
		count = tEventRemoveAll(&memBlock->event,(void*)0,tErrorCodeDel);	
	}
	tTaskExitCritical(status);	
	
	if(count > 0)
	{
		tTaskSchedule();
	}
	return count;
	
}


#endif
