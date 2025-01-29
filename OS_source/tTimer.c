#include "tTimer.h"

#if TINYOS_ENABLE_SOFTTIMER == 1

static tTask timerTask;
static tTaskStack tTimerTaskStack[TINYOS_TIMERTASK_STACK_SIZE];

tList tTimerHardList;
tList tTimerSoftList;

tSemaphore softWareProtectsema;
tSemaphore timerTickSema;

void tTimerInit(tTimer * timer,uint32_t startDelayTicks,uint32_t durationTicks,void(*tTimerFunc)(void * arg),void * arg,uint32_t config)
{
	listNodeInit(&timer->linkNode);
	timer->startDelayTicks = startDelayTicks;
	timer->durationTicks = durationTicks;
	timer->tTimerFunc = tTimerFunc;
	timer->arg = arg;
	timer->config = config;
	
	if(startDelayTicks == 0)
	{
		timer->currentTicks = durationTicks;
	}
	else
	{
		timer->currentTicks = timer->startDelayTicks;
	}
	timer->state = tTimerCreated;
}

void tTimerStartFunc(tTimer * timer)	//定时器启动
{
	switch (timer->state)
	{
		case tTimerCreated :
		case tTimerStart :
			timer->currentTicks = timer->startDelayTicks > 0 ? timer->startDelayTicks : timer->durationTicks;
			timer->state = tTimerStart;
		
			if((timer->config & TIMER_CONFIG_TYPE_HARDWARE) == 1)
			{
				uint32_t status = tTaskEnterCritical();
				{
					tListAddLast(&tTimerHardList,&timer->linkNode);
				}
				tTaskExitCritical(status);
			}
			else
			{
				// 对于软件定时器，并不需要在临界区中保护,因为共享资源的访问只存在于不同任务间
				// 信号量在初始化时会设定计数值为 1，表示当前可以同时并发访问该资源的任务数量只能是一个。
				// 每次一个任务成功获取资源（tTimerSoftList）时，信号量的计数会减少；
				// 每当一个任务释放资源时，计数会增加。通过这样的管理，系统能够动态地调控对资源的访问。
				// 当一个任务在调用 tSemaphoreWait 时，如果资源不可用，它可以被挂起，
				// 等待直到资源可用，而不是持续占用 CPU。
				tSemaphoreWait(&softWareProtectsema,0);	
				{
					tListAddLast(&tTimerSoftList,&timer->linkNode);
				}
				tSemaphoreNotify(&softWareProtectsema);
			}
			break;
			
		default :
			
			break;
	}
}

void tTimerStopFunc(tTimer * timer)
{
    switch (timer->state)
    {
        case tTimerStart:
        case tTimerRunning:
            // 如果已经启动，判断定时器类型，然后从相应的延时列表中移除
            if (timer->config & TIMER_CONFIG_TYPE_HARDWARE)
            {
                // 硬定时器，在时钟节拍中断中处理，所以使用critical来防护
                uint32_t status = tTaskEnterCritical();
				{
					// 从硬定时器列表中移除
					tListRemove(&tTimerHardList, &timer->linkNode);
				}
                tTaskExitCritical(status);
            }
            else
            {
                // 软定时器，先获取信号量。以处理此时定时器任务此时同时在访问软定时器列表导致的冲突问题
                tSemaphoreWait(&softWareProtectsema, 0);
                {
					tListRemove(&tTimerSoftList, &timer->linkNode);
				}
                tSemaphoreNotify(&softWareProtectsema);
            }
            timer->state = tTimerStop;
            break;
        default:
            break;
    }
}

void tTimerDelFunc(tTimer * timer)
{
	tTimerStopFunc(timer);
	timer->state = tTimerDeleted;
}

void tTimerGetInfo(tTimer * timer,tTimerInfo * info)
{
	uint32_t status = tTaskEnterCritical();
	{
		info->startDelayTicks = timer->startDelayTicks;
		info->durationTicks = timer->durationTicks;
		info->tTimerFunc = timer->tTimerFunc;
		info->arg = timer->arg;
		info->config = timer->config;
		info->state = timer->state;
	}
	tTaskExitCritical(status);
}

//遍历指定的定时器列表，调用各个定时器的处理函数
static void tTimerListCallFunc(tList * timerList)
{
	listNode * node;
	
	for(node = timerList->headNode.next; node != &(timerList->headNode); node = node->next)
	{
		tTimer * timer = tNodeParent(node, tTimer, linkNode);
		if((timer->currentTicks == 0 ) || (--timer->currentTicks == 0) )	//延时时间到了，执行相应处理函数
		{
			timer->state = tTimerRunning;
			timer->tTimerFunc(timer->arg);	
			//执行完后定时器由运行态切换回启动状态
			timer->state = tTimerStart;
			
			if(timer->durationTicks > 0)	//判断定时器是否是周期运行
			{
				timer->currentTicks = timer->durationTicks;
			}
			else //否则将这个软件定时器移除定时器列表
			{
				tListRemove(timerList,&timer->linkNode);
				timer->state = tTimerStop;
			}
			
		}
	}
}

void tTimerMouduleNotify(void)
{
	uint32_t status = tTaskEnterCritical();
	{
		tTimerListCallFunc(&tTimerHardList);
	}
	tTaskExitCritical(status);
	
	tSemaphoreNotify(&timerTickSema);
}

static void tTimerSoftTask(void * param)
{
	for(;;)
	{
		tSemaphoreWait(&timerTickSema,0);
		tSemaphoreWait(&softWareProtectsema,0);
		tTimerListCallFunc(&tTimerSoftList);
		tSemaphoreNotify(&softWareProtectsema);
		
	}
	
}

void tTimerModuleInit(void)
{
	tListInit(&tTimerHardList);
	tListInit(&tTimerSoftList);
	
	tSemaphoreInit(&softWareProtectsema,1,1);
	tSemaphoreInit(&timerTickSema,0,0);
	
#if	TINYOS_TIMERTASK_PRIORITY >= (TINYOS_PIORITY_MAX - 1)
	#error "THE PRIORITY OF TIMERTASK MUST HIGHER THAN (TINYOS_PIORITY_MAX - 1)"
#endif
	
	tTaskInit(&timerTask,tTimerSoftTask,(void *)0x12345678,TINYOS_TIMERTASK_PRIORITY,tTimerTaskStack,TINYOS_TIMERTASK_STACK_SIZE);
}


#endif





