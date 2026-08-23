#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "lcd1602.h"
#include "beep.h"
#include "relay_gate.h"
#include "string.h"
#include "exti.h"
#include <stdio.h>

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t sensor_task_handle        = NULL;     // LED1 任务句柄
static TaskHandle_t device_task_handle        = NULL;     // LED2 任务句柄
static TaskHandle_t dispaly_task_handle       = NULL;     // 按键监控任务句柄

static QueueHandle_t display_queue; //显示消息队列句柄
static QueueHandle_t state_queue; //状态队列句柄

static TimerHandle_t wait_timer;    //等待定时器句柄

static SemaphoreHandle_t lcd_mutex; //LCD互斥锁
static SemaphoreHandle_t passage_semaphore; //通行权信号量

#define NOTIFY_WAIT_TIMEOUT 0x01 //等待超时通知
#define NOTIFY_PERSON_PASS 0x02 // 有人通过通知

#define MAX_PESSAGERS 5 //最大单次通行人数

//状态定义
enum
{
    PASS_STATE,
    WAIT_STATE
};


//显示消息类型定义
typedef struct
{
    uint8_t row;
    uint8_t col;
    char messsge[16];
}DisplayMessage_t;

uint8_t system_state = PASS_STATE;

/* ---------------------------------------------------------------------------------- */
/* [任务主体] RTOS 任务逻辑实现                                                       */
/* ---------------------------------------------------------------------------------- */
static void sensor_moniter_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t current_state;
    uint8_t avaiable_passages = MAX_PESSAGERS;
    uint32_t notification_val;
    DisplayMessage_t display_message;

    //初始状态
    system_state = PASS_STATE;
    
    //发送初始状态到队列
    xQueueSend(state_queue,&system_state,0);
    
    //初始显示
    sprintf(display_message.messsge,"PASS...00/05");
    display_message.row = 1;
    display_message.col = 1;
    xQueueSend(display_queue,&display_message,0);
    while(1)
    {
        //获取当前状态
        current_state = system_state;
        
        //检测红外传感器
        if(current_state == PASS_STATE && get_infrared_sensor_flag() == TRUE)
        {
            set_infrared_sensor_flag(FALSE);//清除标志
            
            //尝试获取一个通行权
            if(xSemaphoreTake(passage_semaphore,0) == pdTRUE)
            {
                avaiable_passages--;
                //更新显示
                sprintf(display_message.messsge,"PASS...%02d/05",MAX_PESSAGERS - avaiable_passages);
                display_message.row = 1;
                display_message.col = 1;
                xQueueSend(display_queue,&display_message,0);
                
                //检查是否已无可用通行权
                if(avaiable_passages == 0)
                {
                    //如果用完,如用完则切换到不允许通行状态
                    system_state = WAIT_STATE;
                    xQueueSend(state_queue,&system_state,0);
                    
                    //更新显示
                    sprintf(display_message.messsge,"WAIT...00/05");
                    display_message.row = 1;
                    display_message.col = 1;
                    xQueueSend(display_queue,&display_message,0);
                }
            }
        }

        
        //等待任务通知
        if(xTaskNotifyWait(0,0xFFFFFFFF,&notification_val,0) == pdTRUE)
        {
            //处理等待超时通知
            if((notification_val & NOTIFY_WAIT_TIMEOUT) != 0)
            {
                if(current_state == WAIT_STATE)
                {
                    //重置信号量,恢复通信权
                    avaiable_passages = MAX_PESSAGERS;
                    
                    //确保信号量为空
                    while(xSemaphoreTake(passage_semaphore,0) == pdTRUE);
                    
                    //再给5个信号量
                    for(int i=0;i<MAX_PESSAGERS;i++)
                    {
                        xSemaphoreGive(passage_semaphore);
                    }
                    
                    set_infrared_sensor_flag(FALSE);
                    
                    //切换状态
                    system_state =PASS_STATE;
                    xQueueSend(state_queue,&system_state,0);
                    
                    //更新显示
                    sprintf(display_message.messsge,"PASS...00/05");
                    display_message.row = 1;
                    display_message.col = 1;
                    xQueueSend(display_queue,&display_message,0);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static void device_control_task(void *pvParameters)
{
    (void)pvParameters;
    uint16_t cnt = 0;

    uint8_t current_state = PASS_STATE;
    while(1)
    {
        //从状态队列获取最新状态
        if(xQueueReceive(state_queue,&current_state,0) == pdPASS)
        {
            //状态变更
            if(current_state == WAIT_STATE)
            {
                xTimerStart(wait_timer,0);
            }
            else if(current_state == PASS_STATE)
            {
                xTimerStop(wait_timer,0);
            }
            
            cnt =100;
        }
        
        //根据状态控制外设
        if(current_state == PASS_STATE)
        {
            if(cnt >= 100)
            {                                                                                                                                                   
                //LED1以1s频率闪烁
                led1_toggle();
                //LED2不闪烁
                led2_off();
                //蜂鸣器不响
                beep_off();
                //开门
                relay_gate_on();
                
                cnt = 0;
            }
            else
            {
                cnt++;
            }
            
            
        }
        else if(current_state == WAIT_STATE)
        {
            if(cnt >= 20)
            {
                //LED1不闪烁
                led1_off();
                //LED2以200ms频率闪烁
                led2_toggle();
                //蜂鸣器响
                beep_on();
                //关门
                relay_gate_off();
                
                cnt = 0;
            }
            else
            {
                cnt++;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void dispaly_task(void *pvParameters)
{
    (void)pvParameters;

    DisplayMessage_t display_message;
    while(1)
    {
        //从队列接收显示信息
        if(xQueueReceive(display_queue,&display_message,portMAX_DELAY) == pdPASS)
        {
            //获取LCD互斥锁
            if(xSemaphoreTake(lcd_mutex,portMAX_DELAY) == pdTRUE)
            {
                //显示消息到LCD
                lcd1602_show_char_string(display_message.row,display_message.col,display_message.messsge);
                
                //释放互斥锁
                xSemaphoreGive(lcd_mutex);
            }
        }
    }
}

void wait_timer_callback(TimerHandle_t xTimer)
{
    //使用任务通知直接通知传感器任务
    if(sensor_task_handle != NULL)
    //发送任务通知值
    xTaskNotify(sensor_task_handle,NOTIFY_WAIT_TIMEOUT,eSetBits);
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务 (统筹创建各个子任务)
void app_freertos_test_init(void)
{
    //创建队列
    state_queue = xQueueCreate(3,sizeof(uint8_t));
    display_queue = xQueueCreate(3,sizeof(DisplayMessage_t));
    
    //创建互斥锁
    lcd_mutex = xSemaphoreCreateMutex();
    
    //创建计数型信号量
    passage_semaphore = xSemaphoreCreateCounting(MAX_PESSAGERS,MAX_PESSAGERS); //最大值为5,初始值为5
    
    //创建软件定时器
    wait_timer = xTimerCreate(
    "wait_timer",
    3000,
    pdFALSE,
    (void *)0,
    &wait_timer_callback);
    
    xTaskCreate(
        sensor_moniter_task,
        "sensor_task",
        128,
        NULL,
        1,
        &sensor_task_handle
    );
    
    xTaskCreate(
        device_control_task,
        "control_task",
        128,
        NULL,
        1,
        &device_task_handle
    );
    
    xTaskCreate(
        dispaly_task,
        "dispaly_task",
        128,
        NULL,
        1,
        &dispaly_task_handle
    );
}
