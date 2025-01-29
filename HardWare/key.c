#include "stm32f10x.h"
#include "key.h"
						    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
    GPIO_InitTypeDef GPIO_InitStruct;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA,ENABLE);
    
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   
 	GPIO_Init(GPIOB,&GPIO_InitStruct);
    
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认下拉	
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  
    
	GPIO_Init(GPIOA, &GPIO_InitStruct);//初始化GPIOA.0


}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
uint8_t GetKey_Value(uint8_t mode)
{	 
	static uint8_t key_up=1;//按键按松开标志
	if(mode)
        key_up=1;       //支持连按		  
	if(key_up && (KEY0==0||KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		tTaskDelay(2);//去抖动 
		key_up=0;
        if      (KEY0==0)   return KEY0_PRES;
		else if (KEY1==0)   return KEY1_PRES;
		else if (KEY2==0)   return KEY2_PRES;
		else if (KEY3==0)   return KEY3_PRES;
        else if (KEY4==0)   return KEY4_PRES;
	}
    else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)
        key_up=1; 	    
 	return KEY_NOT_PRESS;// 无按键按下
}



#define WINDOW_SIZE 5

int window[WINDOW_SIZE];
int windowIndex = 0;

int filter(int value) 
{
    static int sum = 0;
    
    // 更新滑动窗口
    sum = sum - window[windowIndex] + value;
    window[windowIndex] = value;
    windowIndex = (windowIndex + 1) % WINDOW_SIZE;
    
    // 计算窗口内平均值
    int avg = sum / WINDOW_SIZE;
    
    return avg;
}

