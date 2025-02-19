#include "stm32f10x.h"         
#include "stdio.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"

uint8_t Timmer_NumCount1 = 0;

void TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	

	TIM_TimeBaseStructure.TIM_Period = arr-1; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE );
 

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  
 
	TIM_Cmd(TIM4, ENABLE); 				 
}

uint32_t timeToCloseFan = 0;
uint8_t retVal1 = KEY_NONE;
uint8_t closeFanFlag = 0;
uint8_t timerCountDownCMD = 0;
void TIM4_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  
    {   
		//专门用于设置定时关闭风扇的按键
		retVal1 = timerKeyScan();
		if(retVal1 == timerkeyInfo.doubleKeyVal)
		{
			//...双击取消定时时间
			timerCountDownCMD = 0;
			closeFanFlag = 0;
		}
		else if(retVal1 == timerkeyInfo.longKeyVal)
		{
			//...长按增加定时时间
			timeToCloseFan += 50;		//每10ms增加0.5s -->> 每按1s增加20s，便于测试
		}
		else if(retVal1 == timerkeyInfo.shortKeyVal)
		{
			//...短按确定定时时间
			timerCountDownCMD = 1;
		}

		if (timerCountDownCMD == 1)		//如果开启倒计时
		{
			if(--timeToCloseFan == 0 )	//倒计时结束后，关闭风扇
			{
				closeFanFlag = 1;
			}
		}
		
		//单片机工作指示灯，每500ms闪烁一次
        Timmer_NumCount1++;
        if(Timmer_NumCount1 > 50)
        {
            LED0 = !LED0;
            Timmer_NumCount1 = 0;
        }  
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update ); 
    }
}



