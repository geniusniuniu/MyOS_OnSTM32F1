#include "stm32f10x.h"                  // Device header  
#include "stdio.h"
#include "LED.h"
#include "Timer.h"
#include "PWM.h"
#include "Servo.h"

uint8_t Timmer_NumCount1 = 0;

void TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = arr-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断
 
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
 
	TIM_Cmd(TIM4, ENABLE);  //使能TIMx					 
}

//定时器4中断服务程序
void TIM4_IRQHandler(void)   //TIM4中断
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
    {   
//        case 1: Servo_SetCompare(1700);break;  //中值
//        case 2: Servo_SetCompare(1550);break;//左
//        case 3: Servo_SetCompare(1850);break;
//        case 4: Servo_SetCompare(1280);break;          
//        case 5: Servo_SetCompare(2120);break;     
                          
        Timmer_NumCount1++;
        //闪灯,判断程序是否运行
        if(Timmer_NumCount1 > 50)
        {
            LED0 = !LED0;
            Timmer_NumCount1 = 0;
        }  
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update );  //清除TIMx更新中断标志 
    }
}



