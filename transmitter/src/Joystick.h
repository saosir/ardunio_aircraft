#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

class Joystick {
private:
    int pin;            // 模拟输入引脚
    int center;         // 中心值
    int deadzone;       // 死区值
    bool autoCenter;    // 是否自动回中
    int minValue;       // 最小值
    int maxValue;       // 最大值

public:
    // 构造函数添加死区和范围参数
    Joystick(int pin, bool autoCenter = true, 
             int deadzone = 50,
             int minValue = 0,
             int maxValue = 1023);
             
    void begin();
    void calibrate();   // 校准中心点
    int getRaw();       // 获取原始值
    int getMapped();    // 获取映射后的值 (-100 到 100)
    bool isCenter();    // 判断是否在中心位置
};

#endif 