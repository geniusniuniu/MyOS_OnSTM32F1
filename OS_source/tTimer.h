#ifndef __tTIMER_H
#define __tTIMER_H

#include "tinyOS.h"

#define TIMER_CONFIG_TYPE_HARDWARE	(1 << 0)
#define TIMER_CONFIG_TYPE_TASK		(0 << 0)

typedef enum _tTimerState
{
	tTimerCreated = 0,
	tTimerStart,
	tTimerRunning,
	tTimerStop,
	tTimerDeleted,
	
}tTimerState;

typedef struct _tTimer
{
	listNode linkNode;
	uint32_t startDelayTicks;	// ����������ʱ������ʱ��ticks��
	uint32_t durationTicks;		// ���ڼ���ֵ
	uint32_t currentTicks;		// ��ǰ��ʱ�ݼ�����ֵ
	void (*tTimerFunc) (void * arg);
	void * arg;
	uint32_t config;
	
	tTimerState state;
}tTimer;

typedef struct _tTimerInfo
{
	uint32_t startDelayTicks;	// ����������ʱ������ʱ��ticks��
	uint32_t durationTicks;		// ���ڼ���ֵ
	void (*tTimerFunc) (void * arg);
	void * arg;
	uint32_t config;	
	tTimerState state;
}tTimerInfo;


/**********************************************************************************************************
** Function name        :   tTimerInit
** Descriptions         :   ��ʼ����ʱ��
** parameters           :   timer �ȴ���ʼ���Ķ�ʱ��
** parameters           :   startDelayTicks ��ʱ����ʼ��������ʱticks����
** parameters           :   durationTicks �������Զ�ʱ���õ�����tick����һ���Զ�ʱ����Ч
** parameters           :   timerFunc ��ʱ���ص�����
** parameters           :   arg ���ݸ���ʱ���ص������Ĳ���
** parameters           :   config ��ʱ���ĳ�ʼ����
** Returned value       :   ��
***********************************************************************************************************/
void tTimerInit(tTimer * timer,uint32_t startDelayTicks,uint32_t durationTicks,void(*tTimerFunc)(void * arg),void * arg,uint32_t config);
void tTimerModuleInit(void);
//��ʱ������
void tTimerStartFunc(tTimer * timer);	
void tTimerStopFunc(tTimer * timer);
void tTimerDelFunc(tTimer * timer);
void tTimerGetInfo(tTimer * timer,tTimerInfo * info);

//����ָ���Ķ�ʱ���б����ø�����ʱ���Ĵ�����
void tTimerListCallFunc(tList * timerList);

void tTimerMouduleNotify(void);


#endif






