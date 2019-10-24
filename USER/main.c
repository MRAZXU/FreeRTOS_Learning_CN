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

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define MALLOC_TASK_PRIO	2
//�����ջ��С	
#define MALLOC_STK_SIZE 	32
//������
TaskHandle_t MallocTask_Handler;
//������
void malloc_task(void *p_arg);

//�������ȼ�
#define RUNTIMESTATS_TASK_PRIO	4
//�����ջ��С	
#define RUNTIMESTATS_STK_SIZE 	128  
//������
TaskHandle_t RunTimeStats_Handler;
//������
void RunTimeStats_task(void *pvParameters);

char InfoBuffer[1000];				//����������Ϣ������
char RunTimeInfo[400];		//������������ʱ����Ϣ
int main(void)
{ 


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED  
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ��  
	W25QXX_Init();				//��ʼ��W25Q128
	usmart_dev.init(168);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
  	f_mount(fs[0],"0:",1); 		//����SD��     
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	font_init();
	SD_Init();
	update_font(20,110,16,"0:");//�����ֿ�
	
	POINT_COLOR=BLUE;       
	Show_Str(30,20,200,16,"����ϵͳ����",24,0);	
	POINT_COLOR=BLACK; 
	Show_Str(30,100,200,16,"�ж�ʵ��",16,0);		
	Show_Str(30,120,200,16,"����ɾ�������𣬻ָ�ʵ��",16,0);		
	Show_Str(30,140,200,16,"����ʱ��Ƭ����",16,0);
	Show_Str(30,160,200,16,"����״̬ʱ��ͳ��",16,0);			
	Show_Str(30,180,200,16,"��������ʱ��ͳ��",16,0);
	Show_Str(30,200,200,16,"�б���Ĳ��������",16,0);	
	Show_Str(30,220,200,16,"���в���ʵ��",16,0);
	Show_Str(30,240,200,16,"�ź���ʵ��",16,0);
	Show_Str(30,260,200,16,"�����ʱ��ʵ��",16,0);
	Show_Str(30,280,200,16,"�¼���־��",16,0);	
	Show_Str(30,300,200,16,"�͹���ʵ��",16,0);	
	Show_Str(30,320,200,16,"����֪ͨʵ��",16,0);
	Show_Str(30,340,200,16,"��������ʵ��",16,0);
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����TASK1����
    xTaskCreate((TaskFunction_t )malloc_task,             
                (const char*    )"malloc_task",           
                (uint16_t       )MALLOC_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )MALLOC_TASK_PRIO,        
                (TaskHandle_t*  )&MallocTask_Handler); 

	//����RunTimeStats����
	xTaskCreate((TaskFunction_t )RunTimeStats_task,     
                (const char*    )"RunTimeStats_task",   
                (uint16_t       )RUNTIMESTATS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )RUNTIMESTATS_TASK_PRIO,
                (TaskHandle_t*  )&RunTimeStats_Handler); 
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//MALLOC������ 
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
			memset(RunTimeInfo,0,400);				//��Ϣ����������
			vTaskGetRunTimeStats(RunTimeInfo);		//��ȡ��������ʱ����Ϣ
			printf("������\t\t\t����ʱ��\t������ռ�ٷֱ�\r\n");
			printf("%s\r\n",RunTimeInfo);
		}
		vTaskDelay(10);                           	//��ʱ10ms��Ҳ����1000��ʱ�ӽ���	
	}
}
