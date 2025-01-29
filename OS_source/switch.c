/*
	这里的代码都是和CPU相关的
	这里的代码都是和CPU相关的
	这里的代码都是和CPU相关的
*/

#include <stdio.h>
#include "tinyOS.h"
#include "stm32f10x.h"                  // Device header


uint32_t tTaskEnterCritical(void)	//进入临界区函数,返回值 是PRIMASK可屏蔽中断 寄存器的值
{									//会保存当前的中断状态,并禁用中断
	//进入临界区，获取primask值
	uint32_t primask = __get_PRIMASK();
	__disable_irq();	//关闭中断
	return primask;
}

void tTaskExitCritical(uint32_t status)
{
	__set_PRIMASK(status);
}

__asm__ void PendSV_Handler(void)
{
	IMPORT  currTask                  // 使用import导入C文件中声明的全局变量
    IMPORT  nextTask                  // 类似于在C文文件中使用extern int variable
    
    MRS     R0, PSP                   // 获取当前任务的堆栈指针
    CBZ     R0, PendSVHandler_nosave  // if 这是由tTaskSwitch触发的(此时，PSP肯定不会是0了，0的话必定是tTaskRunFirst)触发
                                      // 不清楚的话，可以先看tTaskRunFirst和tTaskSwitch的实现
    STMDB   R0!, {R4-R11}             //     那么，我们需要将除异常自动保存的寄存器这外的其它寄存器自动保存起来{R4, R11}
                                      //     保存的地址是当前任务的PSP堆栈中，这样就完整的保存了必要的CPU寄存器,便于下次恢复
    LDR     R1, =currTask          	  //     保存好后，将最后的堆栈顶位置，保存到currentTask->stack处    
    LDR     R1, [R1]                  //     由于stack处在结构体stack处的开始位置处，显然currentTask和stack在内存中的起始
    STR     R0, [R1]                  //     地址是一样的，这么做不会有任何问题

PendSVHandler_nosave                  // 无论是tTaskSwitch和tTaskSwitch触发的，最后都要从下一个要运行的任务的堆栈中恢复
                                      // CPU寄存器，然后切换至该任务中运行
    LDR     R0, =currTask             // 好了，准备切换了
    LDR     R1, =nextTask             
    LDR     R2, [R1]  
    STR     R2, [R0]                  // 先将currentTask设置为nextTask，也就是下一任务变成了当前任务
 
    LDR     R0, [R2]                  // 然后，从currentTask中加载stack，这样好知道从哪个位置取出CPU寄存器恢复运行
    LDMIA   R0!, {R4-R11}             // 恢复{R4, R11}。为什么只恢复了这么点，因为其余在退出PendSV时，硬件自动恢复

    MSR     PSP, R0                   // 最后，恢复真正的堆栈指针到PSP  
    ORR     LR, LR, #0x04             // 标记下返回标记，指明在退出LR时，切换到PSP堆栈中(PendSV使用的是MSP) 
    BX      LR                        // 最后返回，此时任务就会从堆栈中取出LR值，恢复到上次运行的位置                  // 最后返回，此时任务就会从堆栈中取出LR值，恢复到上次运行的位置
}

void tTaskRunFirst()
{
    // 这里设置了一个标记，PSP = 0, 用于与tTaskSwitch()区分，用于在PEND_SV
    // 中判断当前切换是tinyOS启动时切换至第1个任务，还是多任务已经跑起来后执行的切换
    __set_PSP(0);

    MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;   // 向NVIC_SYSPRI2写NVIC_PENDSV_PRI，设置其为最低优先级
    
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;    // 向NVIC_INT_CTRL写NVIC_PENDSVSET，用于PendSV

    // 可以看到，这个函数是没有返回
    // 这是因为，一旦触发PendSV后，将会在PendSV后立即进行任务切换，切换至第1个任务运行
    // 此后，tinyOS将负责管理所有任务的运行，永远不会返回到该函数运行
}

void tTaskSwitch(void)
{
	/*  你可以在临界区中调用 tTaskSchedule()，它会在离开临界区并恢复中断后有效。
		PendSV 中断会在中断恢复后被处理，从而达到调度任务的目的。
		这保证了任务调度的有效性，同时保持了临界区操作的安全。
		确保在调用 tTaskSchedule 之前对共享资源的所有访问和修改都已经完成，
		以避免潜在的竞态条件。
	*/
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  //手动触发一次中断	
}

