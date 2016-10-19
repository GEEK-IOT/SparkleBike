/*
 * ssd1306.h
 *
 *  Created on: 2016年5月14日
 *      Author: Cocoonshu
 */
#include "c_types.h"

#ifndef ICACHE_DATA_ATTR
#define ICACHE_DATA_ATTR ICACHE_RODATA_ATTR
#endif

#ifndef APP_INCLUDE_COSMART_DEVICE_SSD1306_H_
#define APP_INCLUDE_COSMART_DEVICE_SSD1306_H_

#define ENABLE_FAST_TRANSFER                false                          // 启动快速写入，如果开启，则不检测SSD1306的ACK信号

#define SSD1306_SLAVE_NO_ADDRESS            0x00                           // 无效IIC从机地址
#define SSD1306_SLAVE_HIGH_ADDRESS          0x78                           // AD0拉高后，作为IIC从机的地址, ‭1111 000(rw)
#define SSD1306_SLAVE_LOW_ADDRESS           0x7A                           // AD0拉低后，作为IIC从机的地址, ‭1111 101‬(rw)

#define FLAG_COMMAND                        0x00                           // 发送命令标识
#define FLAG_DATA                           0x40                           // 发送数据标识

#define SSD1306_DISPLAYOFF                  0xAE                           // 屏幕打开
#define SSD1306_DISPLAYON                   0xAF                           // 屏幕关闭
#define SSD1306_PIXELON                     0xA5                           // 全部像素亮
#define SSD1306_PIXELOFF                    0xA4                           // 全部像素灭
#define SSD1306_SETCONTRAST                 0x81                           // 设置屏幕对比度
#define SSD1306_STARTPAGE(page)             (0xB0 | page)                  // 设置PAGE0为页面寻址的起始地址
#define SSD1306_STARTLINE(line)             (0x40 | line)                  // 设置显示的起始行
#define SSD1306_SETDISPLAYOFFSET            0xD3                           // 设置显示的行偏移
#define SSD1306_SEGREMAP                    0xA0                           // 设置SEG重映射，正向
#define SSD1306_INVERTSEGREMAP              0xA1                           // 设置SEG重映射，反向
#define SSD1306_NORMALDISPLAY               0xA6                           // 设置像素显示模式，正常
#define SSD1306_INVERTDISPLAY               0xA7                           // 设置像素显示模式，反色
#define SSD1306_PAGERANGE                   0x22                           // 设置页的起始及终止页，仅用于水平和垂直寻址模式
#define SSD1306_COLUMNRANGE                 0x21                           // 设置列的起始及终止像素，仅用于水平和垂直寻址模式
#define SSD1306_SETMULTIPLEX                0xA8                           // 设置MUX
#define SSD1306_SETDISPLAYCLOCKDIV          0xD5                           // 设置显示时钟分频因子
#define SSD1306_SETPRECHARGE                0xD9                           // 设置Pre-charge间隔
#define SSD1306_SETCOMPINS                  0xDA                           // 设置COM的硬件配置
#define SSD1306_SETVCOMDETECT               0xDB                           // 设置COMH的电压
#define SSD1306_CHARGEPUMP                  0x8D                           // 设置DC2DC配置
#define SSD1306_MEMORYMODE                  0x20                           // 设置GDDRAM寻址模式
#define SSD1306_ACTIVESCROLL                0x2F                           // 启动卷轴模式
#define SSD1306_DEACTIVESCROLL              0x2E                           // 停用卷轴模式
#define SSD1306_COMSCANASC                  0xC0                           // 设置COM为升序输出扫描模式，即COM(0)->COM(N-1)
#define SSD1306_COMSCANDEC                  0xC8                           // 设置COM为降序输出扫描模式，即COM(N-1)->COM(0)
#define SSD1306_STARTCOLUMN_LOW(column)     (column & 0xF)                 // 设置列指针，低4位
#define SSD1306_STARTCOLUMN_HIGH(column)    (((column >> 4) & 0xF) | 0x10) // 设置列指针，高4位

#define SSD1306_MEMORYMODE_HORIZONTAL       0x00
#define SSD1306_MEMORYMODE_VERTICAL         0x01
#define SSD1306_MEMORYMODE_PAGING           0x02 // Default value

#define SSD1306_COM_CONFIG_L2R_SEQUENTIAL   (0x02) // 0000 0010
#define SSD1306_COM_CONFIG_L2R_ALTERNATIVE  (0x12) // 0001 0010
#define SSD1306_COM_CONFIG_R2L_SEQUENTIAL   (0x22) // 0010 0010
#define SSD1306_COM_CONFIG_R2L_ALTERNATIVE  (0x32) // 0011 0010

#define SSD1306_COMH_VOLT_0_65              (0x00) // 0.65 * VCC
#define SSD1306_COMH_VOLT_0_77              (0x20) // 0.77 * VCC
#define SSD1306_COMH_VOLT_0_88              (0x30) // 0.88 * VCC

#define SSD1306_CHARGEPUMP_ENABLE           (0x14) // 启动DC2DC
#define SSD1306_CHARGEPUMP_DISABLE          (0x10) // 禁用DC2DC

#define DEVICE_PAGE_COUNT                   8
#define DEVICE_COM_COUNT                    8
#define DEVICE_COLUMN_COUNT                 128
#define DEVICE_SEGMENT_COUNT                64
#define DEVICE_HEIGHT_PER_PAGE              8
#define DEVICE_WIDTH_PER_COM                16
#define DEVICE_READ_RETRY_COUNT             5    // 读取IIC从机设备重试次数
#define DEVICE_WRITE_RETRY_COUNT            3    // 写入IIC从机设备重试次数
#define SLAVE_DEVICE_COUNT                  2    // 额定设备挂在数量

extern uint8 ASCII[][7];

typedef struct {
	uint8  address;
	uint8* frameBuffer;
	uint8  comSize;
	uint8  pageSize;
	uint16 colummSize;
	uint16 segmentSize;
} SSD1306Device;

void SSD1306_initialize();
int  SSD1306_getSlaveDeviceCount();
int  SSD1306_getValidSlaveDeviceCount();
bool SSD1306_bindDevice(int index);
void SSD1306_drawPixels(uint16 x, uint16 y, uint16 width, uint16 height, const uint8* pixels);
void SSD1306_drawBuffer(uint16 startColumn, uint16 endColumn, uint16 startPage, uint16 endPage, const uint8* buffer);
void SSD1306_drawText(char* text, uint16 x, uint16 y);
void SSD1306_cleanScreen();
void SSD1306_reset();

#endif /* APP_INCLUDE_COSMART_DEVICE_SSD1306_H_ */

