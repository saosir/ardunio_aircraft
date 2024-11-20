#include "Joystick.h"
#include <Arduino.h>

Joystick::Joystick(int pin, bool autoCenter, int deadzone, int minValue, int maxValue) {
    this->pin = pin;
    this->deadzone = deadzone;
    this->autoCenter = autoCenter;
    this->minValue = minValue;
    this->maxValue = maxValue;
    this->center = autoCenter ? (maxValue + minValue) / 2 : minValue;
}

void Joystick::begin() {
    pinMode(pin, INPUT);
    if (autoCenter) {
        calibrate();
    }
}

void Joystick::calibrate() {
    if (autoCenter) {
        center = analogRead(pin);
        // 确保中心点在有效范围内
        center = constrain(center, minValue, maxValue);
        delay(100);
    }
}

int Joystick::getRaw() {
    int raw = analogRead(pin);
    return constrain(raw, minValue, maxValue);
}

int Joystick::getMapped() {
    int raw = getRaw();
    
    if (autoCenter) {
        // 自动回中的摇杆使用中心点计算
        int value = raw - center;
        
        // 应用死区
        if (abs(value) < deadzone) {
            return 0;
        }
        
        // 映射到 -100 到 100
        if (value > 0) {
            return map(value, deadzone, maxValue-center, 0, 100);
        } else {
            return map(value, minValue-center, -deadzone, -100, 0);
        }
    } else {
        // 不自动回中的摇杆直接映射整个范围
        return map(raw, minValue, maxValue, -100, 100);
    }
}

bool Joystick::isCenter() {
    if (!autoCenter) {
        return false;
    }
    int value = getRaw() - center;
    return abs(value) < deadzone;
} 