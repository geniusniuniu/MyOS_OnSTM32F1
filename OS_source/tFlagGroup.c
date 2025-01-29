#include "tFlagGroup.h"


#if TINYOS_ENABLE_FLAGGROUP == 1

void tFlagGroupInit(tFlagGroup * flagGroup,uint32_t flags)
{
	tEventInit(&flagGroup->event,tEventTypeFlagGroup);
	flagGroup->flag = flags;
}

static uint32_t tFlagGroupCheckAndConsume(tFlagGroup * flagGroup,uint32_t type,uint32_t * taskFlag)
{
	uint32_t srcFlag = *taskFlag;			//�����������־λ������
	uint32_t set = type & tFLAGGROUP_SET;	//�ж�type���� ��������λ����Ϊ1������λ��1
	uint32_t isAll = type & tFLAGGROUP_ALL;	//�ж��Ƿ���Ҫ�����¼�λ����������
	uint32_t consume = type & tFLAGGROUP_CONSUME;	//�����ж�ʹ�����¼�λ���Ƿ�������¼�λ
	
	//�������־λ�������ʵ�ʱ�־λ�ȶ�
	uint32_t calFlag = set ? (flagGroup->flag & srcFlag):(~flagGroup->flag & srcFlag);	
	if (((isAll != 0) && (calFlag == srcFlag)) || ((isAll == 0) && (calFlag != 0)))	//���б�־λ�â�����Ҫλλ��Ӧ
	{
		//�ж��Ƿ���Ҫ�����־λ
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
		*taskFlag = calFlag;	//��������洢
		return tErrorCodeNone;
	}
	*taskFlag = calFlag;	//��������洢
	return tErrorCodeNA;
}

uint32_t tFlagGroupWait(tFlagGroup * flagGroup,uint32_t waitType,uint32_t rqFlag,uint32_t * resFlag,uint32_t waitTicks)
{
	uint32_t result;
	uint32_t flags = rqFlag;
	uint32_t status = tTaskEnterCritical();
	
		result = tFlagGroupCheckAndConsume(flagGroup,waitType,&flags);	//�������޸�rqFlagֵ�����Ը���һ�ݳ���		
		if (result != tErrorCodeNone)
		{
			// ����¼���־����������������뵽�ȴ�������
			currTask->waitFlagsType = waitType;
			currTask->eventRqFlags = rqFlag;
			tEventWait(&flagGroup->event, currTask, (void *)0,  tEventTypeFlagGroup, waitTicks);

			tTaskExitCritical(status);

			// ��ִ��һ���¼����ȣ��Ա����л�����������
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
	
	*resFlag = flags;		//�������¼
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
		
		//���������ȴ��б������Ƿ���������Ҫ�ͷ�
		for (node = waitList->headNode.next; node != &(waitList->headNode); node = nextNode) 
		{
			tTask *task = tNodeParent(node, tTask, linkNode);
			uint32_t flags = task->eventRqFlags;
			nextNode = node->next;

			// ����־
			result = tFlagGroupCheckAndConsume(flagGroup, task->waitFlagsType, &flags);
			if (result == tErrorCodeNone) 
			{
				// ��������
				task->eventRqFlags = flags;
				tEventWkUpSpecify(&flagGroup->event, task, (void *)0, tErrorCodeNone);
				schedule = 1;
			}
		} 
	}
	tTaskExitCritical(status);	
	
	// ����������������ִ��һ�ε���
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
