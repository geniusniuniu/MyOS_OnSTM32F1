#ifndef __tMSGBOX_H
#define __tMSGBOX_H

#include "tinyOS.h"

#define tMSGHIGHPRIO	0x01
#define tMSGNORMAL		0x00

typedef struct _tMsgBox
{
	tEvent event;	//����ĵȴ�����
	
	uint32_t msgCount;
	uint32_t msgCountMax;
	uint32_t read;		//��Ϣ��ָ��
	uint32_t write;		//дָ��
	
	void ** msgBuffer;	//��Ϣ������
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
//notifyPrio������߱��������Ϣ�����ȼ�
uint32_t tMsgNotify(tMsgBox * msgBox,void *msg,uint32_t notifyPrio);

void tMsgClearUp(tMsgBox * msgBox);
uint32_t tMsgDelete(tMsgBox * msgBox);
void tMsgGetInfo(tMsgBox * msgBox,tMsgBoxInfo * MsgBoxInfo);

#endif


