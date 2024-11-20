#include "LED.h"

LED::LED(uint8_t ledPin) : 
    pin(ledPin),
    currentState(OFF),
    lastUpdateTime(0),
    flashInterval(500),    // 默认闪烁间隔500ms
    breathCycle(2000),     // 默认呼吸周期2000ms
    isOn(false),
    flashCount(0) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void LED::setState(State newState) {
    if (currentState != newState) {
        currentState = newState;
        isOn = false;
        flashCount = 0;
        lastUpdateTime = millis();
        
        // 如果切换到常亮或关闭状态，立即更新LED
        if (newState == ON) {
            digitalWrite(pin, HIGH);
            isOn = true;
        } else if (newState == OFF) {
            digitalWrite(pin, LOW);
            isOn = false;
        }
    }
}

void LED::update() {
    switch (currentState) {
        case ON:
            // 常亮状态不需要更新
            break;
        case OFF:
            // 关闭状态不需要更新
            break;
        case SINGLE_FLASH:
            updateSingleFlash();
            break;
        case DOUBLE_FLASH:
            updateDoubleFlash();
            break;
        case CONTINUOUS_FLASH:
            updateContinuousFlash();
            break;
        case BREATHING:
            updateBreathing();
            break;
    }
}

void LED::updateSingleFlash() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= flashInterval) {
        if (flashCount == 0) {
            digitalWrite(pin, HIGH);
            isOn = true;
            flashCount++;
        } else if (flashCount == 1) {
            digitalWrite(pin, LOW);
            isOn = false;
            flashCount = 0;
            setState(OFF);  // 完成单闪后，使用setState将状态设置为关闭
        }
        lastUpdateTime = currentTime;
    }
}

void LED::updateDoubleFlash() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= flashInterval/2) {
        switch (flashCount) {
            case 0:  // 第一次亮
                digitalWrite(pin, HIGH);
                isOn = true;
                break;
            case 1:  // 第一次灭
                digitalWrite(pin, LOW);
                isOn = false;
                break;
            case 2:  // 第二次亮
                digitalWrite(pin, HIGH);
                isOn = true;
                break;
            case 3:  // 第二次灭
                digitalWrite(pin, LOW);
                isOn = false;
                flashCount = -1;  // 重置计数
                setState(OFF);  // 完成双闪后，使用setState将状态设置为关闭
                break;
        }
        flashCount++;
        lastUpdateTime = currentTime;
    }
}

void LED::updateContinuousFlash() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= flashInterval) {
        isOn = !isOn;
        digitalWrite(pin, isOn ? HIGH : LOW);
        lastUpdateTime = currentTime;
    }
}

void LED::updateBreathing() {
    unsigned long currentTime = millis();
    unsigned long timeDiff = currentTime - lastUpdateTime;
    
    if (timeDiff >= 20) {  // 每20ms更新一次亮度
        // 计算当前周期内的位置(0-1023)
        unsigned int position = (currentTime % breathCycle) * 1023 / breathCycle;
        
        // 使用正弦函数计算亮度
        float brightness = sin((position * PI) / 1023.0);
        brightness = brightness * brightness * 255;  // 平方使呼吸效果更自然
        
        analogWrite(pin, (int)brightness);
        lastUpdateTime = currentTime;
    }
} 