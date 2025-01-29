#include "tCPU.h"

uint32_t idleCount;
uint32_t idleCountMax;
uint32_t tickCount;			     //统计时钟节拍发生的次数

#if TINYOS_ENABLE_CPUSTATE == 1

static float cpuUsage;          // cpu使用率统计
uint32_t enableCpuUsageState;	// 是否使能cpu统计

void initCpuUsageState (void)
{
	cpuUsage = 0.0f;
	
	idleCount = 0;
	idleCountMax = 0;
	tickCount = 0;
	enableCpuUsageState = 0;
}

//为检查cpu使用率与系统时钟节拍同步
void cpuUsagSyncWithSystick(void)	
{
	while(enableCpuUsageState == 0)
	{
		
	}
}

//检查cpu使用率
void checkCpuUsage(void)
{
	if(enableCpuUsageState == 0)
	{
		enableCpuUsageState = 1;	//在tTaskSystemTickHandler产生中断时，立即置位，进行时钟同步
		tickCount = 0;				//清零计数
		return ;	
	}
	if(tickCount == TINYOS_INTERRUPT_PER_SECOND)
	{
		idleCountMax = idleCount;	//第一秒，记录空闲任务中idleCount自增次数
		idleCount = 0;
		
		tTaskSchedLockDisable();	//开启任务调度
	}
	else if(tickCount % TINYOS_INTERRUPT_PER_SECOND == 0)	//之后的每一秒计算CPU使用率
	{
		cpuUsage = 100.0 - (idleCount * 100.0) / idleCountMax * 1.0;
	}
}

float cpuUsageGet(void)
{
	float usage = 0;
	uint32_t status = tTaskEnterCritical();
	{
		usage = cpuUsage;
	}
	tTaskExitCritical(status);
	
	return usage;
}


#endif



