#ifndef __tTASK_H
#define __tTASK_H

#include "tLib.h"
#include "sys.h"

typedef uint32_t tTaskStack;

//前向引用，避免包含整个文件
struct tEvent;

typedef struct _tTask
{
	tTaskStack *stack;
	listNode delayNode;
	listNode linkNode;
	
    uint32_t * stackBase;	 // 堆栈的基地址
    uint32_t stackSize;	     // 堆栈的大小
	
	uint32_t taskDelayTicks;	
	uint32_t State;
	uint32_t priority;
	uint32_t suspendCount;
	uint32_t slice; //时间片计数器
	
	void(*clean)(void * param);
	void * cleanParam;
	uint8_t tTaskDelRequest;
	
	struct _tEvent * waitEvent;	//添加任务的事件控制块
	void * eventMsg;			
	uint32_t waitEventRes; 
	
	uint32_t waitFlagsType;
	uint32_t eventRqFlags;
	
}tTask;

typedef struct _tTaskInfo
{
	uint32_t suspendCount;
	uint32_t delayTicks;
	uint32_t priority;
	uint32_t State;
	uint32_t slice;
	
    uint32_t stackSize;// 堆栈的总容量
    uint32_t stackFree;// 堆栈空余量
}tTaskInfo;

extern tTask* currTask;
extern tTask* nextTask;
extern tTask* idleTask;
extern tList TaskTable[];

extern uint8_t schedLockCount;
extern tList taskDelayList;

void tTaskGetInfo(tTask* task,tTaskInfo * taskInfo);

void tTaskInit(tTask* task ,void(*entry)(void *),void * param,uint32_t priority,tTaskStack * stackBottom,uint32_t size);
void tTaskSwitch(void);
void tTaskRunFirst(void);

uint32_t tTaskEnterCritical(void);
void tTaskExitCritical(uint32_t status);

tTask * tTaskGetReady(void);
void tTaskSchedule(void);
void tTaskSchedInit(void);
void tTaskSchedLockEnable(void);
void tTaskSchedLockDisable(void);

void tTaskADTDelayList(tTask * task, uint32_t ticks);
void tTaskRFDelayList(tTask * task);
void tTaskSetReady (tTask * task);
void tTaskRFReadyList(tTask * task);

void tTaskSuspend(tTask * task);
void tTaskResume(tTask * task);
void tTaskDelRunning(tTask * task);

//任务请求删除的回调函数
void tTaskDeleteCallback(tTask * task, void(*clean)(void * param),void * param);
void tTaskForceDelete(tTask * task);
void tTaskRequestDelete(tTask * task);
uint8_t tTaskIsRequestDel(void);
void tTaskDeleteSelf(void);

void tTaskSystemTickHandler(void);

#endif
