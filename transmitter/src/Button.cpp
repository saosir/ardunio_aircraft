#include "Button.h"

Button::Button(uint8_t buttonPin) : 
    pin(buttonPin),
    lastState(HIGH),
    buttonState(HIGH),
    lastDebounceTime(0),
    lastPressTime(0),
    waitingForSecondClick(false),
    pressStartTime(0),
    isLongPressing(false),
    longPressHandled(false) {
    pinMode(pin, INPUT_PULLUP);
}

Button::ButtonEvent Button::update() {
    ButtonEvent event = NONE;
    int reading = digitalRead(pin);
    unsigned long currentTime = millis();

    // 去抖动处理
    if (reading != lastState) {
        lastDebounceTime = currentTime;
    }

    if ((currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != buttonState) {
            buttonState = reading;
            
            if (buttonState == LOW) {  // 按钮按下
                pressStartTime = currentTime;
                isLongPressing = true;
                longPressHandled = false;
            }
            else {  // 按钮释放
                if (isLongPressing) {
                    if (longPressHandled) {
                        event = LONG_PRESS_RELEASE;
                    } else {
                        // 检查是否为双击的一部分
                        if (waitingForSecondClick) {
                            if ((currentTime - lastPressTime) <= DOUBLE_CLICK_TIME) {
                                event = DOUBLE_CLICK;
                                waitingForSecondClick = false;
                            }
                        } else {
                            waitingForSecondClick = true;
                            lastPressTime = currentTime;
                        }
                    }
                }
                isLongPressing = false;
            }
        }
    }

    // 检查长按
    if (isLongPressing && !longPressHandled) {
        if ((currentTime - pressStartTime) >= LONG_PRESS_TIME) {
            event = LONG_PRESS;
            longPressHandled = true;
        }
    }

    // 检查单击超时
    if (waitingForSecondClick && 
        (currentTime - lastPressTime) > DOUBLE_CLICK_TIME && 
        !isLongPressing) {
        event = SINGLE_CLICK;
        waitingForSecondClick = false;
    }

    lastState = reading;
    return event;
} 