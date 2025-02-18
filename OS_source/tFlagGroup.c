#include "tFlagGroup.h"


#if TINYOS_ENABLE_FLAGGROUP == 1

void tFlagGroupInit(tFlagGroup * flagGroup,uint32_t flags)
{
	tEventInit(&flagGroup->event,tEventTypeFlagGroup);
	flagGroup->flag = flags;
}

static uint32_t tFlagGroupCheckAndConsume(tFlagGroup * flagGroup,uint32_t type,uint32_t * taskFlag)
{
	uint32_t srcFlag = *taskFlag;			//将任务需求标志位做保留
	uint32_t set = type & tFLAGGROUP_SET;	//判断type类型 包括任意位置置为1和所有位置1
	uint32_t isAll = type & tFLAGGROUP_ALL;	//判断是否需要所有事件位都满足条件
	uint32_t consume = type & tFLAGGROUP_CONSUME;	//用来判断使用完事件位后是否清除该事件位
	
	//将需求标志位和任务的实际标志位比对
	uint32_t calFlag = set ? (flagGroup->flag & srcFlag):(~flagGroup->flag & srcFlag);	
	if (((isAll != 0) && (calFlag == srcFlag)) || ((isAll == 0) && (calFlag != 0)))	//所有标志位置①则需要位位对应
	{
		//判断是否需要清除标志位
		if(consume)
		{
			if(set)
			{
				flagGroup->flag &= ~srcFlag;
			}
			else
			{
				flagGroup->flag |= srcFlag;
			}
		}
		*taskFlag = calFlag;	//将检查结果存储
		return tErrorCodeNone;
	}
	*taskFlag = calFlag;	//将检查结果存储
	return tErrorCodeNA;
}

uint32_t tFlagGroupWait(tFlagGroup * flagGroup,uint32_t waitType,uint32_t rqFlag,uint32_t * resFlag,uint32_t waitTicks)
{
	uint32_t result;
	uint32_t flags = rqFlag;
	uint32_t status = tTaskEnterCritical();
	
		result = tFlagGroupCheckAndConsume(flagGroup,waitType,&flags);	//函数会修改rqFlag值，所以复制一份出来		
		if (result != tErrorCodeNone)
		{
			// 如果事件标志不满足条件，则插入到等待队列中
			currTask->waitFlagsType = waitType;
			currTask->eventRqFlags = rqFlag;
			tEventWait(&flagGroup->event, currTask, (void *)0,  tEventTypeFlagGroup, waitTicks);

			tTaskExitCritical(status);

			// 再执行一次事件调度，以便于切换到其它任务
			tTaskSchedule();

			*resFlag = currTask->eventRqFlags;
			result = currTask->waitEventRes;
		}
		else
		{
			*resFlag = flags;
			tTaskExitCritical(status);
		}
	return result;	
}

uint32_t tFlagGroupNoWait(tFlagGroup * flagGroup,uint32_t waitType,uint32_t rqFlag,uint32_t * resFlag)
{
	uint32_t result;
	uint32_t flags = rqFlag;
	uint32_t status = tTaskEnterCritical();
	{
		result = tFlagGroupCheckAndConsume(flagGroup,waitType,&flags);	
	}
	tTaskExitCritical(status);
	
	*resFlag = flags;		//将结果记录
	return result;
}


void tFlagGroupNotify(tFlagGroup * flagGroup,uint32_t isSet,uint32_t flag)
{
	uint32_t result;
	uint8_t schedule = 0;
	tList * waitList;
	listNode* node;
	listNode* nextNode;
	uint32_t status = tTaskEnterCritical();
	{
		if(isSet)
		{
			flagGroup->flag |= flag; 
		}
		else
		{
			flagGroup->flag &= ~flag;
		}
		
		waitList = &flagGroup->event.waitingList;
		
		//遍历整个等待列表，查找是否有任务需要释放
		for (node = waitList->headNode.next; node != &(waitList->headNode); node = nextNode) 
		{
			tTask *task = tNodeParent(node, tTask, linkNode);
			uint32_t flags = task->eventRqFlags;
			nextNode = node->next;

			// 检查标志
			result = tFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
			if (result == tErrorCodeNone) 
			{
				// 唤醒任务
				task->eventRqFlags = flags;
				tEventWkUpSpecify(&flagGroup->event, task, (void *)0, tErrorCodeNone);
				schedule = 1;
			}
		} 
	}
	tTaskExitCritical(status);	
	
	// 如果有任务就绪，则执行一次调度
    if (schedule == 1)
    {
        tTaskSchedule();
    }
}

uint32_t tFlagGroupDelete(tFlagGroup * flagGroup)
{
	uint32_t count;
	uint32_t status = tTaskEnterCritical();
	{
		count = tEventRemoveAll(&flagGroup->event,(void *)0,tErrorCodeDel);
	}		
	
	if(count > 0)
	{
		tTaskSchedule();
	}
	tTaskExitCritical(status);	
	return count;
}

void tFlagGroupGetInfo(tFlagGroup * flagGroup,tFlagGroupInfo * info)
{
	uint32_t status = tTaskEnterCritical();
	{
		info->taskCount = tEventWaitTaskCount(&flagGroup->event);
		info->currFlag = flagGroup->flag;
	}
	tTaskExitCritical(status);	
}



#endif
