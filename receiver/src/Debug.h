#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

// 调试开关，在编译时决定是否包含调试代码
#define DEBUG_ENABLED 1

// 初始化调试串口
#if DEBUG_ENABLED
    #define DEBUG_BEGIN(baud) do { \
        Serial.begin(baud); \
        while(!Serial) { ; } \
    } while(0)
#else
    #define DEBUG_BEGIN(baud)
#endif

// 基础打印宏
#if DEBUG_ENABLED
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    
    // 带标签的打印
    #define DEBUG_INFO(x) do { \
        Serial.print(F("[INFO] ")); \
        Serial.println(x); \
    } while(0)
    
    #define DEBUG_ERROR(x) do { \
        Serial.print(F("[ERROR] ")); \
        Serial.println(x); \
    } while(0)
    
    #define DEBUG_WARN(x) do { \
        Serial.print(F("[WARN] ")); \
        Serial.println(x); \
    } while(0)
    
    // 格式化打印，最大支持128字节
    #define DEBUG_PRINTF(fmt, ...) do { \
        char buf[128]; \
        snprintf(buf, sizeof(buf), fmt, ##__VA_ARGS__); \
        Serial.print(buf); \
    } while(0)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_INFO(x)
    #define DEBUG_ERROR(x)
    #define DEBUG_WARN(x)
    #define DEBUG_PRINTF(fmt, ...)
#endif

#endif 