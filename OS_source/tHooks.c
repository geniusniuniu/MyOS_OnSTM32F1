#include "tHooks.h"

#if TINYOS_ENABLE_HOOKS == 1


//cpu����ʱ��hooks
void tHooksCpuIdle (void)
{

}

//ʱ�ӽ����жϴ���Hooks
void tHooksSysTickHandler (void)
{

}

//�����л�hooks
//from ���ĸ�����ʼ�л�
//to �л����ĸ�����
void tHooksTaskSchedule (tTask * from, tTask * to)
{

}

//�����ʼ����Hooks
void tHooksTaskInit (tTask * task)
{

}


#endif




