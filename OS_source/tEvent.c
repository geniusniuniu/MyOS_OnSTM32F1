#include "tinyOS.h"

void tEventInit(tEvent * event,tEventType type)
{
	event->type = tEventTypeUnknown;
	tListInit(&(event->waitingList));
}

//事件的等待,超时处理
void tEventWait(tEvent * event,tTask * task,void * msg,uint32_t state,uint32_t timeOut)
{
	uint32_t status = tTaskEnterCritical();
	{
		//1.保存事件相关的任务信息
		task->State |= state;			//state高16位的事件状态位
		task->waitEvent = event;		//任务存放到哪个事件中		
		task->eventMsg = msg;			//设置任务等待事件的消息存储位置，需要消息接受区
		task->waitEventRes = tErrorCodeTimeOut;	//事件的结果
		
		//2.将任务从就绪态移除
		tTaskRFReadyList(task);
		
		//3.将任务排队添加到事件列表中
		tListAddLast(&(event->waitingList),&(task->linkNode));
		//4.判断事件是否有超时时间如果发现有设置超时，在同时插入到延时队列中
		if(timeOut != 0)
		{
			//如果没有超时限制，当前任务将会被阻塞，直到事件发生
			tTaskADTDelayList(task,timeOut);	
		}
	}
	tTaskExitCritical(status);
}

//从事件控制块中唤醒首个等待的任务
tTask * tEventWkUp(tEvent * event,void * msg,uint32_t result)
{
	listNode * node;
	tTask * task;
	uint32_t status = tTaskEnterCritical();
	{
		//1.取出事件列表中第一个结点
		node = tListRemoveFirst(&(event->waitingList));
		//2.如果取到节点不为空找到其父系任务结构体地址 
		if(node != (listNode *)0)
		{
			task = tNodeParent(node,tTask,linkNode);
			
			//3.设置收到的消息、结构，清除相应的等待标志位
			task->waitEvent = (tEvent *)0;
			task->eventMsg = msg;
			task->waitEventRes = result;
			task->State &= ~TINYOS_EVENT_STA_MASK;
			
			//4.如果任务申请了超时等待，将其从延时队列中移除
			if (task->taskDelayTicks != 0)
			{ 
				tTaskRFDelayList(task);
			}

			// 将任务加入就绪队列
			tTaskSetReady(task);    
		}
			
	}
	tTaskExitCritical(status);
	return task;
}

//从事件控制块中唤醒指定任务
void tEventWkUpSpecify(tEvent * event,tTask * task,void * msg,uint32_t result)
{
	uint32_t status = tTaskEnterCritical();
	{
		
		tListRemove(&event->waitingList,&task->linkNode);
		task->waitEvent = (tEvent *)0;
		task->eventMsg = msg;
		task->waitEventRes = result;
		task->State &= ~TINYOS_EVENT_STA_MASK;
		
		//如果任务申请了超时等待，将其从延时队列中移除
		if (task->taskDelayTicks != 0)
		{ 
			tTaskRFDelayList(task);
		}

		// 将任务加入就绪队列
		tTaskSetReady(task);    
	}			
	tTaskExitCritical(status);
}

//将任务从其事件等待队列中强制移除
void tEventRemoveTask(tTask * task, void * msg, uint32_t result)
{
	uint32_t status = tTaskEnterCritical();
	{
		tListRemove(&(task->waitEvent->waitingList), &(task->linkNode));	
		
	    //设置收到的消息、结构，清除相应的等待标志位
		task->waitEvent = (tEvent *)0;
		task->eventMsg = msg;
		task->waitEventRes = result;
		task->State &= ~TINYOS_EVENT_STA_MASK;
	}
	tTaskExitCritical(status);
}

//事件控制块的清空
uint32_t tEventRemoveAll(tEvent * event, void * msg, uint32_t result)
{
	uint32_t taskCount;
	listNode * node;
	tTask * task;
	uint32_t status = tTaskEnterCritical();
	{
		//第一步查询事件中等待任务数量
		taskCount = tListCount(&(event->waitingList));
		//第二步开始遍历移除事件中的等待任务
		while((node = tListRemoveFirst(&(event->waitingList))) != (listNode *)0)
		{
			task = tNodeParent(node,tTask,linkNode);
			
			task->waitEvent = (tEvent *)0;
			task->eventMsg = msg;
			task->waitEventRes = result;
			task->State &= ~TINYOS_EVENT_STA_MASK;
			
			if(task->taskDelayTicks != 0)
			{
				tTaskRFDelayList(task);
			}
			tTaskSetReady(task);
		}
		
	}
	tTaskExitCritical(status);
	
	return taskCount;
}

uint32_t tEventWaitTaskCount(tEvent * event)
{
	uint32_t taskCount;
	uint32_t status = tTaskEnterCritical();
	{
		taskCount = tListCount(&(event->waitingList));
	}
	tTaskExitCritical(status);
	return taskCount;
}
