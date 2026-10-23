#include "password.h"
#include "w25q128.h"
#include "string.h"
#include "stdio.h"
#include "keyboard.h"
#include "oled.h"
#include "relay_lock.h"
#include "beep.h"
#include "delay.h"

#define PASSWOED_SIZE   10
uint8_t pwd_input[PASSWOED_SIZE] = {0};
uint8_t pwd_read[PASSWOED_SIZE] = {0};
uint8_t i = 0;
uint8_t key_value = 0;
uint8_t pwd_error_count = 0;

//初始化函数
void password_init(void)
{
    w25q128_init();
}

//清空输入缓存
void password_input_clear(void)
{
    memset(pwd_input,0,PASSWOED_SIZE);
    i = 0;
}

//保存密码
void password_save(void)
{
    w25q128_erase_sector(0x000000);
    w25q128_write_page(0x000000,pwd_input,PASSWOED_SIZE);
}

//获取键盘输入(用于 裸机 开发)
uint8_t password_get_input(void)
{
    password_input_clear();
    while(1)
    {
        key_value = keyboard_get_value();
        if(key_value == POUND_KEY )
        {
            printf("按下了#键,input:%s\r\n",pwd_input);
            return POUND_KEY;
        }
        else if(key_value == START_KEY)
        {
            printf("按下了*键,input:%s\r\n",pwd_input);
            return START_KEY;
        }
        else if(key_value != 0)
        {
            printf("按下了*键,input:%c\r\n",key_value);
            oled_show_char(20+i *10,4,key_value,16);
            pwd_input[i++] = key_value;
        }
    }
}

//用于保存外部密码设置(用于 OS 开发)
void password_set_input(uint8_t key_val)
{
    if(i < PASSWOED_SIZE && key_val != 0)
    {
        pwd_input[i++] = key_val;
    }
}

//密码比对
uint8_t password_compare(void)
{
    uint8_t i = 0;
    w25q128_read_data(0x000000,pwd_read,PASSWOED_SIZE);
    
    // 长度不等直接返回错误
    if(strlen((char *)pwd_input) != strlen((char *)pwd_read))
    {
        return FALSE;
    }
    
    // 逐个字符比对
    for(i = 0; i < strlen((char *)pwd_read); i++)
    {
        if(pwd_input[i] != pwd_read[i])
        {
            return FALSE;
        }
    }
    
    return TRUE;
}


//密码输入正确的操作
void password_input_right_operate(void)
{
    oled_show_right();
    relay_lock_on();
    beep_on();
    delay_ms(500);
    beep_off();
    delay_ms(1000);
    relay_lock_off();
    pwd_error_count = 0;
}

//密码输入错误的操作
void password_input_wrong_operate(void)
{
    oled_show_wrong();
    pwd_error_count++;
    if(pwd_error_count >= 3)
    {
        beep_on();
        delay_ms(1000);
        beep_off();
        pwd_error_count = 0;
    }
     delay_ms(1000);
}

//旧密码输入正确的操作
void password_old_right_operate(void)
{
    oled_show_new();
    password_get_input();
    password_save();
    
    beep_on();
    delay_ms(300);
    beep_off();
    
    delay_ms(700);
}

//旧密码输入错误的操作
void password_old_wrong_operate(void)
{
    oled_show_wrong();
    delay_ms(1000);
}

//上电后检查密码是否存在
void password_check(void)
{
    w25q128_read_data(0x000000,pwd_read,PASSWOED_SIZE);
    printf("请输入密码:%s\r\n",pwd_read);
    if(pwd_read[0] == '\0' || pwd_read[0] == 0xFF)
    {
        oled_show_set();
        password_get_input();
        password_save();
    }
}
