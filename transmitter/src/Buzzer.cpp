#include "Buzzer.h"

Buzzer::Buzzer(uint8_t buzzerPin) : 
    pin(buzzerPin),
    currentState(OFF),
    lastUpdateTime(0),
    continuousInterval(500),    // 默认持续响间隔500ms
    doubleBeepInterval(200),    // 默认双响间隔200ms
    isBeeping(false),
    beepCount(0) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Buzzer::setState(State newState) {
    if (currentState != newState) {
        currentState = newState;
        isBeeping = false;
        beepCount = 0;
        lastUpdateTime = millis();
        
        // 如果切换到常响或关闭状态，立即更新蜂鸣器
        if (newState == ON) {
            digitalWrite(pin, HIGH);
            isBeeping = true;
        } else if (newState == OFF) {
            digitalWrite(pin, LOW);
            isBeeping = false;
        }
    }
}

void Buzzer::update() {
    switch (currentState) {
        case ON:
            // 常响状态不需要更新
            break;
        case OFF:
            // 关闭状态不需要更新
            break;
        case SINGLE_BEEP:
            updateSingleBeep();
            break;
        case DOUBLE_BEEP:
            updateDoubleBeep();
            break;
        case CONTINUOUS_BEEP:
            updateContinuousBeep();
            break;
    }
}

void Buzzer::updateSingleBeep() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= doubleBeepInterval) {
        if (beepCount == 0) {
            digitalWrite(pin, HIGH);
            isBeeping = true;
            beepCount++;
        } else if (beepCount == 1) {
            digitalWrite(pin, LOW);
            isBeeping = false;
            beepCount = 0;
            setState(OFF);  // 完成单响后关闭
        }
        lastUpdateTime = currentTime;
    }
}

void Buzzer::updateDoubleBeep() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= doubleBeepInterval) {
        switch (beepCount) {
            case 0:  // 第一次响
                digitalWrite(pin, HIGH);
                isBeeping = true;
                break;
            case 1:  // 第一次停
                digitalWrite(pin, LOW);
                isBeeping = false;
                break;
            case 2:  // 第二次响
                digitalWrite(pin, HIGH);
                isBeeping = true;
                break;
            case 3:  // 第二次停
                digitalWrite(pin, LOW);
                isBeeping = false;
                beepCount = -1;  // 重置计数
                setState(OFF);  // 完成双响后关闭
                break;
        }
        beepCount++;
        lastUpdateTime = currentTime;
    }
}

void Buzzer::updateContinuousBeep() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= continuousInterval) {
        isBeeping = !isBeeping;
        digitalWrite(pin, isBeeping ? HIGH : LOW);
        lastUpdateTime = currentTime;
    }
} 