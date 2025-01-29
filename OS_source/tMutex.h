#ifndef __tMUTEX_H
#define __tMUTEX_H

#include "tinyOS.h"

typedef struct _tMutex
{
	//event ��һ���ṹ��ʵ����
	//������ tMutex ʱ,event���ڴ�
	//�ᱻ������tMutex���ڴ�ռ��п���ֱ��ʹ�á�
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
	uint32_t inheritedPrio;	//�̳е����ȼ�
	uint32_t ownerOriginalPrio;
}tMutexInfo;

void tMutexInit(tMutex * mutex);
uint32_t tMutexWait(tMutex * mutex,uint32_t waitTicks);
uint32_t tMutexNoWait(tMutex * mutex);
uint32_t tMutexNotify(tMutex * mutex);

uint32_t tMutexDelete(tMutex * mutex);
void tMutexGetInfo(tMutex * mutex,tMutexInfo * info);

#endif







