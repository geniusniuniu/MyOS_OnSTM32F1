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
    
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; 			//设置成上拉输入	
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  
    
	GPIO_Init(GPIOA, &GPIO_InitStruct);//初始化GPIOA


}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//...以此类推
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
uint8_t GetKey_Value(uint8_t mode)
{	 
	static uint8_t key_up=1;//按键按松开标志
	if(mode)
        key_up=1;       //支持连按		  
	if(key_up && (KEY0==0 || KEY1==0 || KEY2==0 || KEY3==0 || KEY4==0))
	{
		tTaskDelay(2);//去抖动 
		key_up=0;
        if      (KEY0 == 0)   return KEY0_PRES;
		else if (KEY1 == 0)   return KEY1_PRES;
		else if (KEY2 == 0)   return KEY2_PRES;
		else if (KEY3 == 0)   return KEY3_PRES;
        else if (KEY4 == 0)   return KEY4_PRES;
	}
    else if(KEY0==1 && KEY1==1 && KEY2==1 && KEY3==1 && KEY4==1)
        key_up=1; 	    
 	return KEY_NOT_PRESS;// 无按键按下
}

uint8_t isTimerKeyInput(void)
{
	if(KEY5 == 0)
	{
		return KEY5_PRES;
	}
	return KEY_NOT_PRESS;
}

key_scan_t timerKey = {0};

//按键5支持短按，长按，连续按，双击四种功能
key_info_t timerkey_info = {KEY5_PRES, KEY5_SHORT, KEY5_LONG, KEY5_CNTINUS, KEY5_DOUBLE, 100};

static uint8_t isMistouchKey5(uint16_t new_input)	//按键消抖并防止误触
{	
    if (timerKey.cnt < SHORT_KEY_DELAY) 			//消抖
	{ 
        timerKey.cnt++;								// 按键按下计时变量+1	
    }
    if (timerKey.cnt == SHORT_KEY_DELAY)			//短按有效
	{
        timerKey.pressed = 1;						// 短按有效标志位
		timerKey.last_input = new_input;			//记录上次按键状态
    }
    return KEY5_PRES;
}

/**
 * @brief: 处于按下状态的处理：检测长按键,连续按是否有效
 * @param [in] new_input,当前的按键输入
 * @return 按键有效则返回相应键值,无效则返回KEY_NONE
 */
static uint8_t key_pressed_handle(uint16_t new_input)
{
    uint8_t res = KEY_NONE;
	timerKey.cnt++;
	if (new_input == timerKey.last_input) 
	{
		if (timerkey_info.long_cnt == timerKey.cnt) 	// 长按达到1秒钟
		{ 
			res = timerkey_info.long_key_val; 			// 长按键值
		} 
		else if (((timerKey.cnt - timerkey_info.long_cnt) % CNTINUS_KEY_DELAY) == 0) 
		{ 	
			if(timerKey.cnt > 400)						//时间超过4秒，长按无效
			{
				timerKey.waitLongTimeout = 1;	
				res = KEY_NONE;
			}
			res = timerkey_info.cntinus_key_val;		// 一直按,连续触发的键值

		}
	}
    return res;
}

static uint8_t key_release_handle(void)
{
    uint8_t res = KEY_NONE;

    if (timerKey.pressed == 1) 
	{
		if (timerKey.cnt < timerkey_info.long_cnt) 				// 按下的时长，小于长按判定时间
		{ 
			res = timerkey_info.short_key_val; 					// 短按键值.															
			if (KEY_NONE != timerkey_info.double_key_val) 		// 如果当前按键支持双击
			{ 
				if (timerKey.wait_double) 
				{
					timerKey.wait_double = 0; 					// 清除等待双击标志
					timerKey.double_timeout = 0;
					res = timerkey_info.double_key_val; 		// 双击键值
				} 
				else 
				{
					timerKey.wait_double = 1; 					// 设置等待双击标志
					timerKey.double_timeout = DOUBLE_KEY_DELAY; // 设置超时时间
					timerKey.wait_double_flag = 1;
					res = KEY_NONE;
				}
			}
		}

    }
    timerKey.cnt = 0;
    timerKey.pressed = 0;
    timerKey.last_input = KEY_NOT_PRESS;

    return res;
}

/**
 * @brief: 判定等待双击是否超时,超时则返回短按值
 * @param none.
 * @return 等待双击超时则返回短按键值,无效则返回KEY_NONE
 */
static uint8_t key_wait_double_timeout_handle(void)
{
    uint8_t res = KEY_NONE;

    timerKey.wait_double_flag = 0;
	if (timerKey.double_timeout) 					// 如果按键正在等待双击
	{	
		timerKey.double_timeout--;
		timerKey.wait_double_flag = 1;
		if (0 == timerKey.double_timeout) 			// 减到0的时刻,表示等待超时了
		{ 
			timerKey.wait_double = 0; 				// 清除等待双击标志
			return (timerkey_info.short_key_val); 	// 返回该键的短按值
		}
	}
    return res;
}

uint8_t timerKeyScan(void)
{
	uint8_t retVal = KEY_NONE;
	uint16_t key_input = 0xFF;						//开始按键5未按下
	timerKey.scan_flag = 0;
	key_input = isTimerKeyInput();					//检测到按键5输入
	if(KEY_NOT_PRESS != key_input) 
	{
        if (0 == timerKey.pressed) 					//按键5首次按下，判断是否误触
		{            
            isMistouchKey5(key_input);	
		} 
		else 										//按键5短按有效后
		{
			if(timerKey.waitLongTimeout == 0)
				retVal = key_pressed_handle(key_input);
			else									//长按超时处理
			{
				timerKey.pressed = 0;
				timerKey.cnt = 0;
				timerKey.last_input = KEY_NOT_PRESS;
				retVal = KEY_NONE;
			}
				
        }
    } 
	else 											//第二次按下按键判断
	{
        retVal = key_release_handle();
    }
    if (retVal == KEY_NONE) 						
	{ 
        if (timerKey.wait_double_flag) 				// 前面的处理没有产生有效键值,再处理双击超时
		{
            retVal = key_wait_double_timeout_handle();
        } 
    }
    return retVal;
}
