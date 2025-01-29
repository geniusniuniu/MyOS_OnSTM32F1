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

void tTimerStartFunc(tTimer * timer)	//��ʱ������
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
				// ���������ʱ����������Ҫ���ٽ����б���,��Ϊ������Դ�ķ���ֻ�����ڲ�ͬ�����
				// �ź����ڳ�ʼ��ʱ���趨����ֵΪ 1����ʾ��ǰ����ͬʱ�������ʸ���Դ����������ֻ����һ����
				// ÿ��һ������ɹ���ȡ��Դ��tTimerSoftList��ʱ���ź����ļ�������٣�
				// ÿ��һ�������ͷ���Դʱ�����������ӡ�ͨ�������Ĺ���ϵͳ�ܹ���̬�ص��ض���Դ�ķ��ʡ�
				// ��һ�������ڵ��� tSemaphoreWait ʱ�������Դ�����ã������Ա�����
				// �ȴ�ֱ����Դ���ã������ǳ���ռ�� CPU��
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
            // ����Ѿ��������ж϶�ʱ�����ͣ�Ȼ�����Ӧ����ʱ�б����Ƴ�
            if (timer->config & TIMER_CONFIG_TYPE_HARDWARE)
            {
                // Ӳ��ʱ������ʱ�ӽ����ж��д�������ʹ��critical������
                uint32_t status = tTaskEnterCritical();
				{
					// ��Ӳ��ʱ���б����Ƴ�
					tListRemove(&tTimerHardList, &timer->linkNode);
				}
                tTaskExitCritical(status);
            }
            else
            {
                // ��ʱ�����Ȼ�ȡ�ź������Դ����ʱ��ʱ�������ʱͬʱ�ڷ�����ʱ���б��µĳ�ͻ����
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

//����ָ���Ķ�ʱ���б����ø�����ʱ���Ĵ�����
static void tTimerListCallFunc(tList * timerList)
{
	listNode * node;
	
	for(node = timerList->headNode.next; node != &(timerList->headNode); node = node->next)
	{
		tTimer * timer = tNodeParent(node, tTimer, linkNode);
		if((timer->currentTicks == 0 ) || (--timer->currentTicks == 0) )	//��ʱʱ�䵽�ˣ�ִ����Ӧ������
		{
			timer->state = tTimerRunning;
			timer->tTimerFunc(timer->arg);	
			//ִ�����ʱ��������̬�л�������״̬
			timer->state = tTimerStart;
			
			if(timer->durationTicks > 0)	//�ж϶�ʱ���Ƿ�����������
			{
				timer->currentTicks = timer->durationTicks;
			}
			else //������������ʱ���Ƴ���ʱ���б�
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





