#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
public:
    // 蜂鸣器状态枚举
    enum State {
        OFF,                // 关闭
        ON,                 // 常响
        SINGLE_BEEP,       // 单响
        DOUBLE_BEEP,       // 双响
        CONTINUOUS_BEEP    // 持续响
    };

    // 构造函数
    Buzzer(uint8_t buzzerPin);
    
    // 更新蜂鸣器状态
    void update();
    
    // 设置蜂鸣器状态
    void setState(State newState);
    
    // 设置持续响的间隔(ms)
    void setContinuousInterval(unsigned long interval) { continuousInterval = interval; }
    
    // 设置双响的间隔(ms)
    void setDoubleBeepInterval(unsigned long interval) { doubleBeepInterval = interval; }

private:
    uint8_t pin;                       // 蜂鸣器引脚
    State currentState;                // 当前状态
    unsigned long lastUpdateTime;       // 上次更新时间
    unsigned long continuousInterval;   // 持续响的间隔 (默认500ms)
    unsigned long doubleBeepInterval;   // 双响的间隔 (默认200ms)
    bool isBeeping;                    // 蜂鸣器当前是否在响
    uint8_t beepCount;                 // 响声计数
    
    // 更新不同模式
    void updateSingleBeep();
    void updateDoubleBeep();
    void updateContinuousBeep();
};

#endif 