#include "Motor.h"

Motor::Motor(int pwmPin) {
    this->pwmPin = pwmPin;
    this->currentSpeed = 0;
}

void Motor::begin() {
    pinMode(pwmPin, OUTPUT);
    analogWrite(pwmPin, 0);
}

void Motor::setSpeed(int speed) {
    // 限制速度在0-100范围内
    speed = constrain(speed, 0, 100);
    currentSpeed = speed;
    
    // 将0-100的速度映射到0-255的PWM值
    int pwmValue = map(speed, 0, 100, 0, 255);
    analogWrite(pwmPin, pwmValue);
}

int Motor::getCurrentSpeed() {
    return currentSpeed;
} 