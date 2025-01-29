#include "tSemaphore.h"
#include "tEvent.h"

#if TINYOS_ENABLE_SEMAPHORE == 1

void tSemaphoreInit(tSemaphore * sema,uint32_t semaCount,uint32_t semaCountMax)
{
	tEventInit(&(sema->event),tEventTypeSema);
	sema->semaCountMax = semaCountMax;
	
	if(semaCountMax == 0)
	{
		sema->semaCount = semaCount;	//semaCountMax 最大计数，如果为0，则不限数量
	}
	else
	{
		sema->semaCount = (semaCount > semaCountMax) ? semaCountMax : semaCount;
	}
}


//获取信号量有等待时间
uint32_t tSemaphoreWait(tSemaphore * sema,uint32_t WaitTicks)
{
	uint32_t status = tTaskEnterCritical();
		if(sema->semaCount > 0)	//有接收到信号，消耗掉一个信号量
		{
			--sema->semaCount;
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		else	//没接收到信号量，或者信号量清空，进行一次任务调度
		{
			//当任务等待信号量时，将tEventTypeSema枚举值赋给 
			//state 表示该任务是等待一个信号量事件，而不是其他类型的事件
			tEventWait(&(sema->event),currTask,(void *)0,tEventTypeSema,WaitTicks);
			tTaskExitCritical(status);
			tTaskSchedule();
			return currTask->waitEventRes;	//返回等待结果
		}
}

//获取信号量且无等待时间
uint32_t tSemaphoreNoWait(tSemaphore * sema)
{
	uint32_t status = tTaskEnterCritical();
		if(sema->semaCount > 0)	//有接收到信号，消耗掉一个信号量
		{
			--sema->semaCount;
			tTaskExitCritical(status);
			return tErrorCodeNone;	//正常获取
		}
		else
		{
			tTaskExitCritical(status);
			return tErrorCodeNA	;//说明此时资源不可用
		}
}

//释放信号量
void tSemaphoreNotify(tSemaphore * sema)
{
	uint32_t status = tTaskEnterCritical();
		if(tEventWaitTaskCount(&sema->event) > 0)
		{
			tTask * task = tEventWkUp(&(sema->event),(void *)0,tErrorCodeNone);//唤醒首个等待的任务,且他的等待结果没有错误
			if(task->priority  < currTask->priority)	//优先级越高，数字越小
			{
				tTaskSchedule();
			}
		}
		else
		{
			++sema->semaCount;
			if((sema->semaCountMax != 0) && (sema->semaCount > sema->semaCountMax))
			{
				sema->semaCount = sema->semaCountMax;
			}
		}
	tTaskExitCritical(status);
}


void tSemaGetInfo(tSemaphore * sema,tSemaInfo * semaInfo)
{
	uint32_t status = tTaskEnterCritical();
		semaInfo->Count = sema->semaCount;
		semaInfo->CountMax = sema->semaCountMax;
		semaInfo->taskCount = tEventWaitTaskCount(&(sema->event));
	tTaskExitCritical(status);
}

uint32_t tSemaClearUp(tSemaphore * sema)
{
	uint32_t count;
	uint32_t status = tTaskEnterCritical();
	//清除所有等待的任务，并把信号量清零
		count = tEventRemoveAll(&(sema->event),(void *)0,tErrorCodeDel);
		sema->semaCount = 0;
	tTaskExitCritical(status);
	
	if(count > 0)
	{
		tTaskSchedule();
	}
	return count;
}

#endif


