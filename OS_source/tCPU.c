#include "tCPU.h"

uint32_t idleCount;
uint32_t idleCountMax;
uint32_t tickCount;			     //ͳ��ʱ�ӽ��ķ����Ĵ���

#if TINYOS_ENABLE_CPUSTATE == 1

static float cpuUsage;          // cpuʹ����ͳ��
uint32_t enableCpuUsageState;	// �Ƿ�ʹ��cpuͳ��

void initCpuUsageState (void)
{
	cpuUsage = 0.0f;
	
	idleCount = 0;
	idleCountMax = 0;
	tickCount = 0;
	enableCpuUsageState = 0;
}

//Ϊ���cpuʹ������ϵͳʱ�ӽ���ͬ��
void cpuUsagSyncWithSystick(void)	
{
	while(enableCpuUsageState == 0)
	{
		
	}
}

//���cpuʹ����
void checkCpuUsage(void)
{
	if(enableCpuUsageState == 0)
	{
		enableCpuUsageState = 1;	//��tTaskSystemTickHandler�����ж�ʱ��������λ������ʱ��ͬ��
		tickCount = 0;				//�������
		return ;	
	}
	if(tickCount == TINYOS_INTERRUPT_PER_SECOND)
	{
		idleCountMax = idleCount;	//��һ�룬��¼����������idleCount��������
		idleCount = 0;
		
		tTaskSchedLockDisable();	//�����������
	}
	else if(tickCount % TINYOS_INTERRUPT_PER_SECOND == 0)	//֮���ÿһ�����CPUʹ����
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



