/*
 * ssd1306.h
 *
 *  Created on: 2016年5月14日
 *      Author: Cocoonshu
 */
#include "c_types.h"

#ifndef APP_INCLUDE_COSMART_DEVICE_SSD1306_H_
#define APP_INCLUDE_COSMART_DEVICE_SSD1306_H_

#define ENABLE_FAST_TRANSFER                false                          // 启动快速写入，如果开启，则不检测SSD1306的ACK信号

#define SSD1306_SLAVE_NO_ADDRESS            0x00                           // 无效IIC从机地址
#define SSD1306_SLAVE_HIGH_ADDRESS          0x78                           // AD0拉高后，作为IIC从机的地址, ‭1111 000(rw)
#define SSD1306_SLAVE_LOW_ADDRESS           0x7A                           // AD0拉低后，作为IIC从机的地址, ‭1111 101‬(rw)

#define FLAG_COMMAND                        0x00                           // 发送命令标识
#define FLAG_DATA                           0x40                           // 发送数据标识

#define CMD_SCREEN_OFF                      0xAE                           // 屏幕打开
#define CMD_SCREEN_ON                       0xAF                           // 屏幕关闭
#define CMD_ALL_PIXEL_ON                    0xA5                           // 全部像素亮
#define CMD_ALL_PIXEL_OFF                   0xA4                           // 全部像素灭
#define CMD_SET_CONTRAST                    0x81                           // 设置屏幕对比度
#define CMD_SET_MEM_ADDRESSING_MODE         0x20                           // 内存寻址模式
#define CMD_SET_PAGE_POINTER(page)          (0xB0 + page)                  // 设置PAGE0为页面寻址的起始地址
#define CMD_SET_DISPLAY_START_LINE(line)    (0x40 + line)                  // 设置显示的起始行
#define CMD_SET_DISPLAY_LINE_OFFSET         0xD3                           // 设置显示的行偏移
#define CMD_SET_SEGMENT_REMAP_NORMAL        0xA0                           // 设置SEG重映射，正向
#define CMD_SET_SEGMENT_REMAP_INVERT        0xA1                           // 设置SEG重映射，反向
#define CMD_SET_DISPLAY_MODE_NORNAL         0xA6                           // 设置像素显示模式，正常
#define CMD_SET_DISPLAY_MODE_INVERT         0xA7                           // 设置像素显示模式，反色
#define CMD_SET_PAGE_RANGE                  0x22                           // 设置页的起始及终止页，仅用于水平和垂直寻址模式
#define CMD_SET_COLUMN_RANGE                0x21                           // 设置列的起始及终止像素，仅用于水平和垂直寻址模式
#define CMD_SET_MULTIPLEX_RATIO             0xA8                           // 设置MUX
#define CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO  0xD5                           // 设置显示时钟分频因子
#define CMD_SET_PRECHARGE_INTERVAL          0xD9                           // 设置Pre-charge间隔
#define CMD_SET_COM_CONFIG                  0xDA                           // 设置COM的硬件配置
#define CMD_SET_COMH_VOLT                   0xDB                           // 设置COMH的电压
#define CMD_SET_DC2DC                       0x8D                           // 设置DC2DC配置
#define CMD_ENABLE_SCOLL_MODE               0x2F                           // 启动卷轴模式
#define CMD_DISABLE_SCOLL_MODE              0x2E                           // 停用卷轴模式
#define CMD_COM_SCAN_MODE_ASC               0xC0                           // 设置COM为升序输出扫描模式，即COM(0)->COM(N-1)
#define CMD_COM_SCAN_MODE_DESC              0xC8                           // 设置COM为降序输出扫描模式，即COM(N-1)->COM(0)
#define CMD_COLUMN_POINTER_LOW(column)      (column & 0xF)                 // 设置列指针，低4位
#define CMD_COLUMN_POINTER_HIGH(column)     (((column >> 4) & 0xF) | 0x10) // 设置列指针，高4位

#define ARG_HORIZONTAL_ADDRESSING           0x00
#define ARG_VERTICAL_ADDRESSING             0x01
#define ARG_PAGE_ADDRESSING                 0x02 // Default value

#define ARG_COM_CONFIG_L2R_SEQUENTIAL       (0x02) // 0000 0010
#define ARG_COM_CONFIG_L2R_ALTERNATIVE      (0x12) // 0001 0010
#define ARG_COM_CONFIG_R2L_SEQUENTIAL       (0x22) // 0010 0010
#define ARG_COM_CONFIG_R2L_ALTERNATIVE      (0x32) // 0011 0010

#define ARG_COMH_VOLT_0_65                  (0x00) // 0.65 * VCC
#define ARG_COMH_VOLT_0_77                  (0x20) // 0.77 * VCC
#define ARG_COMH_VOLT_0_88                  (0x30) // 0.88 * VCC

#define ARG_DC2DC_ENABLE                    (0x14) // 启动DC2DC
#define ARG_DC2DC_DISABLE                   (0x10) // 禁用DC2DC

#define NaN 0
#define DEFAULT_SCREEN_OFF                  CMD_SCREEN_OFF,                     NaN,                            0
#define DEFAULT_MEM_ADDRESSING_MODE         CMD_SET_MEM_ADDRESSING_MODE,        ARG_PAGE_ADDRESSING,            1
#define DEFAULT_DISPLAY_START_LINE          CMD_SET_DISPLAY_START_LINE(0),      NaN,                            0
#define DEFAULT_DISPLAY_START_LINE_OFFSET   CMD_SET_DISPLAY_LINE_OFFSET,        0,                              1
#define DEFAULT_PAGE_POINTER                CMD_SET_PAGE_POINTER(0),            NaN,                            0
#define DEFAULT_COLUMN_POINTER_LOW          CMD_COLUMN_POINTER_LOW(0),          0,                              1
#define DEFAULT_COLUMN_POINTER_HIGH         CMD_COLUMN_POINTER_HIGH(0),         0,                              1
#define DEFAULT_COM_SCAN_DIRECTION          CMD_COM_SCAN_MODE_ASC,              NaN,                            0
#define DEFAULT_SEGMENT_REMAP               CMD_SET_SEGMENT_REMAP_NORMAL,       NaN,                            0
#define DEFAULT_CONSTRAST                   CMD_SET_CONTRAST,                   0x7F,                           1
#define DEFAULT_DISPLAY_MODE                CMD_SET_DISPLAY_MODE_NORNAL,        NaN,                            0
#define DEFAULT_MULTIPLEX_RATIO             CMD_SET_MULTIPLEX_RATIO,            63,                             1
#define DEFAULT_COM_CONFIG                  CMD_SET_COM_CONFIG,                 ARG_COM_CONFIG_L2R_ALTERNATIVE, 1
#define DEFAULT_ALL_PIXEL_OFF               CMD_ALL_PIXEL_OFF,                  NaN,                            0
#define DEFAULT_ALL_PIXEL_ON                CMD_ALL_PIXEL_ON,                   NaN,                            0
#define DEFAULT_START_LINE_OFFSET           CMD_SET_DISPLAY_LINE_OFFSET,        0,                              1
#define DEFAULT_DISPLAY_CLOCK_DIVIDE_RATIO  CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO, 0xF0,                           1
#define DFFAULT_PRECHARGE_INTERVAL          CMD_SET_PRECHARGE_INTERVAL,         0x22,                           1
#define DEFAULT_COMH_VOLTAGE                CMD_SET_COMH_VOLT,                  ARG_COMH_VOLT_0_77,             1
#define DEFAULT_DC2DC_CONFIG                CMD_SET_DC2DC,                      ARG_DC2DC_ENABLE,               1
#define DEFAULT_SCREEN_ON                   CMD_SCREEN_ON,                      NaN,                            0

#define DEVICE_PAGE_COUNT                   8
#define DEVICE_COM_COUNT                    8
#define DEVICE_COLUMN_COUNT                 128
#define DEVICE_SEGMENT_COUNT                64
#define DEVICE_HEIGHT_PER_PAGE              8
#define DEVICE_WIDTH_PER_COM                16
#define DEVICE_READ_RETRY_COUNT             5    // 读取IIC从机设备重试次数
#define DEVICE_WRITE_RETRY_COUNT            3    // 写入IIC从机设备重试次数
#define SLAVE_DEVICE_COUNT                  2    // 额定设备挂在数量

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
void SSD1306_drawPixels(uint16 x, uint16 y, uint16 width, uint16 height, uint8* pixels);
void SSD1306_cleanScreen();
void SSD1306_reset();

#endif /* APP_INCLUDE_COSMART_DEVICE_SSD1306_H_ */

