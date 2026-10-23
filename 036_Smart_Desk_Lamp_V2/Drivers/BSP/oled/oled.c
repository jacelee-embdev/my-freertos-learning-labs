#include "sys.h"
#include "oled.h"
#include "delay.h"
#include "font.h"

/* 
 * 说    明: 配置模拟I2C所需的SCL(时钟)和SDA(数据)引脚
 * SSD1306 OLED使用I2C接口，需要高速推挽输出模式
 */
void oled_gpio_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    
    // 1. 开启GPIO端口时钟
    OLED_IIC_SCL_CLK();
    OLED_IIC_SDA_CLK();
    
    // 2. 配置SCL引脚 (时钟线)
    gpio_initstruct.Pin=OLED_IIC_SCL_PIN;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;   // 推挽输出，驱动能力强
    gpio_initstruct.Pull=GPIO_PULLUP;           // 上拉，I2C总线空闲时需保持高电平
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_IIC_SCL_PORT,&gpio_initstruct);
    
    // 3. 配置SDA引脚 (数据线)
    gpio_initstruct.Pin=OLED_IIC_SDA_PIN;
    HAL_GPIO_Init(OLED_IIC_SDA_PORT,&gpio_initstruct);
}

/* * I2C协议：起始信号
 * 时序要求: SCL为高电平时，SDA由高变低
 */
void oled_iic_start(void)
{
    OLED_SCL_SET();     // 拉高时钟
    OLED_SDA_SET();     // 拉高数据
    OLED_SDA_RESET();   // 数据线拉低 -> 产生START信号
    OLED_SCL_RESET();   // 钳住I2C总线，准备发送数据
    
}

/* * I2C协议：停止信号
 * 时序要求: SCL为高电平时，SDA由低变高
 */
void oled_iic_stop(void)
{
    OLED_SCL_SET();
    OLED_SDA_RESET();   // 确保SDA先为低  
    OLED_SDA_SET();     // 数据线拉高 -> 产生STOP信号
}

/* * I2C协议：等待应答 (简化处理)
 * 说明: 这里为了简化，没有实际读取SDA的ACK状态，而是直接产生一个时钟脉冲
 * 这在OLED只写不读的应用中通常可行
 */
void oled_iic_ack(void)
{
    OLED_SCL_SET();     // 产生第九个时钟脉冲的高电平
    OLED_SCL_RESET();   // 结束应答
}

/* * I2C协议：发送一个字节
 * 参数: data - 要发送的8位数据
 * 说明: MSB(最高位)先行，SCL低电平改变数据，SCL高电平读取数据
 */
void oled_write_byte(uint8_t data)
{
    uint8_t i,temp;
    temp = data;
    for(i=0; i<8; i++)
    {
        // 判断最高位是1还是0
        if((temp & 0x80) == 0x80)
        {
            OLED_SDA_SET();
        }
        else
        {
            OLED_SDA_RESET();
        }
        
        temp = temp << 1;   // 左移一位，准备发送下一位
        
        OLED_SCL_SET();     // 拉高时钟，通知从机读取SDA上的数据
        OLED_SCL_RESET();   // 拉低时钟，准备发送下一位
    }
}

/* * 写命令到OLED
 * 参数: cmd - 命令字节
 * 流程: Start -> 0x78(从机地址) -> 0x00(命令控制字) -> Command -> Stop
 */
void oled_write_command(uint8_t cmd)
{
    oled_iic_start();
    oled_write_byte(0x78);      // OLED的I2C从机地址 (通常是0x78或0x7A)
    oled_iic_ack();
    oled_write_byte(0x00);      // 控制字节: Co=0, D/C#=0 -> 接下来的字节是“命令”
    oled_iic_ack();
    oled_write_byte(cmd);       // 发送实际命令
    oled_iic_ack();
    oled_iic_stop();
}

/* * 写数据到OLED (显示内容)
 * 参数: data - 数据字节
 * 流程: Start -> 0x78 -> 0x40(数据控制字) -> Data -> Stop
 */
void oled_write_data(uint8_t data)
{
    oled_iic_start();
    oled_write_byte(0x78);
    oled_iic_ack();
    oled_write_byte(0x40);      // 控制字节: Co=0, D/C#=1 -> 接下来的字节是“显存数据”
    oled_iic_ack();
    oled_write_byte(data);      // 发送实际数据
    oled_iic_ack();
    oled_iic_stop();
}

void oled_init(void)
{
    oled_gpio_init();
    
    delay_ms(100);              // 必须延时，等待OLED模块电源稳定
         
    oled_write_command(0xAE);    //设置显示开启/关闭，0xAE关闭，0xAF开启

    // --- 时钟设置 ---
    oled_write_command(0xD5);    //设置显示时钟分频比/振荡器频率
    oled_write_command(0x80);    //0x00~0xFF

    // --- 多路复用设置 ---
    oled_write_command(0xA8);    //设置多路复用率
    oled_write_command(0x3F);    //0x0E~0x3F

    // --- 显示偏移设置 ---
    oled_write_command(0xD3);    //设置显示偏移
    oled_write_command(0x00);    //0x00~0x7F

    oled_write_command(0x40);    //设置显示开始行，0x40~0x7F

    // --- 屏幕方向设置 (重要) ---
    oled_write_command(0xA1);    //设置左右方向，0xA1正常，0xA0左右反置
    oled_write_command(0xC8);    //设置上下方向，0xC8正常，0xC0上下反置

    // --- 硬件引脚配置 ---
    oled_write_command(0xDA);    //设置COM引脚硬件配置
    oled_write_command(0x12);

    // --- 亮度与对比度 ---
    oled_write_command(0x81);    //设置对比度
    oled_write_command(0xCF);    //0x00~0xFF

    oled_write_command(0xD9);    //设置预充电周期
    oled_write_command(0xF1);

    oled_write_command(0xDB);    //设置VCOMH取消选择级别
    oled_write_command(0x30);

    // --- 显示开启 ---
    oled_write_command(0xA4);    //设置整个显示打开/关闭

    oled_write_command(0xA6);    //设置正常/反色显示，0xA6正常，0xA7反色

    oled_write_command(0x8D);    //设置充电泵
    oled_write_command(0x14);

    oled_write_command(0xAF);    //开启显示
}

/* * 设置光标位置 (页寻址模式)
 * x: 列地址 (0-127)
 * y: 页地址 (0-7, 对应64像素高，每页8像素)
 */
void oled_set_cursor(uint8_t x,uint8_t y)
{
    oled_write_command(0xB0 + y);

    // SSD1306将列地址分为高4位和低4位发送
    oled_write_command((x & 0x0F) | 0x00);          //低4位
    oled_write_command(((x & 0xF0) >> 4) | 0x10);   //高4位
}

/* * 全屏填充/清屏
 * data: 填充内容 (0x00=全黑清屏, 0xFF=全亮)
 */
void oled_fill(uint8_t data)
{
    uint8_t i, j;
    for(i=0;i<8;i++)            // 遍历8个页 (Page 0-7)
    {
        oled_set_cursor(0,i);
        for(j=0;j<128;j++)      // 遍历128列
        {
            oled_write_data(data);
        }
    }
}

/* * 显示单个ASCII字符
 * x, y: 坐标 (y为页地址 0-7)
 * numb: 字符内容
 * size: 字体大小 (12/16/24)，对应高度。宽度是高度的一半。
 */
void oled_show_char(uint8_t x,uint8_t y,uint8_t numb,uint8_t size)
{
    uint8_t i,j,page;
    
    numb = numb - ' ';      // 将ASCII码转换为字库数组的索引 (字库通常从空格(32)开始)
    
    // 计算该字体跨越了几个“页”(8像素高为1页)
    page=size/8;
    if(size % 8)
    {
        page++;
    }
    
    for(j=0;j<page;j++)
    {
        oled_set_cursor(x,y+j);             // 移动光标到下一页
        for(i=size/2*j;i<size/2*(j+1);i++)
        {
            if(size==12)
            {
                oled_write_data(ascii_6X12[numb][i]);
            }
            else if(size==16)
            {
                oled_write_data(ascii_8X16[numb][i]);
            }
            else if(size==24)
            {
                oled_write_data(ascii_12X24[numb][i]);
            }
        }
    }
}

/* * 显示字符串
 * 原理：循环调用显示字符函数，并自动增加x坐标
 */
void oled_show_string(uint8_t x,uint8_t y,char *string,uint8_t size)
{
    while(*string != '\0')
    {
        oled_show_char(x,y,*string,size);
        x +=size/2;                         // 光标右移一个字符的宽度
        string++;                           // 指针后移
    }
}

/* * 显示汉字
 * x, y: 坐标
 * ch_string: 汉字在数组 chinese_16x16[] 中的索引号
 * size: 字体大小 (16/24)
 */
void oled_show_Chinese(uint8_t x,uint8_t y,uint8_t ch_string)
{
    uint16_t i,j;
    for(j = 0;j < 16/8;j++)
    {
        oled_set_cursor(x,y+j);
        for(i = 16*j;i < 16 * (j+1);i++)
        {
                oled_write_data(lamp_16x16[ch_string][i]);       // 遍历宽度上的像素数据
        }
    }
}

/* * 显示BMP图片
 * x, y: 起始坐标
 * width: 图片宽度 (像素)
 * height: 图片高度 (这里实际上代表占用的“页数”，即 像素高度/8)
 * bmp: 图片取模数据数组指针
 */
void oled_show_imge(uint8_t x,uint8_t y,uint8_t width,uint8_t height,uint8_t *bmp)
{
    uint8_t i,j;
    for(j =0;j < height;j++)                            // 注意：这里的height通常是 Page数，而非像素数
    {
        oled_set_cursor(x,y+j);
        for(i = 0;i < width;i++)
        {
            oled_write_data(bmp[width * j + i]);        // 按序写入图片数据
        }
    }
}

void oled_show_init(void)
{
    oled_fill(0x00);    //清屏

    oled_show_Chinese(10,0,1);   //模式：
    oled_show_Chinese(25,0,2);
    oled_show_Chinese(40,0,0);

    oled_show_Chinese(10,2,12);   //亮度：
    oled_show_Chinese(25,2,13);
    oled_show_Chinese(40,2,0);

    oled_show_Chinese(25,4,11);   //  人        cm
    oled_show_string(96,4,"cm",16);

    oled_show_Chinese(10,6,18);   //计时：
    oled_show_Chinese(25,6,19);
    oled_show_Chinese(40,6,0);
    oled_show_char(100,6,'s',16);
}
