#ifndef __W25Q128_H__
#define __W25Q128_H__

#include "sys.h"

/* ===========================================================================================
 * 硬件接口定义
 * =========================================================================================== */
#define W25Q_SPI_INSTANCE               SPI1                             // SPI 外设实例
#define W25Q_SPI_RCC_CLK_ENABLE()       __HAL_RCC_SPI1_CLK_ENABLE()      // SPI 时钟使能

#define W25Q_GPIO_PORT                  GPIOA                            // GPIO 端口
#define W25Q_GPIO_RCC_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()     // GPIO 时钟使能

#define W25Q_CS_PIN                     GPIO_PIN_4                       // 片选引脚 (NSS)
#define W25Q_SCK_PIN                    GPIO_PIN_5                       // 时钟引脚 (SCK)
#define W25Q_MISO_PIN                   GPIO_PIN_6                       // 主入从出引脚 (MISO)
#define W25Q_MOSI_PIN                   GPIO_PIN_7                       // 主出从入引脚 (MOSI)

/* ===========================================================================================
 * 指令表与功能宏
 * =========================================================================================== */
/* 片选控制宏 */
#define W25Q128_CS(x)   do{ x ? \
                                HAL_GPIO_WritePin(W25Q_GPIO_PORT, W25Q_CS_PIN, GPIO_PIN_SET): \
                                HAL_GPIO_WritePin(W25Q_GPIO_PORT, W25Q_CS_PIN, GPIO_PIN_RESET); \
                          }while(0)

/* W25Q128 指令 */
#define FLASH_ManufactDeviceID          0x90    // 读制造商/设备ID
#define FLASH_WriteEnable               0x06    // 写使能
#define FLASH_ReadStatusReg1            0x05    // 读状态寄存器1
#define FLASH_ReadData                  0x03    // 读取数据
#define FLASH_PageProgram               0x02    // 页编程
#define FLASH_SectorErase               0x20    // 扇区擦除
#define FLASH_DummyByte                 0xFF    // 哑字节
#define FLASH_ChipErase                 0xC7    // 全片擦除指令
#define FLASH_WriteStatusReg1           0x01    // 写状态寄存器1

/* ===========================================================================================
 * 函数声明
 * =========================================================================================== */
void w25q128_init(void);                                                        // 初始化
void w25q128_read_data(uint32_t address, uint8_t *data, uint32_t size);         // 读取数据
void w25q128_write_page(uint32_t address, uint8_t *data, uint16_t size);        // 页写入
void w25q128_erase_sector(uint32_t address);                                    // 扇区擦除
uint16_t w25q128_read_id(void);                                                 // 读取ID
uint8_t w25q128_spi_swap_byte(uint8_t data);                                    // SPI 字节交换
void w25q128_chip_erase(void);                                                  //全片擦除函数
void w25q128_test(void);                                                        // 测试函数

#endif
