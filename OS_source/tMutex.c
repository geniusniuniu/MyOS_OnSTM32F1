#include "tMutex.h"

#if TINYOS_ENABLE_MUTEX == 1

void tMutexInit(tMutex * mutex)
{
	tEventInit(&mutex->event,tEventTypeMutex);
	mutex->lockCount = 0;
	mutex->owner = (tTask*)0;
	mutex->ownerOriginalPrio = TINYOS_PIORITY_MAX;
	
}

uint32_t tMutexWait(tMutex * mutex,uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();
	//情况1：初次上锁
		if(mutex->lockCount <= 0)
		{
			mutex->owner = currTask;
			mutex->ownerOriginalPrio = currTask->priority;
			mutex->lockCount++;
			
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		else	//情况2：已上过锁
		{
				//情况2.1：对同一任务上锁
			if(mutex->owner ==  currTask)
			{
				mutex->lockCount++;
				
				tTaskExitCritical(status);
				return tErrorCodeNone;
			}
			else//情况2.2：对不同任务上锁
			{
				//情况2.2.1：对更高优先级任务上锁，低优先级任务继承高优先级任务优先级别
				if(currTask->priority < mutex->owner->priority)
				{
					//将低优先级任务放到高优先级就绪队列中
					if(mutex->owner->State == TINYOS_TASK_STA_READY)
					{
						//从低优先级就绪队列移除
						tTaskRFReadyList(mutex->owner);
						mutex->owner->priority = currTask->priority;
						//放到高优先级就绪队列中
						tTaskSetReady(mutex->owner);
					}
					else //如果任务没处于就绪态，只需更改该任务优先级
					{
						mutex->owner->priority = currTask->priority;
					}
				}
				//情况2.2.2：无论对于其他更高优先级还是较低优先级的任务，都进入等待状态
				tEventWait(&mutex->event,currTask,(void *)0,tEventTypeMutex,waitTicks);
				
				tTaskExitCritical(status);				
				tTaskSchedule();
				return currTask->waitEventRes;
			}
		}
}


uint32_t tMutexNoWait(tMutex * mutex)
{
	uint32_t status = tTaskEnterCritical();
	
		if(mutex->lockCount <= 0)
		{
			mutex->owner = currTask;
			mutex->ownerOriginalPrio = currTask->priority;
			mutex->lockCount++;
			
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		else
		{
			if(mutex->owner ==  currTask)
			{
				mutex->lockCount++;
				
				tTaskExitCritical(status);
				return tErrorCodeNone;
			}
			tTaskExitCritical(status);
			return tErrorCodeNA;
		}
}

uint32_t tMutexNotify(tMutex * mutex)
{
	uint32_t status = tTaskEnterCritical();
	{
		if(mutex->lockCount <= 0)
		{
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		
		//限制只有持有锁的任务能对锁进行操作
		if(mutex->owner != currTask)
		{
			tTaskExitCritical(status);
			return tErrorCodeOwner;
		}
		
		// 减1后计数仍不为0, 直接退出，不唤醒等待的任务
		if(--mutex->lockCount > 0)
		{
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		
		//发生了优先级的继承,将owner改回原有优先级
		if(mutex->ownerOriginalPrio != mutex->owner->priority)
		{
			if(mutex->owner->State == TINYOS_TASK_STA_READY)
			{
				tTaskRFReadyList(mutex->owner);
				currTask->priority = mutex->ownerOriginalPrio;
				tTaskSetReady(mutex->owner);
			}
			else
			{
				currTask->priority = mutex->ownerOriginalPrio;
			}
		}
		
		//只要等待队列中有任务，那么就取出一个任务，并占有该信号量（owner）
		if(tEventWaitTaskCount(&mutex->event) > 0)
		{
			tTask * task = tEventWkUp(&mutex->event,(void *)0,tErrorCodeNone);
			
			mutex->owner = task;
			mutex->ownerOriginalPrio = task->priority;
			mutex->lockCount++;
			
			if(task->priority < currTask->priority)
			{
				tTaskSchedule();
			}
		}
		tTaskExitCritical(status);
		return tErrorCodeNone;
	}
}

uint32_t tMutexDelete(tMutex * mutex)
{
	uint32_t count;
	uint32_t status = tTaskEnterCritical();
	{
		if(mutex->lockCount > 0)
		{
			if(mutex->ownerOriginalPrio != mutex->owner->priority)	//发生优先级的反转
			{
				if(mutex->owner->State == TINYOS_TASK_STA_READY)
				{
					tTaskRFReadyList(mutex->owner);
					mutex->owner->priority = mutex->ownerOriginalPrio;
					tTaskSetReady(mutex->owner);
				}
				else
				{
					mutex->owner->priority = mutex->ownerOriginalPrio;
				}			
			}
		}
		count = tEventRemoveAll(&mutex->event,(void*)0,tErrorCodeDel);
	}
	if(count > 0)
	{
		tTaskSchedule();
	}
	tTaskExitCritical(status);
	return count;
}

void tMutexGetInfo(tMutex * mutex,tMutexInfo * info)
{
	uint32_t status = tTaskEnterCritical();
	{
		info->owner = mutex->owner;
		info->lockCount = mutex->lockCount;
		info->ownerOriginalPrio = mutex->ownerOriginalPrio;
		info->taskCount = tEventWaitTaskCount(&mutex->event);	
		if(mutex->owner != (tTask*)0)
		{
			info->inheritedPrio = mutex->owner->priority;
		}
		else
		{
			info->inheritedPrio = TINYOS_PIORITY_MAX;
		}
	}
	tTaskExitCritical(status);
}


#endif
