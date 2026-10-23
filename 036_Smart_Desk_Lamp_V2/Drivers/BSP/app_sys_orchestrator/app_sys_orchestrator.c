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
/* [配置参数区] 业务逻辑参数与阈值定义                                                */
/* ---------------------------------------------------------------------------------- */

#define ALARM_DISTANCE_THRESHOLD    5           // 距离报警阈值 (cm)
#define ALARM_SIT_TIME_THRESHOLD    100         // 久坐报警时间阈值
#define DISTANCE_FILTER_TICKS       250         // 距离过近滤波计数 (20ms * 250 = 5000ms)

#define EVENT_DISTANCE_ALARM        (1 << 0)    // 距离过近报警事件
#define EVENT_SIT_TIME_ALARM        (1 << 1)    // 久坐超时报警事件

enum
{
    AUTO_MODE = 0,      // 自动模式：根据光照与人体自动调光
    MANUAL_MODE,        // 手动模式：按键手动调整亮度等级
    REMOTE_MODE         // 远程模式：蓝牙接管设备控制权
};

typedef struct
{
    uint8_t person_flag;    // 人体存在状态 (TRUE/FALSE)
    uint8_t mode;           // 当前设备运行模式
    uint8_t distance;       // 实时测距距离 (cm)
    uint8_t light_value;    // 环境光照强度
    uint8_t led_level;      // 手动模式下的 LED 亮度等级
} SystemStatus_t;

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄与全局状态                              */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t         sensor_task_handle;
static TaskHandle_t         display_task_handle;
static TaskHandle_t         key_task_handle;
static TaskHandle_t         device_task_handle;
static TaskHandle_t         bluetooth_task_handle;

static SemaphoreHandle_t    status_mutex;           // 保护 g_sys_status 全局状态的互斥锁
static EventGroupHandle_t   system_event_group;     // 系统级报警与通知事件组

static SystemStatus_t       g_sys_status;           // 系统全局状态机数据载体

/* ---------------------------------------------------------------------------------- */
/* [任务主体区] 系统核心业务任务实现                                                  */
/* ---------------------------------------------------------------------------------- */

static void sensor_monitor_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t  prev_person_flag       = FALSE;
    uint16_t distance_close_counter = 0;

    while(1)
    {
        if(xSemaphoreTake(status_mutex, portMAX_DELAY) == pdTRUE)
        {
            g_sys_status.person_flag = get_ia_sensor_flag();
            
            /* 阶段 1: 边缘检测与计时器控制 
             * 捕获人员靠近与离开的瞬间状态跳变，启停本地计时器并重置状态防错
             */
            if(g_sys_status.person_flag != prev_person_flag)
            {
                prev_person_flag = g_sys_status.person_flag;
                
                if(g_sys_status.person_flag == TRUE)
                {
                    timer_start(); 
                }
                else
                {
                    g_sys_status.distance = 0;
                    timer_stop();
                    set_sit_time(0);
                    distance_close_counter = 0; 
                    xEventGroupClearBits(system_event_group, EVENT_DISTANCE_ALARM | EVENT_SIT_TIME_ALARM);
                }
            }
            
            /* 阶段 2: 状态数据采集与报警判定
             * 仅在人员存在期间更新外设传感器数据，避免无效读取阻塞总线
             */
            if(g_sys_status.person_flag == TRUE)
            {
                g_sys_status.light_value = light_sensor_get_value();
                g_sys_status.distance    = get_length_from_hcsr04();
                
                // 持续距离滤波判定：防止偶发测距跳动触发误报
                if(g_sys_status.distance < ALARM_DISTANCE_THRESHOLD)
                {
                    distance_close_counter++;
                    if(distance_close_counter >= DISTANCE_FILTER_TICKS)
                    {
                        xEventGroupSetBits(system_event_group, EVENT_DISTANCE_ALARM);
                        distance_close_counter = DISTANCE_FILTER_TICKS; // 截断计数，防止数值溢出
                    }
                }
                else
                {
                    distance_close_counter = 0;
                    xEventGroupClearBits(system_event_group, EVENT_DISTANCE_ALARM);
                }
                
                // 久坐超时判定
                if(get_sit_time() >= ALARM_SIT_TIME_THRESHOLD)
                {
                    xEventGroupSetBits(system_event_group, EVENT_SIT_TIME_ALARM);
                }
                else
                {
                    xEventGroupClearBits(system_event_group, EVENT_SIT_TIME_ALARM);
                }
            }
            xSemaphoreGive(status_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static void display_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t dis_str[8]      = {0};
    uint8_t sit_time_str[8] = {0};
    
    while(1)
    {
        if(xSemaphoreTake(status_mutex, portMAX_DELAY) == pdTRUE)
        {
            if(g_sys_status.person_flag == TRUE)
            {
                oled_show_Chinese(10, 4, 9);
            }
            else
            {
                oled_show_Chinese(10, 4, 10);
            }
            
            sprintf((char *)dis_str, "%3d", g_sys_status.distance);
            oled_show_string(60, 4, (char *)dis_str, 16);
            
            uint32_t raw_time = get_sit_time(); 
            sprintf((char *)sit_time_str, "%2d.%d", raw_time / 10, raw_time % 10);
            oled_show_string(60, 6, (char *)sit_time_str, 16);
            
            switch(g_sys_status.mode)
            {
                case AUTO_MODE:
                    oled_show_Chinese(60, 0, 3);
                    oled_show_Chinese(75, 0, 4);
                    break;
                case MANUAL_MODE:
                    oled_show_Chinese(60, 0, 5);
                    oled_show_Chinese(75, 0, 6);
                    break;
                case REMOTE_MODE:
                    oled_show_Chinese(60, 0, 7);
                    oled_show_Chinese(75, 0, 8);
                    break;
                default:
                    break;
            }
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
            if(xSemaphoreTake(status_mutex, portMAX_DELAY) == pdTRUE)
            {
                /* 阶段 1: 模式轮转切换
                 * 按键 1：在自动、手动、远程模式间循环切换
                 */
                if(key_num == 1)
                {
                    g_sys_status.mode++;
                    if(g_sys_status.mode > REMOTE_MODE)
                    {
                        g_sys_status.mode = AUTO_MODE;
                    }
                }
                
                /* 阶段 2: 手动光强调节
                 * 按键 2：仅在手动模式下响应，循环切换 LED 亮度等级
                 */
                else if(key_num == 2 && g_sys_status.mode == MANUAL_MODE)
                {
                    g_sys_status.led_level = led_level_get();
                    g_sys_status.led_level++;
                    
                    if(g_sys_status.led_level > 3)
                    {
                        g_sys_status.led_level = 0;
                    }
                    
                    switch(g_sys_status.led_level)
                    {
                        case 0: led_off();    break;
                        case 1: led_low();    break;
                        case 2: led_medium(); break;
                        case 3: led_high();   break;
                        default:              break;
                    }
                }
                
                /* 阶段 3: 计时器独立接管
                 * 按键 3/4：负责倒计时的硬启停与强制清零
                 */
                else if(key_num == 3)
                {
                    timer_toggle();
                }
                else if(key_num == 4)
                {
                    timer_stop();
                    set_sit_time(0);
                    xEventGroupClearBits(system_event_group, EVENT_SIT_TIME_ALARM);
                }
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
        // 提取系统全局事件标志（无阻塞轮询，避免卡死后续的硬件控制流）
        event_bits = xEventGroupGetBits(system_event_group);
        
        if(xSemaphoreTake(status_mutex, portMAX_DELAY) == pdTRUE)
        {
            // 听觉报警执行端：响应过近或超时
            if(event_bits & (EVENT_DISTANCE_ALARM | EVENT_SIT_TIME_ALARM))
            {
                beep_on();
            }
            else
            {
                beep_off();
            }
            
            // 视觉调光执行端：自动模式下的自适应照度补偿
            if(g_sys_status.mode == AUTO_MODE)
            {
                if(g_sys_status.person_flag == TRUE)
                {
                    if(g_sys_status.light_value < 40)
                    {
                        led_high();
                    }
                    else if(g_sys_status.light_value >= 40 && g_sys_status.light_value <= 70)
                    {
                        led_medium();
                    }
                    else if(g_sys_status.light_value > 70)
                    {
                        led_low();
                    }
                }
                else
                {
                    led_off();
                }
            }
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
        if(xSemaphoreTake(status_mutex, portMAX_DELAY) == pdTRUE)
        {
            if(g_sys_status.mode == REMOTE_MODE)
            {
                bt_cmd = bt_rx_get() - '0';
                
                switch(bt_cmd)
                {
                    case 0: led_off();                                                      break;
                    case 1: led_low();                                                      break;
                    case 2: led_medium();                                                   break;
                    case 3: led_high();                                                     break;
                    case 4: timer_start();                                                  break;
                    case 5: timer_stop();                                                   break;
                    case 6: 
                            timer_stop();
                            set_sit_time(0);
                            xEventGroupClearBits(system_event_group, EVENT_DISTANCE_ALARM); break;
                    default:                                                                break;
                }
            }
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
    status_mutex       = xSemaphoreCreateMutex();
    system_event_group = xEventGroupCreate();
    
    memset(&g_sys_status, 0, sizeof(SystemStatus_t));
    g_sys_status.mode = AUTO_MODE;
    
    xTaskCreate(sensor_monitor_task,
                "sensor_monitor",
                128,
                NULL,
                1,
                &sensor_task_handle);
    
    xTaskCreate(display_task,
                "display_update",
                512,
                NULL,
                1,
                &display_task_handle);
    
    xTaskCreate(key_task,
                "key_handler",
                128,
                NULL,
                1,
                &key_task_handle);
     
    xTaskCreate(device_task,
                "device_control",
                128,
                NULL,
                1,
                &device_task_handle);
     
    xTaskCreate(bluetooth_task,
                "bluetooth_rx",
                128,
                NULL,
                1,
                &bluetooth_task_handle);
}
