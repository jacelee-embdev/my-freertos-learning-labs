// app_sys_orchestrator.c
#include "app_sys_orchestrator.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"
#include "led.h"
#include "beep.h"
#include "bluetooth.h"
#include "hcsr04.h"
#include "ia_sensor.h"
#include "key.h"
#include "light_sensor.h"
#include "oled.h"
#include "timer.h"
#include "string.h"
#include <stdio.h>

/* ---------------------------------------------------------------------------------- */
/* [配置参数区] 模块内部使用的宏定义与枚举                                            */
/* ---------------------------------------------------------------------------------- */
enum
{
    AUTO_MODE = 0,
    MANUL_MODE,
    REMOTE_MODE
};

typedef struct
{
    uint8_t person_flag;    //人体检测标志
    uint8_t mode;           //工作模式
    uint8_t distance;       //距离值
    uint8_t light_value;    //光照值
    uint8_t led_level;      //LED亮度级数
}SystemStatus_t;

//事件标志位
#define EVENT_DISTANCE_ALARM    (1 << 0)
#define EVENT_SITIME_ALARM      (1 << 1)

//警告预警值设置
#define ALAMR_DISTANCE_VALUE    5
#define ALAMR_TIME_VALUE        100


/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄与全局变量                              */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t      sensor_task_handle;
static TaskHandle_t      display_task_handle;
static TaskHandle_t      key_task_handle;
static TaskHandle_t      device_task_handle;
static TaskHandle_t      bluetooth_task_handle;

static SemaphoreHandle_t status_mutex;
static EventGroupHandle_t system_event_group;

static SystemStatus_t system_status;    //系统全局状态变量

/* ---------------------------------------------------------------------------------- */
/* [任务主体区] RTOS 任务逻辑实现                                                     */
/* ---------------------------------------------------------------------------------- */

static void sensor_monitor_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t person_flag_pre = FALSE;
    uint16_t distance_close_counter = 0; //用于计算“距离过近”持续时间的计数器

    while(1)
    {
        // 获取互斥量
        if(xSemaphoreTake(status_mutex, portMAX_DELAY) == pdTRUE)
        {
            // 人体检测是否变化
            system_status.person_flag = get_ia_sensor_flag();
            
            // ---------------- 1. 计时功能逻辑 ----------------
            // 边缘检测：有人来 或 人离开 的瞬间
            if(system_status.person_flag != person_flag_pre)
            {
                person_flag_pre = system_status.person_flag;
                
                if(system_status.person_flag == TRUE)
                {
                    timer_start(); // 检测到有人，开始计时
                }
                else
                {
                    // 检测到没人，清零并停止所有相关状态
                    system_status.distance = 0;
                    timer_stop();
                    set_sit_time(0);
                    
                    distance_close_counter = 0; // 清零距离计数器
                    xEventGroupClearBits(system_event_group, EVENT_DISTANCE_ALARM | EVENT_SITIME_ALARM);
                }
            }
            
            // ---------------- 2. 报警触发逻辑 ----------------
            // 只有在检测到有人的状态下，才进行距离和久坐的判断
            if(system_status.person_flag == TRUE)
            {
                // 获取光照强度
                system_status.light_value = light_sensor_get_value();
                
                // 获取距离的值
                system_status.distance = get_length_from_hcsr04();
                
                // (a) 距离报警：持续小于 5 厘米并超过 5 秒钟
                if(system_status.distance < ALAMR_DISTANCE_VALUE)
                {
                    distance_close_counter++;
                    
                    // 任务周期为 20ms，5秒钟 = 5000ms / 20ms = 250 次循环
                    if(distance_close_counter >= 250)
                    {
                        xEventGroupSetBits(system_event_group, EVENT_DISTANCE_ALARM);
                        distance_close_counter = 250; // 锁定数值，防止长时间过近导致整型溢出
                    }
                }
                else
                {
                    // 一旦距离拉开(大于等于5cm)，立刻清零计数器并解除距离报警
                    distance_close_counter = 0;
                    xEventGroupClearBits(system_event_group, EVENT_DISTANCE_ALARM);
                }
                
                // (b) 久坐报警：超过设定的时间阈值
                if(get_sit_time() >= ALAMR_TIME_VALUE)
                {
                    xEventGroupSetBits(system_event_group, EVENT_SITIME_ALARM);
                }
                else
                {
                    xEventGroupClearBits(system_event_group, EVENT_SITIME_ALARM);
                }
            }
            
            // 释放互斥量
            xSemaphoreGive(status_mutex);
        }
        
        // 维持 20ms 的任务调度周期
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static void display_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t dis_str[8] = {0};
    uint8_t sit_time_str[8] = {0};
    
    while(1)
    {
        //获取互斥量
        if(xSemaphoreTake(status_mutex,portMAX_DELAY) == pdTRUE)
        {
            //显示人体检测状态
            if(system_status.person_flag == TRUE)
            {
                oled_show_Chinese(10,4,9);   //显示有人
            }
            else
            {
                oled_show_Chinese(10,4,10);   //显示无人
            }
            
            //显示距离
            sprintf((char *)dis_str,"%3d",system_status.distance);
            oled_show_string(60,4,(char *)dis_str,16);
            
            //显示久坐时间
            uint32_t raw_time = get_sit_time(); 
            sprintf((char *)sit_time_str, "%2d.%d", raw_time / 10, raw_time % 10);
            oled_show_string(60,6,(char *)sit_time_str,16);
            
            //显示当前模式
            switch(system_status.mode)
            {
                case AUTO_MODE:
                    oled_show_Chinese(60,0,3);   //智
                    oled_show_Chinese(75,0,4);   //能
                    break;
                case MANUL_MODE:
                    oled_show_Chinese(60,0,5);   //按
                    oled_show_Chinese(75,0,6);   //键
                    break;
                case REMOTE_MODE:
                    oled_show_Chinese(60,0,7);   //远
                    oled_show_Chinese(75,0,8);   //程
                    break;
                default:
                    break;
            }
            //释放互斥量
            xSemaphoreGive(status_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


static void key_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t key_num = 0;

    while(1)
    {
        key_num = KeyScan();
        if(key_num != 0)
        {
            //获取互斥量
            if(xSemaphoreTake(status_mutex,portMAX_DELAY) == pdTRUE)
            {
                //处理按键1:切换工作模式
                if(key_num == 1)
                {
                    system_status.mode++;
                    if(system_status.mode > 2)
                    {
                        system_status.mode = 0;
                    }
                }
                
                //处理按键2:按键模式下调整亮度
                else if(key_num == 2 && system_status.mode == MANUL_MODE)
                {
                    system_status.led_level = led_level_get();
                    //关~低~中~高
                    system_status.led_level++;
                    if(system_status.led_level > 3)
                    {
                        system_status.led_level = 0;
                    }
                    
                    switch(system_status.led_level)
                    {
                        case 0:
                            led_off();
                            break;
                        case 1:
                            led_low();
                            break;
                        case 2:
                            led_medium();
                            break;
                        case 3:
                            led_high();
                            break;
                        default:
                            break;
                    }
                    
                }
                
                //处理按键3:暂停/开始计时
                else if(key_num == 3)
                {
                    timer_toggle();
                }
                
                //处理按键4:清零计时
                else if(key_num == 4)
                {
                    timer_stop();
                    set_sit_time(0);
                    xEventGroupClearBits(system_event_group,EVENT_SITIME_ALARM);
                }
                
                //释放互斥量
                xSemaphoreGive(status_mutex);
            }
            
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void device_task(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t event_bits;
    
    while(1)
    {
        //获取互斥量
       event_bits = xEventGroupGetBits(system_event_group);
        if(xSemaphoreTake(status_mutex,portMAX_DELAY) == pdTRUE)
        {
            //处理蜂鸣器报警
            if(event_bits & (EVENT_DISTANCE_ALARM | EVENT_SITIME_ALARM))
            {
                beep_on();
            }
            else
            {
                beep_off();
            }
            
            //根据不同模式控制LED
            if(system_status.mode == AUTO_MODE)
            {
                if(system_status.person_flag == TRUE)
                {
                    if(system_status.light_value < 40)
                    {
                        led_high();
                    }
                    else if(system_status.light_value >= 40 && system_status.light_value <= 70)
                    {
                        led_medium();
                    }
                    else if(system_status.light_value > 70)
                    {
                        led_low();
                    }
                    else if(system_status.light_value < 40)
                    {
                        led_low();
                    }
                }
                else
                {
                    led_off();
                }
            }
            //释放互斥量
            xSemaphoreGive(status_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static void bluetooth_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t bt_cmd;
    
    while(1)
    {
        //获取互斥量
        if(xSemaphoreTake(status_mutex,portMAX_DELAY) == pdTRUE)
        {
            //接收数据
            if(system_status.mode == REMOTE_MODE)
            {
               bt_cmd = bt_rx_get() - '0';
                
                //根据命令执行操作
                switch(bt_cmd)
                {
                    case 0:
                        led_off();  //关灯
                        break;
                    case 1:
                        led_low();  //低亮度
                        break;
                    case 2:
                        led_medium();  //中亮
                        break;
                    case 3:
                        led_high(); //高亮
                        break;
                    case 4:
                        timer_start();//开始计时
                        break;
                    case 5:
                        timer_stop();//暂停计时
                        break;
                    case 6:
                        timer_stop();   //清零计时
                        set_sit_time(0);
                        xEventGroupClearBits(system_event_group,EVENT_DISTANCE_ALARM);
                        break;
            }
            }
            
            //释放互斥量
            xSemaphoreGive(status_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统核心编排器 (统筹创建各个子任务与 OS 资源)
void app_sys_orchestrator_init(void)
{
    //创建互斥量
    status_mutex = xSemaphoreCreateMutex();
    
    //创建事件标志组
    system_event_group = xEventGroupCreate();
    
    //初始化相关信息
    system_status.person_flag = 0;
    system_status.mode = AUTO_MODE;
    system_status.distance = 0;
    system_status.light_value = 0;
    system_status.led_level = 0;
    
    // 实例化系统任务群
    xTaskCreate(sensor_monitor_task,
                "sensor_task",
                128,
                NULL,
                1,
                &sensor_task_handle);
    
    xTaskCreate(display_task,
                "display_task",
                512,
                NULL,
                1,
                &display_task_handle);
    
    xTaskCreate(key_task,
                "key_task",
                128,
                NULL,
                1,
                &key_task_handle);
     
    xTaskCreate(device_task,
                "device_task",
                128,
                NULL,
                1,
                &device_task_handle);
     
    xTaskCreate(bluetooth_task,
                "bluetooth_task",
                128,
                NULL,
                1,
                &bluetooth_task_handle);
}
