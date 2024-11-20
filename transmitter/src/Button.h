#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
public:
    // 定义按钮事件类型
    enum ButtonEvent {
        NONE,           // 无事件
        SINGLE_CLICK,   // 单击
        DOUBLE_CLICK,   // 双击
        LONG_PRESS,     // 长按
        LONG_PRESS_RELEASE  // 长按释放
    };

    // 构造函数
    Button(uint8_t buttonPin);
    
    // 更新按钮状态并返回事件类型
    ButtonEvent update();

private:
    uint8_t pin;              // 按钮引脚
    bool lastState;           // 上一次按钮状态
    bool buttonState;         // 当前按钮状态
    unsigned long lastDebounceTime;  // 上次去抖动时间
    
    // 双击检测相关
    unsigned long lastPressTime;     // 上次按下时间
    bool waitingForSecondClick;      // 等待第二次点击标志
    
    // 长按检测相关
    unsigned long pressStartTime;     // 按下开始时间
    bool isLongPressing;             // 正在长按标志
    bool longPressHandled;           // 长按是否已处理标志

    // 时间常量
    static const unsigned long DEBOUNCE_DELAY = 50;    // 去抖动延时(ms)
    static const unsigned long DOUBLE_CLICK_TIME = 400; // 双击间隔时间(ms)
    static const unsigned long LONG_PRESS_TIME = 1000;  // 长按触发时间(ms)
};

#endif 