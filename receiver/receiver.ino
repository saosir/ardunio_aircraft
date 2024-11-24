#include "src/Motor.h"
#include "src/Debug.h"
#include <SPI.h>
#include <RF24.h>

// RF24 配置
RF24 radio(7, 8);  // CE, CSN 引脚
const byte address[][6] = {"1Node", "2Node"};  // 使用两个地址用于双向通信

// 创建电机对象
Motor leftMotor(5);    // 左电机PWM引脚
Motor rightMotor(6);   // 右电机PWM引脚

// 控制数据结构
struct ControlData {
    int8_t throttle;    // 油门值 (0 到 100)
    int8_t direction;   // 方向值 (-100 到 100)
} controlData;

// 失联检测
unsigned long lastReceiveTime = 0;
unsigned long lastDebugTime = 0;    // 上次打印调试信息的时间
const unsigned long timeoutDuration = 1000;  // 失联超时时间
const unsigned long debugInterval = 1000;    // 调试打印间隔（1秒）

void setup() {
    DEBUG_BEGIN(9600);
    DEBUG_INFO("接收端启动");
    
    // 初始化电机
    leftMotor.begin();
    rightMotor.begin();
    
    // RF24初始化
    if (!radio.begin()) {
        DEBUG_ERROR("Radio hardware not responding!");
        while (1) {}
    }
    
    // NRF24L01+特定配置
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.enableAckPayload();
    
    // 设置通道并验证
    radio.setChannel(76);
    uint8_t actualChannel = radio.getChannel();
    DEBUG_PRINTF("设置通道: 76, 实际通道: %d\n", actualChannel);
    
    radio.openWritingPipe(address[1]);
    radio.openReadingPipe(1, address[0]);
    radio.startListening();
    
    // 准备ACK数据
    char ack = 'A';
    radio.writeAckPayload(1, &ack, sizeof(ack));
    
    DEBUG_INFO("初始化完成");
}

void loop() {
    if (radio.available()) {
        // 读取数据
        radio.read(&controlData, sizeof(controlData));
        lastReceiveTime = millis();
        
        // 准备下一个ACK
        char ack = 'A';
        radio.writeAckPayload(1, &ack, sizeof(ack));
        
        // 计算和设置电机速度
        int leftSpeed, rightSpeed;
        calculateMotorSpeeds(controlData.throttle, controlData.direction, leftSpeed, rightSpeed);
        leftMotor.setSpeed(leftSpeed);
        rightMotor.setSpeed(rightSpeed);
        
        // 打印接收到的数据
        DEBUG_PRINTF("接收数据 - 油门: %d, 方向: %d -> 左电机: %d%%, 右电机: %d%%\n", 
            controlData.throttle, 
            controlData.direction,
            leftSpeed,
            rightSpeed);
    }
    
    // 检查是否失联
    if (millis() - lastReceiveTime > timeoutDuration) {
        if (leftMotor.getCurrentSpeed() != 0 || rightMotor.getCurrentSpeed() != 0) {
            DEBUG_WARN("失联超时，停止电机");
            leftMotor.setSpeed(0);
            rightMotor.setSpeed(0);
        }
    }
}

// 计算左右电机速度
void calculateMotorSpeeds(int throttle, int direction, int& leftSpeed, int& rightSpeed) {
    // 将油门限制在0到100范围内，方向限制在-100到100范围内
    throttle = constrain(throttle, 0, 100);
    direction = constrain(direction, -100, 100);
    
    // 直接使用throttle作为实际油门值（因为已经是0~100范围）
    int actualThrottle = throttle;
    
    // 在高速时增加转向效果（实际油门大于75%时）
    if (actualThrottle > 75) {
        if (direction > 0) {  // 向右转
            leftSpeed = actualThrottle;
            // 高速时内侧电机降低更多
            rightSpeed = actualThrottle * (100 - abs(direction) * 1.5) / 100;
        } else if (direction < 0) {  // 向左转
            rightSpeed = actualThrottle;
            // 高速时内侧电机降低更多
            leftSpeed = actualThrottle * (100 - abs(direction) * 1.5) / 100;
        } else {  // 直行
            leftSpeed = actualThrottle;
            rightSpeed = actualThrottle;
        }
    } else {
        // 低速时使用普通差速转向
        float turnRatio = abs(direction) / 100.0;  // 转向比例
        if (direction > 0) {  // 向右转
            leftSpeed = actualThrottle;
            rightSpeed = actualThrottle * (1 - turnRatio);
        } else if (direction < 0) {  // 向左转
            rightSpeed = actualThrottle;
            leftSpeed = actualThrottle * (1 - turnRatio);
        } else {  // 直行
            leftSpeed = actualThrottle;
            rightSpeed = actualThrottle;
        }
    }
    
    // 确保速度在0到100范围内
    leftSpeed = constrain(leftSpeed, 0, 100);
    rightSpeed = constrain(rightSpeed, 0, 100);
    
    // 调试输出
    DEBUG_PRINTF("接收数据 - 油门: %d, 方向: %d -> 左电机: %d%%, 右电机: %d%%\n", 
        throttle, 
        direction,
        leftSpeed,
        rightSpeed);
}