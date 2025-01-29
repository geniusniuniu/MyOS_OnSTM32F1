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
	uint32_t startDelayTicks;	// 初次启动定时器后延时的ticks数
	uint32_t durationTicks;		// 周期计数值
	uint32_t currentTicks;		// 当前定时递减计数值
	void (*tTimerFunc) (void * arg);
	void * arg;
	uint32_t config;
	
	tTimerState state;
}tTimer;

typedef struct _tTimerInfo
{
	uint32_t startDelayTicks;	// 初次启动定时器后延时的ticks数
	uint32_t durationTicks;		// 周期计数值
	void (*tTimerFunc) (void * arg);
	void * arg;
	uint32_t config;	
	tTimerState state;
}tTimerInfo;


/**********************************************************************************************************
** Function name        :   tTimerInit
** Descriptions         :   初始化定时器
** parameters           :   timer 等待初始化的定时器
** parameters           :   startDelayTicks 定时器初始启动的延时ticks数。
** parameters           :   durationTicks 给周期性定时器用的周期tick数，一次性定时器无效
** parameters           :   timerFunc 定时器回调函数
** parameters           :   arg 传递给定时器回调函数的参数
** parameters           :   config 定时器的初始配置
** Returned value       :   无
***********************************************************************************************************/
void tTimerInit(tTimer * timer,uint32_t startDelayTicks,uint32_t durationTicks,void(*tTimerFunc)(void * arg),void * arg,uint32_t config);
void tTimerModuleInit(void);
//定时器启动
void tTimerStartFunc(tTimer * timer);	
void tTimerStopFunc(tTimer * timer);
void tTimerDelFunc(tTimer * timer);
void tTimerGetInfo(tTimer * timer,tTimerInfo * info);

//遍历指定的定时器列表，调用各个定时器的处理函数
void tTimerListCallFunc(tList * timerList);

void tTimerMouduleNotify(void);


#endif






