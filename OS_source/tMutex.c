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
	//���1����������
		if(mutex->lockCount <= 0)
		{
			mutex->owner = currTask;
			mutex->ownerOriginalPrio = currTask->priority;
			mutex->lockCount++;
			
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		else	//���2�����Ϲ���
		{
				//���2.1����ͬһ��������
			if(mutex->owner ==  currTask)
			{
				mutex->lockCount++;
				
				tTaskExitCritical(status);
				return tErrorCodeNone;
			}
			else//���2.2���Բ�ͬ��������
			{
				//���2.2.1���Ը������ȼ����������������ȼ�����̳и����ȼ��������ȼ���
				if(currTask->priority < mutex->owner->priority)
				{
					//�������ȼ�����ŵ������ȼ�����������
					if(mutex->owner->State == TINYOS_TASK_STA_READY)
					{
						//�ӵ����ȼ����������Ƴ�
						tTaskRFReadyList(mutex->owner);
						mutex->owner->priority = currTask->priority;
						//�ŵ������ȼ�����������
						tTaskSetReady(mutex->owner);
					}
					else //�������û���ھ���̬��ֻ����ĸ��������ȼ�
					{
						mutex->owner->priority = currTask->priority;
					}
				}
				//���2.2.2�����۶��������������ȼ����ǽϵ����ȼ������񣬶�����ȴ�״̬
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
		
		//����ֻ�г������������ܶ������в���
		if(mutex->owner != currTask)
		{
			tTaskExitCritical(status);
			return tErrorCodeOwner;
		}
		
		// ��1������Բ�Ϊ0, ֱ���˳��������ѵȴ�������
		if(--mutex->lockCount > 0)
		{
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		
		//���������ȼ��ļ̳�,��owner�Ļ�ԭ�����ȼ�
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
		
		//ֻҪ�ȴ���������������ô��ȡ��һ�����񣬲�ռ�и��ź�����owner��
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
			if(mutex->ownerOriginalPrio != mutex->owner->priority)	//�������ȼ��ķ�ת
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
