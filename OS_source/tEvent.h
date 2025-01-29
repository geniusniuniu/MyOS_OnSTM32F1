#ifndef __tEVENT_H
#define __tEVENT_H

#include "tLib.h"
#include "sys.h"
#include "tTask.h"

// Event类型
typedef enum  _tEventType {   
    tEventTypeUnknown   = (0 << 16), 				// 未知类型
    tEventTypeSema   	= (1 << 16), 				// 信号量类型
    tEventTypeMsgbox  	= (2 << 16), 				// 邮箱类型
	tEventTypeMemBlock  = (3 << 16),				// 存储块类型
	tEventTypeFlagGroup = (4 << 16),				// 事件标志组
	tEventTypeMutex     = (5 << 16),				// 互斥信号量类型
}tEventType;

typedef struct _tEvent	//事件控制块
{
	tEventType type;
	tList waitingList;
	
}tEvent;

void tEventInit(tEvent * event,tEventType type);

//事件的等待,超时处理
void tEventWait(tEvent * event,tTask * task,void * msg,uint32_t state,uint32_t timeOut);

//从事件控制块中唤醒首个等待的任务
tTask * tEventWkUp(tEvent * event,void * msg,uint32_t result);
//从事件控制块中唤醒指定任务
void tEventWkUpSpecify(tEvent * event,tTask * task,void * msg,uint32_t result);


//将任务从其事件等待队列中强制移除
void tEventRemoveTask (tTask * task, void * msg, uint32_t result);
uint32_t tEventRemoveAll(tEvent * event, void * msg, uint32_t result);
uint32_t tEventWaitTaskCount(tEvent * event);

#endif 
