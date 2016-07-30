ESP8266_NONOS_SDK_V1.5.4.1_patch 发布说明
本patch基于ESP8266_NONOS_SDK_V1.5.4，使用时将压缩包中.a文件覆盖SDK/lib目录下对应文件。
有如下更新：
1. 修正部分情况下，RF_Init_data 系统参数区可能被破坏的问题。新增 user_rf_cal_sector_set，用户程序必须实现此接口，详细参见文档 “2C-ESP8266__SDK__API Guide”。 (1.5.4.1中解决）
2. 修正外部复位后，小概率存在 rf 不工作的问题。（1.5.4.1中解决）
3. 修正部分情况下，调用cur、def接口导致的状态错误问题。（1.5.4.1中解决）
4. 新增AT针对SDIO的支持。（1.5.4.1中解决）
5. 修正espconn中一处潜在问题。（1.5.4.1中解决）
6. 修改esp_init_data_default.bin的112字节，0x03->0x00，默认关闭频偏自动校正。

注意：基于更新1的示例代码可参考压缩包中的user_main.c。
_____________________________________________________________________________________

ESP8266_NONOS_SDK_V1.5.4.1_patch release notes
This patch is based on the ESP8266_NONOS_SDK_V1.5.4. When this patch is used, .a files in this patch shall replace corresponding files under the SDK/lib Folder.

Optimisation:
1. Resolved an issue that RF_Init_data sector may be broken in stress test. Provided a function “user_rf_cal_sector_set” which has to be added in application by software developer. More details about user_rf_cal_sector_set refer to documentation “2C-ESP8266__SDK__API Guide”. (Resolved in ESP8266_NONOS_SDK_V1.5.4.1) 

2. Fix a potential risk that will cause rf not work after external reset. (Resolved in ESP8266_NONOS_SDK_V1.5.4.1)

3. Fix system state mismatch when call some cur and def APIs. (Resolved in ESP8266_NONOS_SDK_V1.5.4.1)

4. Add SDIO AT support. (Resolved in ESP8266_NONOS_SDK_V1.5.4.1)

5. Fix a potential bug in espconn. (Resolved in ESP8266_NONOS_SDK_V1.5.4.1)

6. Modify 112 byte of esp_init_data_default.bin, 0x03->0x00, turn off auto freq correction.

Attention: Please refer to “user_main.c” in the zip for “user_rf_cal_sector_set” sample code.