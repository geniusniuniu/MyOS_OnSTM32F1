#ifndef __tMSGBOX_H
#define __tMSGBOX_H

#include "tinyOS.h"

#define tMsgHIGHPRIO	0x01
#define tMsgNORMAL		0x00

typedef struct _tMsgBox
{
	tEvent event;	//任务的等待队列
	
	uint32_t msgCount;
	uint32_t msgCountMax;
	uint32_t read;		//消息读指针
	uint32_t write;		//写指针
	
	void ** msgBuffer;	//消息缓冲区
}tMsgBox;

typedef struct _tMsgBoxInfo
{
	uint32_t count;
	uint32_t countMax;	
	uint32_t taskCount;
}tMsgBoxInfo;


void tMsgBoxInit(tMsgBox * msgBox,void ** msgBuffer,uint32_t msgCountMax);
uint32_t tMsgWait(tMsgBox * msgBox,void ** msg,uint32_t WaitTicks);
uint32_t tMsgNoWait(tMsgBox * msgBox,void **msg);
//notifyPrio用来提高被处理的消息的优先级
uint32_t tMsgNotify(tMsgBox * msgBox,void *msg,uint32_t notifyPrio);

void tMsgClearUp(tMsgBox * msgBox);
uint32_t tMsgDelete(tMsgBox * msgBox);
void tMsgGetInfo(tMsgBox * msgBox,tMsgBoxInfo * MsgBoxInfo);

#endif


