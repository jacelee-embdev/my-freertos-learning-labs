#include "w25q128.h"
#include "uart1.h"
#include "delay.h"

SPI_HandleTypeDef spi_handle = {0};

/* SPI 基础参数初始化 */
void w25q128_spi_init(void)
{
    spi_handle.Instance               = W25Q_SPI_INSTANCE;              // 使用宏定义实例
    spi_handle.Init.Mode              = SPI_MODE_MASTER;                         
    spi_handle.Init.Direction         = SPI_DIRECTION_2LINES;               
    spi_handle.Init.DataSize          = SPI_DATASIZE_8BIT;                   
    spi_handle.Init.CLKPolarity       = SPI_POLARITY_LOW;                 
    spi_handle.Init.CLKPhase          = SPI_PHASE_1EDGE;                     
    spi_handle.Init.NSS               = SPI_NSS_SOFT;                             
    spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;  
    spi_handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;                    
    spi_handle.Init.TIMode            = SPI_TIMODE_DISABLE;
    spi_handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    spi_handle.Init.CRCPolynomial     = 7;
    HAL_SPI_Init(&spi_handle);
}

/* SPI 底层引脚与时钟初始化 (MSP回调) */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == W25Q_SPI_INSTANCE)                             // 匹配宏定义实例
    {
        GPIO_InitTypeDef gpio_initstruct;
    
        W25Q_GPIO_RCC_CLK_ENABLE();                                     // 使能端口时钟
        W25Q_SPI_RCC_CLK_ENABLE();                                      // 使能外设时钟
        
        // 1. 片选引脚 (CS) - 推挽输出
        gpio_initstruct.Pin   = W25Q_CS_PIN;
        gpio_initstruct.Mode  = GPIO_MODE_OUTPUT_PP;
        gpio_initstruct.Pull  = GPIO_PULLUP;
        gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(W25Q_GPIO_PORT, &gpio_initstruct);
        
        // 2. 复用引脚 (SCK, MOSI) - 复用推挽
        gpio_initstruct.Pin   = W25Q_SCK_PIN | W25Q_MOSI_PIN;
        gpio_initstruct.Mode  = GPIO_MODE_AF_PP;           
        HAL_GPIO_Init(W25Q_GPIO_PORT, &gpio_initstruct);
        
        // 3. 输入引脚 (MISO) - 浮空/上拉输入
        gpio_initstruct.Pin   = W25Q_MISO_PIN;
        gpio_initstruct.Mode  = GPIO_MODE_INPUT;           
        HAL_GPIO_Init(W25Q_GPIO_PORT, &gpio_initstruct);
    }
}

/* W25Q128 整体初始化 */
void w25q128_init(void)
{
    w25q128_spi_init();
    W25Q128_CS(1);                                                      // 初始释放片选
    delay_ms(5);
}

/* SPI 交换字节 */
uint8_t w25q128_spi_swap_byte(uint8_t data)
{
    uint8_t recv_data = 0;
    HAL_SPI_TransmitReceive(&spi_handle, &data, &recv_data, 1, 1000);
    return recv_data;
}

/* 写使能 */
void w25q128_write_enable(void)
{
    W25Q128_CS(0);
    w25q128_spi_swap_byte(FLASH_WriteEnable);
    W25Q128_CS(1);
}

/* 读状态寄存器1 */
uint8_t w25q128_read_sr1(void)
{
    uint8_t recv_data = 0;
    W25Q128_CS(0);
    w25q128_spi_swap_byte(FLASH_ReadStatusReg1);
    recv_data = w25q128_spi_swap_byte(FLASH_DummyByte);
    W25Q128_CS(1);
    return recv_data;
}

/* 等待忙碌结束 */
void w25q128_wait_buzy(void)
{
    uint32_t timeout = 0;
    while((w25q128_read_sr1() & 0x01) == 0x01)
    {
        timeout++;
        if(timeout > 0xFFFF)
        {
            printf("Error: SPI Flash Wait Busy Timeout!\r\n");
            break;
        }
    }
}

/* 发送24位地址 */
void w25q128_send_address(uint32_t address)
{
    w25q128_spi_swap_byte(address >> 16);
    w25q128_spi_swap_byte(address >> 8);
    w25q128_spi_swap_byte(address);
}

/* 读取数据 */
void w25q128_read_data(uint32_t address, uint8_t *data, uint32_t size)
{
    uint32_t i = 0;
    W25Q128_CS(0);
    w25q128_spi_swap_byte(FLASH_ReadData);
    w25q128_send_address(address);
    for(i = 0; i < size; i++)
    {
        data[i] = w25q128_spi_swap_byte(FLASH_DummyByte);
    }
    W25Q128_CS(1);
}

/* 页编程 */
void w25q128_write_page(uint32_t address, uint8_t *data, uint16_t size)
{
    uint16_t i = 0;
    w25q128_write_enable();
    W25Q128_CS(0);
    w25q128_spi_swap_byte(FLASH_PageProgram);
    w25q128_send_address(address);
    for(i = 0; i < size; i++)
    {
        w25q128_spi_swap_byte(data[i]);
    }
    W25Q128_CS(1);
    w25q128_wait_buzy();
}

/* 扇区擦除 */
void w25q128_erase_sector(uint32_t address)
{
    w25q128_write_enable();
    w25q128_wait_buzy();
    W25Q128_CS(0);
    w25q128_spi_swap_byte(FLASH_SectorErase);
    w25q128_send_address(address);
    W25Q128_CS(1);
    w25q128_wait_buzy();
}

/* 读取设备ID */
uint16_t w25q128_read_id(void)
{
    uint16_t device_id = 0;
    W25Q128_CS(0);
    w25q128_spi_swap_byte(FLASH_ManufactDeviceID);
    w25q128_spi_swap_byte(0x00);
    w25q128_spi_swap_byte(0x00);
    w25q128_spi_swap_byte(0x00);
    device_id = w25q128_spi_swap_byte(FLASH_DummyByte) << 8;
    device_id |= w25q128_spi_swap_byte(FLASH_DummyByte);
    W25Q128_CS(1);
    return device_id;
}


/**
* @brief 全片擦除 (清除密码,恢复出厂设置)
 */
void w25q128_chip_erase(void)
{
    // 1. 解锁：清除 BP0-BP2 保护位
    w25q128_write_enable();
    W25Q128_CS(0);
    w25q128_spi_swap_byte(0x01); // 写状态寄存器指令
    w25q128_spi_swap_byte(0x00); // 清零保护位
    W25Q128_CS(1);
    w25q128_wait_buzy();

    // 2. 发令：写使能后立即发擦除指令
    w25q128_write_enable();
    W25Q128_CS(0);
    w25q128_spi_swap_byte(0xC7); // 全片擦除指令
    W25Q128_CS(1);
    
    // 3. 阻塞：死等 BUSY 位清零
    printf("Wiping chip, please wait...\r\n");
    while((w25q128_read_sr1() & 0x01) == 0x01)
    {
        delay_ms(100); 
    }
    printf("Chip erase success!\r\n");
}

/* 测试函数 */
void w25q128_test(void)
{
    uint16_t device_id = w25q128_read_id();
    printf("W25Q128 Device ID: 0x%04X\r\n", device_id);
}

