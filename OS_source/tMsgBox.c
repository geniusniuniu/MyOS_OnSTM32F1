#include "tMsgBox.h"

#if TINYOS_ENABLE_MSGBOX == 1

void tMsgBoxInit(tMsgBox * msgBox,void ** msgBuffer,uint32_t msgCountMax)
{
	tEventInit(&(msgBox->event),tEventTypeMsgbox); 	
	msgBox->msgBuffer = msgBuffer;
	
	msgBox->msgCount = 0;
	msgBox->msgCountMax = msgCountMax;
	msgBox->read = 0;
	msgBox->write = 0;
	
}

uint32_t tMsgWait(tMsgBox * msgBox,void ** msg,uint32_t WaitTicks)
{
	uint32_t status = tTaskEnterCritical();
		if(msgBox->msgCount > 0)//消息缓冲区有消息
		{
			--msgBox->msgCount;
			*msg = msgBox->msgBuffer[msgBox->read++];	//读取消息
			//指针在数组中循环读取
			if(msgBox->read >= msgBox->msgCountMax)
			{
				msgBox->read = 0;
			}
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		else //没接收到消息、消息全部处理、超时
		{
			tEventWait(&msgBox->event,currTask,(void *)0,tEventTypeMsgbox,WaitTicks);
			tTaskExitCritical(status);
			
			tTaskSchedule();	
			
			*msg = currTask->eventMsg;		
			return currTask->waitEventRes;
		}
}

uint32_t tMsgNoWait(tMsgBox * msgBox,void **msg)
{
	uint32_t status = tTaskEnterCritical();
		if(msgBox->msgCount > 0)//消息缓冲区有消息
		{
			--msgBox->msgCount;
			*msg = msgBox->msgBuffer[msgBox->read++];	//读取消息
			//指针在数组中循环读取
			if(msgBox->read > msgBox->msgCountMax)
			{
				msgBox->read = 0;
			}
			tTaskExitCritical(status);
			return tErrorCodeNone;
		}
		else	//不等待消息，意味着消息无法获取
		{
			tTaskExitCritical(status);
			return tErrorCodeNA;	
		}
}


//notifyPrio用来提高被处理的消息的优先级
uint32_t tMsgNotify(tMsgBox * msgBox,void *msg,uint32_t notifyPrio)
{
	tTask * task;
	uint32_t status = tTaskEnterCritical();
	{
		if(tEventWaitTaskCount(&(msgBox->event)) > 0)	//如果有任务待处理，就先处理任务
		{
			task = tEventWkUp(&(msgBox->event),(void *)0,tErrorCodeNone);
			if(task->priority < currTask->priority)
				tTaskSchedule();
		}
		else
		{
			if(msgBox->msgCount >= msgBox->msgCountMax)
			{
				tTaskExitCritical(status);
				return tErrorCodeFull;
			}
			
			if((notifyPrio & tMSGHIGHPRIO) == 1)
			{
				if(msgBox->read <= 0)
				{
					msgBox->read = msgBox->msgCountMax - 1;
				}
				else
				{
					--msgBox->read;
				}
				msgBox->msgBuffer[msgBox->read] = msg;
			}
			else
			{
				msgBox->msgBuffer[msgBox->write++] = msg;
				if(msgBox->write >= msgBox->msgCountMax)
				{
					msgBox->write = 0;
				}
			}
			
			msgBox->msgCount++;
		}
	}	
	tTaskExitCritical(status);
	return tErrorCodeNone;
}

void tMsgClearUp(tMsgBox * msgBox)
{
	uint32_t status = tTaskEnterCritical();
	{
		if(tEventWaitTaskCount(&msgBox->event) == 0)
		{
			msgBox->read = 0;
			msgBox->write = 0;
			msgBox->msgCount = 0;
		}
	}	
	tTaskExitCritical(status);
}

uint32_t tMsgDelete(tMsgBox * msgBox)
{
	uint32_t count;
	uint32_t status = tTaskEnterCritical();
	{
		count = tEventRemoveAll(&msgBox->event,(void*)0,tErrorCodeDel);
	}	
	tTaskExitCritical(status);
	
	if(count > 0)
	{
		tTaskSchedule();
	}
	return count;	
}

void tMsgGetInfo(tMsgBox * msgBox,tMsgBoxInfo * MsgBoxInfo)
{
	uint32_t status = tTaskEnterCritical();
	{
		MsgBoxInfo->count  = msgBox->msgCount;
		MsgBoxInfo->countMax = msgBox->msgCountMax;
		MsgBoxInfo->taskCount = tEventWaitTaskCount(&msgBox->event);
	}	
	tTaskExitCritical(status);
}


#endif



