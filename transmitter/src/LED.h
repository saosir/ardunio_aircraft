#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LED {
public:
    // LED状态枚举
    enum State {
        OFF,                // 关闭
        ON,                 // 常亮
        SINGLE_FLASH,       // 单闪
        DOUBLE_FLASH,       // 双闪
        CONTINUOUS_FLASH,   // 持续闪烁
        BREATHING           // 呼吸灯效果
    };

    // 构造函数
    LED(uint8_t ledPin);
    
    // 更新LED状态
    void update();
    
    // 设置LED状态
    void setState(State newState);
    
    // 设置闪烁间隔(ms)
    void setFlashInterval(unsigned long interval) { flashInterval = interval; }
    
    // 设置呼吸周期(ms)
    void setBreathCycle(unsigned long cycle) { breathCycle = cycle; }

private:
    uint8_t pin;                    // LED引脚
    State currentState;             // 当前状态
    unsigned long lastUpdateTime;    // 上次更新时间
    unsigned long flashInterval;     // 闪烁间隔
    unsigned long breathCycle;       // 呼吸周期
    bool isOn;                      // LED当前是否亮起
    uint8_t flashCount;             // 闪烁计数
    
    // 更新不同模式
    void updateSingleFlash();
    void updateDoubleFlash();
    void updateContinuousFlash();
    void updateBreathing();
};

#endif 