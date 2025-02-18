#ifndef __tinyOS_H
#define __tinyOS_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "tLib.h"
#include "tTask.h"
#include "sys.h"
#include "tEvent.h"
#include "tSemaphore.h"
#include "tMsgBox.h"
#include "tMemBlock.h"
#include "tFlagGroup.h"
#include "tMutex.h"
#include "tTimer.h"
#include "tCPU.h"
#include "tHooks.h"


#define NVIC_INT_CTRL   			  0xE000ED04	//寄存器地址
#define NVIC_PENDSVSET  			  0x10000000	//寄存器写入的值
#define NVIC_PENDSV_PRI  			  0x000000FF  	//寄存器写入的值
#define NVIC_SYSPRI2				  0xE000ED22  	//寄存器地址

#define MEM32(addr)					  *(volatile unsigned long *)(addr)
#define MEM8(addr)					  *(volatile unsigned char *)(addr)

#define TINYOS_SYSTICK_MS			  10
#define TINYOS_INTERRUPT_PER_SECOND   (1000 / TINYOS_SYSTICK_MS)

#define TINYOS_PIORITY_MAX  		  32
#define TINYOS_SLICE_MAX  		      10		 //同优先级下，每个任务分得的时间片不超过 100ms

#define TINYOS_TIMERTASK_PRIORITY	  1 

#define TINYOS_IDLETASK_STACK_SIZE	  128
#define TINYOS_TIMERTASK_STACK_SIZE	  128

#define TINYOS_APPTASK_STACK_SIZE     128         //应用任务堆栈大小

//使用任务结构体中
//state高16位记录事件状态
#define TINYOS_EVENT_STA_MASK 		  (0xFF << 16)

//使用任务结构体中
//state低16位记录任务状态
#define TINYOS_TASK_STA_READY		  (0 << 0)
#define TINYOS_TASK_STA_DELAY		  (1 << 1)
#define TINYOS_TASK_STA_SUSPEND		  (1 << 2)
#define TINYOS_TASK_STA_DELETE		  (1 << 3)

#define TINYOS_TASK_DELRQST    		  1
#define TINYOS_TASK_NODELRQST		  0

//内核裁剪相关宏
#define TINYOS_ENABLE_SEMAPHORE       1             // 是否使能信号量
#define TINYOS_ENABLE_MUTEX           1             // 是否使能互斥信号量
#define TINYOS_ENABLE_FLAGGROUP       1             // 是否使能事件标志组
#define TINYOS_ENABLE_MSGBOX          1             // 是否使能邮箱
#define TINYOS_ENABLE_MEMBLOCK        1             // 是否使能存储块
#define TINYOS_ENABLE_SOFTTIMER       1             // 是否使能定时器
#define TINYOS_ENABLE_CPUSTATE        1				// 是否使能CPU使用率统计
#define TINYOS_ENABLE_HOOKS      	  0

typedef enum _tErrorCode
{
	tErrorCodeNone = 0,
	tErrorCodeTimeOut,
	tErrorCodeNA,
	tErrorCodeDel,
	tErrorCodeFull,
	tErrorCodeOwner,
}tErrorCode;


#endif





















