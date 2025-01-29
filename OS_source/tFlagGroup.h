#ifndef __tFLAGGROUP_H
#define __tFLAGGROUP_H

#include "tinyOS.h"

#define	FGRP_CHECK_IS_SET	 	0x01
#define FGRP_CHECK_NOT_SET	 	0x00

#define	tFLAGGROUP_CLEAR		(0x0 << 0)
#define	tFLAGGROUP_SET			(0x1 << 0)
#define	tFLAGGROUP_ANY			(0x0 << 1)
#define	tFLAGGROUP_ALL			(0x1 << 1)


//等待的事件类型
#define tFLAGGROUP_SET_ALL		(tFLAGGROUP_SET | tFLAGGROUP_ALL)		//0x11
#define	tFLAGGROUP_SET_ANY		(tFLAGGROUP_SET | tFLAGGROUP_ANY)		//0x01
#define tFLAGGROUP_CLEAR_ALL	(tFLAGGROUP_CLEAR | tFLAGGROUP_ALL)		//0x10
#define tFLAGGROUP_CLEAR_ANY	(tFLAGGROUP_CLEAR | tFLAGGROUP_ANY)		//0x00

#define	tFLAGGROUP_CONSUME		(0x1 << 7)


typedef struct _tFlagGroup
{
	tEvent event;
	uint32_t flag;
	
}tFlagGroup;


typedef struct _tFlagGroupInfo
{
	uint32_t taskCount;
	uint32_t currFlag;
	
}tFlagGroupInfo;

void tFlagGroupInit(tFlagGroup * flagGroup,uint32_t flags);
uint32_t tFlagGroupWait(tFlagGroup * flagGroup,uint32_t waitType,uint32_t rqFlag,uint32_t * resFlag,uint32_t waitTicks);
uint32_t tFlagGroupNoWait(tFlagGroup * flagGroup,uint32_t waitType,uint32_t rqFlag,uint32_t * resFlag);
void tFlagGroupNotify(tFlagGroup * flagGroup,uint32_t isSet,uint32_t flag);

uint32_t tFlagGroupDelete(tFlagGroup * flagGroup);
void tFlagGroupGetInfo(tFlagGroup * flagGroup,tFlagGroupInfo * info);


#endif


