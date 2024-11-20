#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor {
private:
    int pwmPin;         // PWM控制引脚
    int currentSpeed;   // 当前速度值（0-100）

public:
    Motor(int pwmPin);
    void begin();
    void setSpeed(int speed);  // 设置速度 (0-100)
    int getCurrentSpeed();     // 获取当前速度
};

#endif 