#ifndef __tCPU_H
#define __tCPU_H

#include "tinyOS.h"

extern uint32_t idleCount;
extern uint32_t idleCountMax;
extern uint32_t tickCount;	//统计时钟节拍发生的次数


void initCpuUsageState (void);
void checkCpuUsage(void);
void cpuUsagSyncWithSystick(void);
float cpuUsageGet(void);


#endif



