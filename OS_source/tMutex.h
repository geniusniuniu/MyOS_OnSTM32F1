#ifndef __tMUTEX_H
#define __tMUTEX_H

#include "tinyOS.h"

typedef struct _tMutex
{
	//event 是一个结构体实例。
	//当创建 tMutex 时,event的内存
	//会被分配在tMutex的内存空间中可以直接使用。
	tEvent   event;
	tTask *  owner;
	uint32_t ownerOriginalPrio;
	uint32_t lockCount;
}tMutex;

typedef struct _tMutexInfo
{
	tTask *  owner;
	uint32_t taskCount;
	uint32_t lockCount;
	uint32_t inheritedPrio;	//继承的优先级
	uint32_t ownerOriginalPrio;
}tMutexInfo;

void tMutexInit(tMutex * mutex);
uint32_t tMutexWait(tMutex * mutex,uint32_t waitTicks);
uint32_t tMutexNoWait(tMutex * mutex);
uint32_t tMutexNotify(tMutex * mutex);

uint32_t tMutexDelete(tMutex * mutex);
void tMutexGetInfo(tMutex * mutex,tMutexInfo * info);

#endif







