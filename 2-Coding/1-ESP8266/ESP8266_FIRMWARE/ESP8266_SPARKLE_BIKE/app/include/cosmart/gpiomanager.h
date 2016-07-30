/*
 * gpiomanager.h
 *
 *  Created on: 2016年1月18日
 *      Author: 80074591
 */

#ifndef APP_INCLUDE_COSMART_GPIOMANAGER_H_
#define APP_INCLUDE_COSMART_GPIOMANAGER_H_

#include "c_types.h"
#include "eagle_soc.h"
#include "gpio.h"

#define GPIO_00 (GPIO_PIN0_ADDRESS + 4 * 0x0)
#define GPIO_01 (GPIO_PIN0_ADDRESS + 4 * 0x1)
#define GPIO_02 (GPIO_PIN0_ADDRESS + 4 * 0x2)
#define GPIO_03 (GPIO_PIN0_ADDRESS + 4 * 0x3)
#define GPIO_04 (GPIO_PIN0_ADDRESS + 4 * 0x4)
#define GPIO_05 (GPIO_PIN0_ADDRESS + 4 * 0x5)
#define GPIO_06 (GPIO_PIN0_ADDRESS + 4 * 0x6)
#define GPIO_07 (GPIO_PIN0_ADDRESS + 4 * 0x7)
#define GPIO_08 (GPIO_PIN0_ADDRESS + 4 * 0x8)
#define GPIO_09 (GPIO_PIN0_ADDRESS + 4 * 0x9)
#define GPIO_10 (GPIO_PIN0_ADDRESS + 4 * 0xA)
#define GPIO_11 (GPIO_PIN0_ADDRESS + 4 * 0xB)
#define GPIO_12 (GPIO_PIN0_ADDRESS + 4 * 0xC)
#define GPIO_13 (GPIO_PIN0_ADDRESS + 4 * 0xD)
#define GPIO_14 (GPIO_PIN0_ADDRESS + 4 * 0xE)
#define GPIO_15 (GPIO_PIN0_ADDRESS + 4 * 0xF)

#define PWM_REG_00 (PERIPHS_IO_MUX_GPIO0_U)
#define PWM_REG_01 (PERIPHS_IO_MUX_U0TXD_U)
#define PWM_REG_02 (PERIPHS_IO_MUX_GPIO2_U)
#define PWM_REG_03 (PERIPHS_IO_MUX_U0RXD_U)
#define PWM_REG_04 (PERIPHS_IO_MUX_GPIO4_U)
#define PWM_REG_05 (PERIPHS_IO_MUX_GPIO5_U)
#define PWM_REG_06 (0)
#define PWM_REG_07 (0)
#define PWM_REG_08 (0)
#define PWM_REG_09 (PERIPHS_IO_MUX_SD_DATA2_U)
#define PWM_REG_10 (PERIPHS_IO_MUX_SD_DATA3_U)
#define PWM_REG_11 (0)
#define PWM_REG_12 (PERIPHS_IO_MUX_MTDI_U)
#define PWM_REG_13 (PERIPHS_IO_MUX_MTCK_U)
#define PWM_REG_14 (PERIPHS_IO_MUX_MTMS_U)
#define PWM_REG_15 (PERIPHS_IO_MUX_MTDO_U)

#define PWM_MUX_00 (FUNC_GPIO0)
#define PWM_MUX_01 (FUNC_GPIO1)
#define PWM_MUX_02 (FUNC_GPIO2)
#define PWM_MUX_03 (FUNC_GPIO3)
#define PWM_MUX_04 (FUNC_GPIO4)
#define PWM_MUX_05 (FUNC_GPIO5)
#define PWM_MUX_06 (0)
#define PWM_MUX_07 (0)
#define PWM_MUX_08 (0)
#define PWM_MUX_09 (FUNC_GPIO9)
#define PWM_MUX_10 (FUNC_GPIO10)
#define PWM_MUX_11 (0)
#define PWM_MUX_12 (FUNC_GPIO12)
#define PWM_MUX_13 (FUNC_GPIO13)
#define PWM_MUX_14 (FUNC_GPIO14)
#define PWM_MUX_15 (FUNC_GPIO15)

#define PWM_PIN_00 (0x0)
#define PWM_PIN_01 (0x1)
#define PWM_PIN_02 (0x2)
#define PWM_PIN_03 (0x3)
#define PWM_PIN_04 (0x4)
#define PWM_PIN_05 (0x5)
#define PWM_PIN_06 (0x6)
#define PWM_PIN_07 (0x7)
#define PWM_PIN_08 (0x8)
#define PWM_PIN_09 (0x9)
#define PWM_PIN_10 (0xA)
#define PWM_PIN_11 (0xB)
#define PWM_PIN_12 (0xC)
#define PWM_PIN_13 (0xD)
#define PWM_PIN_14 (0xE)
#define PWM_PIN_15 (0xF)

#define GPIO_PIN_00 (0x0)
#define GPIO_PIN_01 (0x1)
#define GPIO_PIN_02 (0x2)
#define GPIO_PIN_03 (0x3)
#define GPIO_PIN_04 (0x4)
#define GPIO_PIN_05 (0x5)
#define GPIO_PIN_06 (0x6)
#define GPIO_PIN_07 (0x7)
#define GPIO_PIN_08 (0x8)
#define GPIO_PIN_09 (0x9)
#define GPIO_PIN_10 (0xA)
#define GPIO_PIN_11 (0xB)
#define GPIO_PIN_12 (0xC)
#define GPIO_PIN_13 (0xD)
#define GPIO_PIN_14 (0xE)
#define GPIO_PIN_15 (0xF)

#define GPIO_REGISTER    0
#define GPIO_PORT_NUMBER 16
#define GPIO_MUX         1
#define GPIO_PIN         2
#define DEFAULT_PERIOD   1000

/**
 * =====================================================================
 * 测试板拨码开关配置
 * =====================================================================
 *  开关名称   - 默认值   -  备注
 * =====================================================================
 * 	GPIO14   -  ON    - ESP8266模块的GPIO14与继电器连接
 * 	DL_MCU   -  OFF   - 测试板MCU与继电器连接
 * 	CH_PD_H  -  ON    - ESP8266模块的CH_PD(EN)拉高
 * 	GPIO00_L -  OFF   - ESP8266模块的GPIO00拉低，如需为ESP8266更新固件，
 * 	                                        置此位为ON
 * 	GPIO15_L -  ON    - ESP8266模块的GPIO15拉低
 * 	GPIO02_L -  OFF   - ESP8266模块的GPIO02拉低
 * ---------------------------------------------------------------------
 * 	DC_R_ON  -  OFF   - ESP8266模块的RXD与测试版MCU的TXD接通
 * 	DC_T_ON  -  OFF   - ESP8266模块的TXD与测试版MCU的RXD接通
 * 	ESP_T_ON -  ON    - ESP8266模块的RXD与USB的TXD接通
 * 	MCU_T_ON -  OFF   - 测试版MCU的RXD与USB的TXD接通
 * 	ESP_R_ON -  ON    - ESP8266模块的TXD与USB的RXD接通
 * 	MCU_R_ON -  OFF   - 测试版MCU的TXD与USB的RXD接通
 * =====================================================================
 *
 * =====================================================================
 * ESP8266最小系统接线配置
 * =====================================================================
 * 启动配置(Flash Boot模式)
 * ---------------------------------------------------------------------
 *  CH_PD(EN) - 拉高
 *  REST      - 拉高
 *  VCC       - 拉高
 *  GND       - 拉低
 *  GPIO00    - 拉高
 *  GPIO02    - 拉高
 *  GPIO15    - 拉低
 * =====================================================================
 * 烧写固件配置 (UART下载模式)
 * ---------------------------------------------------------------------
 *  CH_PD(EN) - 拉高
 *  REST      - 拉高
 *  VCC       - 拉高
 *  GND       - 拉低
 *  GPIO00    - 拉低
 *  GPIO02    - 拉高
 *  GPIO15    - 拉低
 * =====================================================================
 *
 * =====================================================================
 * ESP8266模块管脚说明
 * =====================================================================
 *  序号  -  名称           - 备注
 * ---------------------------------------------------------------------
 *   01  - RST       - ESP8266重置
 *   02  - ADC       - 10bit精度模拟电压检测，主要用于检测传感器输出或电池电压
 *                     输入电压在0.0 ~ 1.0V，取值范围为0 ~ 1024
 *   03  - CH_PD(EN) - ESP8266使能端，高电平时芯片运行
 *   04  - GPIO16    - GPIO16，与RST接通时，可做Deep sleep的唤醒
 *   05  - GPIO14    - GPIO14，                      HSPI_CLK
 *   06  - GPIO12    - GPIO12，                      HSPI_MISO
 *   07  - GPIO13    - GPIO13，UART0_CTS，HSPI_MOSI
 *   08  - VCC       - 3.3V供电
 *   09  - CS0       - 片选，从设备使能信号，由主设备控制
 *   10  - MISO      - SPI从机输出、主机输入
 *   11  - GPIO09    - GPIO09
 *   12  - GPIO10    - GPIO10
 *   13  - MOSI      - SPI主机输出、从机输入
 *   14  - SCLK      - SPI时钟
 *   15  - GND       - 接地
 *   16  - GPIO15    - GPIO15，UART0_RTS，HSPI_ICS ，MTDO
 *   17  - GPIO02    - GPIO02，UART1_TXD
 *   18  - GPIO00    - GPIO00
 *   19  - GPIO04    - GPIO04
 *   20  - GPIO05    - GPIO05
 *   21  - GPIO03    - GPIO03，UART0_RXD
 *   22  - GPIO01    - GPIO01，UART0_TXD
 * =====================================================================
 *
 * =====================================================================
 * ESP8266 - 12接口功能脚说明
 * =====================================================================
 * IR接口
 *    - GPIO14 - IR_T
 *    - GPIO05 - IR_R
 *    IR Remote Control 接口由软件实现，接口使用NEC编码及调制解调，采用
 *    38kHz的调制载波
 * ---------------------------------------------------------------------
 * HSPI接口
 *    - GPIO12 - MISO
 *    - GPIO13 - MOSI
 *    - GPIO14 - SCLK
 *    - GPIO15 - CS
 *    可外接4SPI Flash、显示屏、MCU等
 * ---------------------------------------------------------------------
 * I2C接口
 *    - GPIO14 - SCL
 *    - GPIO02 - SDA
 *    可外接传感器及显示屏等
 * ---------------------------------------------------------------------
 * I2S接口
 *    I2S输入：
 *       - GPIO12 - I2SI_DATA
 *       - GPIO13 - I2SI_BCK
 *       - GPIO14 - I2SI_WS
 *    I2S输出：
 *       - GPIO02 - I2SO_WS
 *       - GPIO03 - I2SO_DATA
 *       - GPIO15 - I2SO_BCK
 *    主要用于音频的采集、处理、传输
 * ---------------------------------------------------------------------
 * UART接口
 *    - GPIO01 - UART0_TXD
 *    - GPIO03 - UART0_RXD
 *    - GPIO15 - UART0_RTS/MTDO
 *    - GPIO13 - UART0_CTS/MTCK
 *    - GPIO02 - UART1_TXD
 *    可外接UART接口的设备。
 *    下载：UART0_TXD + UART0_RXD 或者 UART1_TXD + UART0_RXD
 *    通信：UART0_TXD + UART0_RXD + UART0_RTS + UART0_CTS
 *    调试：UART1_TXD可作为Debug信息的打印
 *    UART0在ESP8266启动时会输出一些打印信息，对此敏感的应用，可以使用UART
 *    的内部引脚交换功能，在初始化时，将UART0_TXD、UART0_RXD分别与UART0_RTS
 *    、UART0_CTS交换，硬件上将MTDO、MTCK连接到对应的外部MCU的串口进行通信
 * =====================================================================
 *
 * =====================================================================
 * 逻辑电平定义
 * =====================================================================
 *  方向         电平                          采样值
 * ---------------------------------------------------------------------
 *  输入           低                -0.3v   ~ 0.25 * VDD
 *  输入           高          0.75 * VDD ~ VDD + 0.3v
 *  输出           低                        0.1 * VDD
 *  输出           高                        0.8 * VDD
 * =====================================================================
 */

typedef enum {
	HighLevel    = 1,
	LowLevel     = 0,
	UnknownLevel = 2
} GPIOLevel;

typedef enum {
	InputMode,
	OutputMode,
	InputAndOutputMode,
	UnknownIOMode
} GPIOIOMode;

void       GPIO_initialize();
void       GPIO_setLevel(uint32 gpioPin, GPIOLevel level);
void       GPIO_setIOMode(uint32 gpioPin, GPIOIOMode ioMode);
GPIOIOMode GPIO_getIOMode(uint32 gpioPin);
GPIOLevel  GPIO_getLevel(uint32 gpioPin);
bool       GPIO_isInputIO(uint32 gpioPin);
bool       GPIO_isOutputIO(uint32 gpioPin);
bool       GPIO_isHighLevel(uint32 gpioPin);
bool       GPIO_isLowLevel(uint32 gpioPin);

#endif /* APP_INCLUDE_COSMART_GPIOMANAGER_H_ */
