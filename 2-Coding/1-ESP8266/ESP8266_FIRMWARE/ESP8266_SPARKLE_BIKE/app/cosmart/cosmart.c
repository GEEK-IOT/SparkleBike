/*
 * cosmart.c
 *
 *  Created on: 2015-08-07
 *      Author: Cocoonshu
 */

#include "cosmart/config.h"
#include "cosmart/textutils.h"
#include "cosmart/cosmart.h"
#include "cosmart/gpiomanager.h"
#include "cosmart/wifimanager.h"
#include "cosmart/pwmmanager.h"
#include "cosmart/device/led.h"
#include "cosmart/device/servo.h"
#include "cosmart/device/mpu6050.h"
#include "cosmart/device/ssd1306.h"
#include "cosmart/log.h"
#include "cosmart/timer.h"
#include "cosmart/mathutils.h"
#include "cosmart/irmanager.h"
#include "cosmart/interruptmanager.h"
#include "user_interface.h"

// 内部函数声明
LOCAL void ICACHE_FLASH_ATTR showSystemInformation();
LOCAL void ICACHE_FLASH_ATTR onSystemInitialized();

///////////////
//  外部函数   //
///////////////
void ICACHE_FLASH_ATTR Cosmart_initialize() {
	// 初始化Log组件
	Log_initialize();
	Log_printCopyright();

	// 初始化US级定时器
#ifdef USE_US_TIMER
	system_timer_reinit();
#endif

	// 注册系统初始化完成回调
	system_init_done_cb(onSystemInitialized);
}

#if ENABLE_SERVO_DEBUG
os_timer_t mServoTimer;
int mServoAngle;

void ICACHE_FLASH_ATTR servoProcess() {
	mServoAngle = (mServoAngle + 180) % 360;
	Log_printfln("[servoProcess] Set Servo to %d", mServoAngle);
	Servo_setAngle(0, mServoAngle);
}
#endif

///////////////
//  内部函数   //
///////////////
LOCAL void ICACHE_FLASH_ATTR onSystemInitialized() {
	Log_printfln("[CoSmart] System initialized");

	// 输出系统信息
	showSystemInformation();

	// 设定Wifi模式为AP & STA
	WiFi_initialize();
	WiFi_setupAPAndSTA();
	WiFi_setupAPRecevier();

	// 启动Timer管理器
	Timer_initialize();

#if ENABLE_LED_DEBUG
	// 启动PWM控制器，并启动LED驱动
	uint32 ledPorts[] = {12, 13, 15};
	PWM_initialize();
	LED_initialize(ledPorts, 3);
	PWM_setup();
	LED_startBreathingWithDurationAndDelay(0, 3000, 000);
	LED_startBreathingWithDurationAndDelay(1, 3000, 333);
	LED_startBreathingWithDurationAndDelay(2, 3000, 666);
#endif

#if ENABLE_SERVO_DEBUG
	// 启动GPIO控制器，并启动Servo驱动
	uint32 servoPorts[] = {14};
	GPIO_initialize();
	Servo_initialize(servoPorts, 1);
	Servo_setupIOPort(0, SERVO_SG90_CONFIG);
	Servo_startMotion(0);
	os_timer_setfn(&mServoTimer, (os_timer_func_t*)servoProcess, NULL);
	os_timer_arm(&mServoTimer, 1500, 1);
#endif

#if ENABLE_MPU6050_DEBUG
	// 启动MPU6050驱动
	MPU6050_initialize();
	MPU6050_startPollingAttitude();
#endif

#if ENABLE_SSD1306_DEBUG
	// 启动SSD1306驱动
	SSD1306_initialize();
#endif
	// 初始化终端管理器
	//InterruptManager_initialize();

	// 启动红外发送管理器
	//IR_initialize();

	// 启动广播接收器，接收控制端广播
	WiFi_setupProtocolBridge();
}

LOCAL void ICACHE_FLASH_ATTR showSystemInformation() {
	uint32      chipID             = system_get_chip_id();
	uint32      spiFlashID         = spi_flash_get_id();
	const char* sdkVersion         = system_get_sdk_version();
	uint8       bootVersion        = system_get_boot_version();
	uint8       bootMode           = system_get_boot_mode();
	uint32      bootAddress        = system_get_userbin_addr();
	uint8       cpuFrequency       = system_get_cpu_freq();
	uint32      physicalMode       = wifi_get_phy_mode();
	uint32      freeHeapSize       = system_get_free_heap_size();
	uint32      flashSizeMap       = system_get_flash_size_map();
	uint32      rtcClock           = system_rtc_clock_cali_proc();
	uint32      rtcClockInteger    = rtcClock >> 12;
	uint32      rtcClockFractional = -(rtcClock | 0xFFFFF000) & 0xFFFFF000;
	uint16      systemVoltLevel    = clampu16(system_get_vdd33(), MIN_SYS_VOLT, MAX_SYS_VOLT);
	float       systemVolt         = (float)systemVoltLevel / 1024.0f;
	uint16      systemVoltInt      = (uint16)systemVolt;
	uint16      systemVoltDec      = (uint16)((systemVolt - systemVoltInt) * 100.0f);
	uint32      pwmVersion         = pwm_get_period();
	uint8       apMacAddr[6]       = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8       staMacAddr[6]      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	wifi_get_macaddr(SOFTAP_IF, apMacAddr);
	wifi_get_macaddr(STATION_IF, staMacAddr);

	Log_printfln(LOG_EMPTY);
	Log_printfln(LOG_SYS_INFO);
	Log_printfln(LOG_SYS_INFO_CHIP_ID,       chipID);
	Log_printfln(LOG_SYS_INFO_FLASH_ID,      spiFlashID);
	Log_printfln(LOG_SYS_INFO_BOOT_VERSION,  bootVersion);
	Log_printfln(LOG_SYS_INFO_BOOT_MODE,     bootMode == 0 ? BOOT_MODE_ENHANCE : BOOT_MODE_NORMAL);
	Log_printfln(LOG_SYS_INFO_BOOT_ADDRESS,  bootAddress);
	Log_printfln(LOG_SYS_INFO_SDK_VERSION,   sdkVersion);
	Log_printfln(LOG_SYS_INFO_ROM_VERSION,   COSMART_VERSION);
	Log_printfln(LOG_SYS_INFO_CPU_FREQUENCY, cpuFrequency);
	Log_printfln(LOG_SYS_INFO_WIFI_PROTOCOL, Text_physicalMode2STR(physicalMode));
	Log_printfln(LOG_SYS_INFO_RTC_CLOCK,     rtcClockInteger, rtcClockFractional);
	Log_printfln(LOG_SYS_INFO_FREE_HEAP,     freeHeapSize);
	Log_printfln(LOG_SYS_INFO_FLASH_SIZE,    Text_flashSizeMap2STR(flashSizeMap));
	Log_printfln(LOG_SYS_INFO_SYSTEM_VOLT,   systemVoltInt, systemVoltDec, systemVolt);
	Log_printfln(LOG_SYS_INFO_PWM_VERSION,   pwmVersion);
	Log_printfln(LOG_SYS_INFO_AP_MAC,        MAC2STR(apMacAddr));
	Log_printfln(LOG_SYS_INFO_STA_MAC,       MAC2STR(staMacAddr));
}
