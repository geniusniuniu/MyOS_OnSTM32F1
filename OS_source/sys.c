#include "stm32f10x.h"                  // Device header
#include "tinyOS.h"


void SysTick_Handler(void)
{
	tTaskSystemTickHandler();
}

void SysTickInit(uint32_t ms)	//系统时钟初始化
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;	//SystemCoreClock / 1000将时钟每秒的频率转换为每毫秒的频率
	NVIC_SetPriority(SysTick_IRQn,(1 << __NVIC_PRIO_BITS)-1);	//将 SysTick 的优先级设置为最低,设置为 7（111）
	SysTick->VAL  = 0;			//计数寄存器清零
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
					SysTick_CTRL_TICKINT_Msk |
					SysTick_CTRL_ENABLE_Msk;
}


void delay(int ms)
{
	while(--ms);
}




uint32_t power(uint32_t x,uint32_t y)
{
		uint32_t mul = 1;

		uint32_t i;
		for( i = 0; i < y; i++)
		{
				mul = mul * x;
		}
		return mul;
}

uint32_t log_2(uint32_t x)
{
		uint8_t i=0;
		while(1)
		{
				if(power(2,i) == x)
					return i;
				else
					i++;
		}
}



