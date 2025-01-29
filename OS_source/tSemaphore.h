#ifndef __tSEMAPHORE_H
#define __tSEMAPHORE_H

#include "tinyOS.h"

struct tEvent;

typedef struct _tSemaphore
{
	tEvent event;
	uint32_t semaCount;
	uint32_t semaCountMax;
	
}tSemaphore;

typedef struct _tSemaInfo
{
	uint32_t Count;
	uint32_t CountMax;
	uint32_t taskCount;
	
}tSemaInfo;

void tSemaphoreInit(tSemaphore * sema,uint32_t semaCount,uint32_t semaCountMax);

uint32_t tSemaphoreWait(tSemaphore * sema,uint32_t WaitTicks);

//获取信号量且无等待时间
uint32_t tSemaphoreNoWait(tSemaphore * sema);
//释放信号量
void tSemaphoreNotify(tSemaphore * sema);

void tSemaGetInfo(tSemaphore * sema,tSemaInfo * semaInfo);

uint32_t tSemaClearUp(tSemaphore * sema);

#endif 

