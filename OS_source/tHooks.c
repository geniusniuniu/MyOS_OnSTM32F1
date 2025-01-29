#include "tHooks.h"

#if TINYOS_ENABLE_HOOKS == 1


//cpu空闲时的hooks
void tHooksCpuIdle (void)
{

}

//时钟节拍中断处理Hooks
void tHooksSysTickHandler (void)
{

}

//任务切换hooks
//from 从哪个任务开始切换
//to 切换至哪个任务
void tHooksTaskSchedule (tTask * from, tTask * to)
{

}

//任务初始化的Hooks
void tHooksTaskInit (tTask * task)
{

}


#endif




