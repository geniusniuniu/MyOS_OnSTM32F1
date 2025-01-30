#include "stm32f10x.h"                  // Device header
#include "tinyOS.h"
#include "LED.h"
#include "OLED.h"
#include "Key.h"
#include "Timer.h"
#include "usart.h"
#include "PWM.h"
#include "Servo.h"

int Servo_compare = 1200,Motor_compare = 0;
uint8_t Key_Value = 0xff;
uint8_t detectFlag = 0xff;

int On_OFF = 0;
int Motor_compare_last;

//创建消息邮箱
tMsgBox keyMsgBox;
void * keyMsgBoxBuffer[5];

//初始化应用层任务
void applicationTaskInit(void);
void hardWareInit(void); 

void OLED_ShowStatic(void);
void OLED_ShowDynamic(void);


//创建任务结构体
tTask oledShowTask;     //OLED显示任务
tTask keyDetectTask;    //按键检测任务
tTask serialParsing;    //串口解析任务

//创建任务的堆栈空间
tTaskStack oledShowTaskEnv[128];
tTaskStack keyDetectTaskEnv[128];
tTaskStack serialParsingEnv[128];


//创建空闲任务结构体和堆栈空间
tTask tTaskIdle;
tTaskStack taskIdleEnv[TINYOS_IDLETASK_STACK_SIZE];

//创建空闲任务
void taskIdleEntry(void * param)
{
	tTaskSchedLockEnable();	//使能调度锁
	
    SysTickInit(TINYOS_SYSTICK_MS);	
	
	#if TINYOS_ENABLE_SOFTTIMER == 1
		tTimerModuleInit();
	#endif
	
	applicationTaskInit();
	
	#if TINYOS_ENABLE_CPUSTATE == 1
		cpuUsagSyncWithSystick();	
	#else
		// 开启调度器，允许切换到其它任务
		tTaskSchedLockDisable();
	#endif
	
	for(;;)
	{
		uint32_t status = tTaskEnterCritical();
		{
			idleCount++;
		}
		tTaskExitCritical(status); 
		
		#if TINYOS_ENABLE_HOOKS == 1
			tHooksCpuIdle();
		#endif
	}
}

//创建任务
void oledShowTaskEntry(void * param)
{	
    for (;;) 
    {		
		;;
    }
}

void keyDetectTaskEntry(void* param)
{
    tMsgBoxInit(&keyMsgBox,(void *)keyMsgBoxBuffer,5);
    for (;;) 
    {	
        Key_Value = GetKey_Value(0);
        if(Key_Value != 255)
        {
			//将Key_Value的值作为消息发送出去
           tMsgNotify(&keyMsgBox,&Key_Value,tMSGNORMAL); 
        }
        if((Key_Value == KEY0_PRES) && (On_OFF == 0)) //按键0风扇启动
        {
            On_OFF = 1;
            Servo_compare = Servo_SetCompare(1200);//舵机归中
            Motor_compare = Motor_SetCompare(2600);//风扇初档位  
            Motor_compare_last = Motor_compare;
        }
        else if(Motor_compare > 0)
        {
            if(Key_Value == KEY1_PRES)//按键1每按一次风力加强
            {
                Motor_compare += 200;
                Motor_compare = Motor_SetCompare(Motor_compare); 
                Motor_compare_last = Motor_compare;
              
            }
            else if(Key_Value == KEY2_PRES)//按键2每按一次风力减弱
            {
                Motor_compare -= 200;
                Motor_compare = Motor_SetCompare(Motor_compare);
                Motor_compare_last = Motor_compare; 
            }
            else if(Key_Value == KEY3_PRES)//按键3舵机转向
            {
                Servo_compare += 100;
                Servo_compare = Servo_SetCompare(Servo_compare); 
            }
            else if(Key_Value == KEY4_PRES)//按键4舵机转向
            {
                Servo_compare -= 100;
                Servo_compare = Servo_SetCompare(Servo_compare); 
            }
            else if(On_OFF == 1 && Key_Value == KEY0_PRES ) //风扇转动时，按下按键0关闭风扇
            {
                On_OFF = 0;
                Motor_compare = Motor_SetCompare(0);    //关闭风扇
                Servo_compare = Servo_SetCompare(1200); //舵机归中 
                Motor_compare_last = Motor_compare;                
            }
        }                        
        OLED_ShowDynamic();
    }
}


void serialParsingEntry(void* param)
{
	for (;;) 
    {	
		void * msg;
        uint32_t retVal = tMsgWait(&keyMsgBox,&msg,3);//超时等待30ms
		if(retVal == tErrorCodeNone)
		{
			uint32_t value = *(uint32_t *)msg;	//将收到的消息强制转换为uint32_t*类型，并取值
			printf("%d\r\n",value);
		}
		
		if (Serial_RxFlag == 1)
		{			
			if (strcmp(Serial_RxPacket, "People") == 0)
			{
				detectFlag = 1;
                Motor_compare = Motor_SetCompare(Motor_compare_last); //风扇保持之前档位  
			}
			else if (strcmp(Serial_RxPacket, "None") == 0)
			{
				detectFlag = 2;
                Motor_compare = Motor_SetCompare(0);    //风扇停转  
			}
			else
			{
                detectFlag = 0;
			}			
			Serial_RxFlag = 0;
		}
		else
        {
            detectFlag = 0xff;
		}
		tTaskDelay(5);
    }
}



//初始化应用层任务
void applicationTaskInit(void)
{
	tTaskInit(&oledShowTask,  oledShowTaskEntry,  (void *)0, 2, oledShowTaskEnv, sizeof(oledShowTaskEnv));
    tTaskInit(&keyDetectTask, keyDetectTaskEntry, (void *)0, 0, keyDetectTaskEnv,sizeof(keyDetectTaskEnv));
    tTaskInit(&serialParsing, serialParsingEntry, (void *)0, 1, serialParsingEnv,sizeof(serialParsingEnv));
}

void hardWareInit(void)    
{
    //中断向量分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    //指示灯初始化
    LED_Init();  
    
    //检测模块初始化
    Detect_Init();
    //按键初始化
    KEY_Init();
	
    //电机方向引脚初始化
    Motor_Dir_Init(Bit_RESET);
    //电机初始化
    Motor_PWM_Init(7200,10);
    
    //OLED初始化
    OLED_Init();
	OLED_ShowStatic();
	
    //串口初始化波特率9600
    Serial_Init(9600);
    
    //舵机初始化
    Servo_Init();
    
    //定时器4初始化，10ms中断一次
    TIM4_Int_Init(7200,100);  	       
}

int main(void)
{
    hardWareInit();
	tTaskSchedInit();
	tListInit(&taskDelayList);
	#if TINYOS_ENABLE_CPUSTATE == 1
		initCpuUsageState();
	#endif
	tTaskInit(&tTaskIdle,taskIdleEntry,(void*)0,TINYOS_PIORITY_MAX-1,taskIdleEnv,TINYOS_IDLETASK_STACK_SIZE);
	idleTask = &tTaskIdle;

	nextTask = tTaskGetReady();	//首次运行第一个任务

	tTaskRunFirst();
	/**************** 后面的代码执行不到 *****************/

	return 0;
}	

void OLED_ShowStatic(void)
{
    OLED_ShowString(1, 1, "ON/OFF");
    OLED_ShowString(1, 10,"Key:");
    OLED_ShowString(2, 1, "Servo:");   
    OLED_ShowString(3, 1, "Motor:"); 
	
	OLED_ShowString(1,8,"0");
    OLED_ShowString(1,14,"255");
    OLED_ShowString(2,8,"1200");
    OLED_ShowString(3,8,"0000");
}

void OLED_ShowDynamic(void)
{
    OLED_ShowNum(1,8,On_OFF , 1);
    OLED_ShowNum(1,14,Key_Value , 3);
    OLED_ShowNum(2,8,Servo_compare , 4);
    OLED_ShowNum(3,8,Motor_compare , 4);
	if(detectFlag == 1)
	{
		OLED_ShowString(4, 1, "               ");
		OLED_ShowString(4, 1, "DETECT_PEOPLE");
	}
	else if(detectFlag == 2)
	{
		OLED_ShowString(4, 1, "               ");
		OLED_ShowString(4, 1, "NO_PEOPLE_HERE");
	}
	else
	{
		OLED_ShowString(4, 1, "  Genius_NN   ");
	}
}




