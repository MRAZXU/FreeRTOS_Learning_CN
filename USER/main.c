#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "malloc.h" 
#include "semphr.h"
#include "timers.h"
#include "limits.h"
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"
#include "usmart.h"  
#include "sdio_sdcard.h" 

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define MALLOC_TASK_PRIO	2
//任务堆栈大小	
#define MALLOC_STK_SIZE 	32
//任务句柄
TaskHandle_t MallocTask_Handler;
//任务函数
void malloc_task(void *p_arg);

//任务优先级
#define RUNTIMESTATS_TASK_PRIO	4
//任务堆栈大小	
#define RUNTIMESTATS_STK_SIZE 	128  
//任务句柄
TaskHandle_t RunTimeStats_Handler;
//任务函数
void RunTimeStats_task(void *pvParameters);

char InfoBuffer[1000];				//保存任务信息的数组
char RunTimeInfo[400];		//保存任务运行时间信息
int main(void)
{ 


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化  
	W25QXX_Init();				//初始化W25Q128
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡     
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	font_init();
	SD_Init();
	update_font(20,110,16,"0:");//更新字库
	
	POINT_COLOR=BLUE;       
	Show_Str(30,20,200,16,"操作系统测试",24,0);	
	POINT_COLOR=BLACK; 
	Show_Str(30,100,200,16,"中断实验",16,0);		
	Show_Str(30,120,200,16,"任务删除，挂起，恢复实验",16,0);		
	Show_Str(30,140,200,16,"任务时间片调度",16,0);
	Show_Str(30,160,200,16,"任务状态时间统计",16,0);			
	Show_Str(30,180,200,16,"任务运行时间统计",16,0);
	Show_Str(30,200,200,16,"列表项的插入与挂起",16,0);	
	Show_Str(30,220,200,16,"队列操作实验",16,0);
	Show_Str(30,240,200,16,"信号量实验",16,0);
	Show_Str(30,260,200,16,"软件定时器实验",16,0);
	Show_Str(30,280,200,16,"事件标志组",16,0);	
	Show_Str(30,300,200,16,"低功耗实验",16,0);	
	Show_Str(30,320,200,16,"任务通知实验",16,0);
	Show_Str(30,340,200,16,"空闲任务实验",16,0);
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )malloc_task,             
                (const char*    )"malloc_task",           
                (uint16_t       )MALLOC_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )MALLOC_TASK_PRIO,        
                (TaskHandle_t*  )&MallocTask_Handler); 

	//创建RunTimeStats任务
	xTaskCreate((TaskFunction_t )RunTimeStats_task,     
                (const char*    )"RunTimeStats_task",   
                (uint16_t       )RUNTIMESTATS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )RUNTIMESTATS_TASK_PRIO,
                (TaskHandle_t*  )&RunTimeStats_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//MALLOC任务函数 
void malloc_task(void *pvParameters)
{
	while(1)
	{
	LED0=!LED0;
	
	}
} 
void RunTimeStats_task(void *pvParameters)
{
	u8 key=0;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			memset(RunTimeInfo,0,400);				//信息缓冲区清零
			vTaskGetRunTimeStats(RunTimeInfo);		//获取任务运行时间信息
			printf("任务名\t\t\t运行时间\t运行所占百分比\r\n");
			printf("%s\r\n",RunTimeInfo);
		}
		vTaskDelay(10);                           	//延时10ms，也就是1000个时钟节拍	
	}
}
