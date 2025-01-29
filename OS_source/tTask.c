#include "tinyOS.h"

//创建任务数量表
tList TaskTable[TINYOS_PIORITY_MAX];
//创建任务延时列表
tList taskDelayList;

//创建任务句柄
tTask* currTask;
tTask* nextTask;
tTask* idleTask;

uint8_t schedLockCount = 0;


//任务初始化函数
void tTaskInit(tTask* task ,void(*entry)(void *),void * param,uint32_t priority,tTaskStack * stackBottom,uint32_t size)
{
	tTaskStack * stackTop;
	
	task->stackBase = stackBottom;	//小端增长
	task->stackSize = size;
	memset(stackBottom,0,size);
	stackTop = stackBottom + size / sizeof(tTaskStack);
	
	*(--stackTop) = (unsigned long)(1<<24);	//xPSR寄存器置为1，从进程堆栈中做出栈操作，返回使用PSP
	*(--stackTop) = (unsigned long)entry;		//PC寄存器：设置为任务入口地址
	*(--stackTop) = (unsigned long)0x14;		//LR寄存器：设置为特定值
	*(--stackTop) = (unsigned long)0x12;		//R12寄存器
	*(--stackTop) = (unsigned long)0x03;		//R3寄存器
	*(--stackTop) = (unsigned long)0x02;		//R2寄存器
	*(--stackTop) = (unsigned long)0x01;		//R1寄存器
	*(--stackTop) = (unsigned long)param;		//R0寄存器：传入参数

	*(--stackTop) = (unsigned long)0x11;
	*(--stackTop) = (unsigned long)0x10;
	*(--stackTop) = (unsigned long)0x09;
	*(--stackTop) = (unsigned long)0x08;
	*(--stackTop) = (unsigned long)0x07;
	*(--stackTop) = (unsigned long)0x06;
	*(--stackTop) = (unsigned long)0x05;
	*(--stackTop) = (unsigned long)0x04;

	task->stack = stackTop;	        //设置任务的堆栈起始地址
	task->taskDelayTicks = 0;
	task->priority = priority;
	task->slice = TINYOS_SLICE_MAX; //100ms
	//任务创建时是就绪状态
	task->State |= TINYOS_TASK_STA_READY;
	task->suspendCount = 0;
	
	task->clean = (void (*)(void *))0;	//将0强制转换为一个函数指针表示clean指向NULL
	task->cleanParam = (void *)0;
	task->tTaskDelRequest = TINYOS_TASK_NODELRQST;
	
	task->waitEvent = (tEvent *)0;                      // 没有等待事件
    task->eventMsg = (void *)0;                         // 没有等待事件
    task->waitEventRes = tErrorCodeNone;               // 没有等待事件错误
	
	listNodeInit(&task->linkNode);
	//初始化延时节点单元
	listNodeInit(&task->delayNode);
	//将任务加入就序列表
	//如果使用 tListAddFirst 方法会将新任务插入到链表的开头，
	//意味着新加入的任务会优先于已存在任务进行调度。
	//tListAddLast(&TaskTable[priority],&(task->linkNode));
	//tBitmapSet(&taskPrioBM,priority);
	//和上述两行代码等效
	tTaskSetReady(task);

#if TINYOS_ENABLE_HOOKS == 1
	tHooksTaskInit(task);
#endif
}

tTask * tTaskGetReady(void)
{
	//查找优先级最高的任务列表
	uint32_t HighestPrio = tBitmapGetFirstSet(&taskPrioBM);
	//找到该任务列表的第一个任务的linkNode
	listNode * firstNode = tListFirst(&TaskTable[HighestPrio]);
	//根据linkNode找到任务入口地址
	return tNodeParent(firstNode,tTask,linkNode);
}

void tTaskSchedInit(void)	//任务调度初始化函数，初始情况下没有上锁
{
	int i = 0;
	schedLockCount = 0;
	tBitmapInit(&taskPrioBM);
	for(i = 0; i<TINYOS_PIORITY_MAX;i++)
	{
		tListInit(&TaskTable[i]);
	}
}

void tTaskSchedLockEnable(void) //上锁
{
	uint32_t status = tTaskEnterCritical();
	{
		if(schedLockCount < 255)
		{
			schedLockCount++;
		}
	}
	tTaskExitCritical(status);
}

void tTaskSchedLockDisable(void)//解锁
{
	uint32_t status = tTaskEnterCritical();
	{
		if(schedLockCount > 0)
		{
			if(--schedLockCount == 0)
			{
				tTaskSchedule(); 
			}
		}
	}
	tTaskExitCritical(status);
}

void tTaskSchedule(void) 
{    
	tTask * tempTask;
    uint32_t status = tTaskEnterCritical();
    {
		if(schedLockCount > 0)
		{
			tTaskExitCritical(status);	//一定是和tTaskEnterCritical();函数成对出现的
			return ;
		}
		
		tempTask = tTaskGetReady();
		if(tempTask != currTask)
		{
			nextTask = tempTask;
			#if TINYOS_ENABLE_HOOKS == 1
				tHooksTaskSchedule(currTask, nextTask);
			#endif
			tTaskSwitch();   
		}	
	}
    tTaskExitCritical(status); 
}

//从延时列表中剔除延时结点
void tTaskRFDelayList(tTask * task)
{
	tListRemove(&taskDelayList,&(task->delayNode));
	task->State &= ~TINYOS_TASK_STA_DELAY;
}

//将任务放入延时列表
void tTaskADTDelayList(tTask * task, uint32_t ticks)
{
    task->taskDelayTicks = ticks;
    tListAddLast(&taskDelayList, &(task->delayNode)); 
    task->State |= TINYOS_TASK_STA_DELAY;
}

//把任务设置为就绪状态
void tTaskSetReady (tTask * task)
{
	tListAddLast(&TaskTable[task->priority],&(task->linkNode));
	//有任务处于该优先级，且是就绪状态
    tBitmapSet(&taskPrioBM, task->priority);
}

//将任务从就绪态移除
void tTaskRFReadyList(tTask * task)
{
	tListRemove(&TaskTable[task->priority],&(task->linkNode));
	if(tListCount(&TaskTable[task->priority]) == 0)	//该优先级列表下没有其他任务，该位位图清零
	{
		tBitmapClear(&taskPrioBM, task->priority);
	}
}



//任务挂起函数
void tTaskSuspend(tTask * task)
{
	uint32_t status = tTaskEnterCritical();
	{
		if((task->State & TINYOS_TASK_STA_DELAY) == 0)	//任务处于延时状态不会被挂起
		{
			if(++task->suspendCount < 2)
			{
				task->State |= TINYOS_TASK_STA_SUSPEND;
				tTaskRFReadyList(task);
			}
			
			if(task == currTask)
			{
				tTaskSchedule();
			}
		}
	}	
	tTaskExitCritical(status);
}

void tTaskResume(tTask * task)
{
	if((task->State & TINYOS_TASK_STA_SUSPEND) != 0)
	{
		if(--task->suspendCount == 0)
		{
			task->State &= ~TINYOS_TASK_STA_SUSPEND;
			tTaskSetReady(task);
			tTaskSchedule();
		}
	}
}

//任务删除的回调函数
void tTaskDeleteCallback(tTask * task, void(*clean)(void * param),void * param)
{
	task->clean = clean;
	task->cleanParam = param;
}

//删除当前运行的任务
void tTaskDelRunning(tTask * task)
{
	tListRemove(&TaskTable[task->priority],&(task->linkNode));
	if(tListCount(&TaskTable[task->priority]) == 0)	//该优先级列表下没有其他任务，该位位图清零
	{
		tBitmapClear(&taskPrioBM, task->priority);
	}
}

void tTaskForceDelete(tTask * task)
{
	uint32_t status = tTaskEnterCritical();
	{
		
		if(task->State & TINYOS_TASK_STA_DELAY)	//判断任务是否处于延时队列
		{
			tListRemove(&taskDelayList, &(task->delayNode));
		}//任务如果处于挂起状态，那他就不存在于延时和就绪队列中
		else if((task->State & TINYOS_TASK_STA_SUSPEND) == 0)//如果处于就绪列表
		{
			tTaskRFReadyList(task);
		}
		/***
			检查 task->clean 是否为空指针
			确保在调用清理函数之前它是有效的
			如果任务在创建或初始化时未设置清理函数
			那么这段代码将避免不必要的调用。
		***/
		if(task->clean != NULL)	
		{
			//运行后，程序跳转到该任务对应的清理函数执行相关代码
			task->clean(task->cleanParam);
		}
		
		if(task == currTask)
		{
			tTaskSchedule();
		}
	}	
	tTaskExitCritical(status);
}

void tTaskRequestDelete(tTask * task)
{
	uint32_t status = tTaskEnterCritical();
	{
		task->tTaskDelRequest = TINYOS_TASK_DELRQST;
	}	
	tTaskExitCritical(status);
}

uint8_t tTaskIsRequestDel(void)
{
	uint8_t del;
	uint32_t status = tTaskEnterCritical();
	{
		del = currTask->tTaskDelRequest;
	}	
	tTaskExitCritical(status);
	
	return del;
}

void tTaskDeleteSelf(void)
{
	uint32_t status = tTaskEnterCritical();
	{
		//删除的时候任务一定不在延时列表
		tTaskDelRunning(currTask);		
		if(currTask->clean)
		{
			currTask->clean(currTask->cleanParam);
		}
		tTaskSchedule();
	}	
	tTaskExitCritical(status);
}

void tTaskSystemTickHandler(void)
{	
	listNode * currNode; 
	uint32_t status = tTaskEnterCritical();
	{
		//扫描整个延时列表
		for(currNode = taskDelayList.headNode.next; currNode != &(taskDelayList.headNode);currNode = currNode->next)
		{
			//找到当前延时结点属于哪个任务 
			//参数：当前字段地址，父系结构类型，在该结构中的位置（变量名）
			tTask * task = tNodeParent(currNode, tTask, delayNode);
			if(--task->taskDelayTicks == 0)
			{
				//如果任务还处于等待事件的状态，则将其从事件等待队列中唤醒
				if (task->waitEvent != (void*)0) 
				{
					// 此时，消息为空，等待结果为超时
					tEventRemoveTask(task, (void *)0, tErrorCodeTimeOut);
				}
				
				//从延时列表中剔除，并把任务设置为就绪状态
				tTaskRFDelayList(task);				
				tTaskSetReady(task);				
			}
		}
		
		//如果当前任务运行时间片超过100ms,将该任务放到列表的最后，并刷新时间片长度
		if(--currTask->slice == 0)
		{
			if(tListCount(&TaskTable[currTask->priority]) > 1)
			{
				tListRemoveFirst(&TaskTable[currTask->priority]);
				
				tListAddLast(&TaskTable[currTask->priority],&(currTask->linkNode));
				currTask->slice = TINYOS_SLICE_MAX;
			}
		}
	}
	// 节拍计数自增
	tickCount++;
	
#if TINYOS_ENABLE_CPUSTATE == 1
    // 检查cpu使用率
    checkCpuUsage();
#endif
	
	tTaskExitCritical(status);
	
#if TINYOS_ENABLE_SOFTTIMER == 1
    // 通知定时器模块节拍事件
    tTimerMouduleNotify();
#endif
	
#if TINYOS_ENABLE_HOOKS == 1
	tHooksSysTickHandler();
#endif	
	// 这个过程中可能有任务延时完毕(delayTicks = 0)，进行一次调度。
	tTaskSchedule();
}

void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();	
	{
		// 设置延时值，插入延时队列，并把任务设置为延时状态
		tTaskADTDelayList(currTask, delay);		
		tTaskRFReadyList(currTask);
		
		tTaskSchedule();
	}	
	tTaskExitCritical(status);	
}

//通过复制的方式获取任务信息
void tTaskGetInfo(tTask* task,tTaskInfo * taskInfo)
{
	tTaskStack * stackBottom;
	uint32_t status = tTaskEnterCritical();	
	{
		taskInfo->suspendCount = task->suspendCount;
		taskInfo->delayTicks = task->taskDelayTicks;
		taskInfo->priority = task->priority;
		taskInfo->slice = task->slice;
		taskInfo->State = task->State;
		taskInfo->stackSize = task->stackSize;
		taskInfo->stackFree = 0;
		
		stackBottom = task->stackBase;
		
		//从堆栈基地址向上检查找到第一个不为零的位置，且该位置没有超出堆栈大小
		while((*stackBottom++) == 0 && (stackBottom <= task->stackBase + task->stackSize / sizeof(tTaskStack)) )
		{
			taskInfo->stackFree++;
		}
		taskInfo->stackFree *= sizeof(tTaskStack);	//将32位堆栈大小转换为字节
	}	
	tTaskExitCritical(status);	
}
