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
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�
 
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
 
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx					 
}

//��ʱ��4�жϷ������
void TIM4_IRQHandler(void)   //TIM4�ж�
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
    {   
//        case 1: Servo_SetCompare(1700);break;  //��ֵ
//        case 2: Servo_SetCompare(1550);break;//��
//        case 3: Servo_SetCompare(1850);break;
//        case 4: Servo_SetCompare(1280);break;          
//        case 5: Servo_SetCompare(2120);break;     
                          
        Timmer_NumCount1++;
        //����,�жϳ����Ƿ�����
        if(Timmer_NumCount1 > 50)
        {
            LED0 = !LED0;
            Timmer_NumCount1 = 0;
        }  
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update );  //���TIMx�����жϱ�־ 
    }
}



