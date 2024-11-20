#include "src/LED.h"
#include "src/Button.h"
#include "src/Joystick.h"
#include "src/Debug.h"
#include <SPI.h>
#include <RF24.h>

// RF24 配置
RF24 radio(7, 8);  // CE, CSN 引脚
const byte address[][6] = {"1Node", "2Node"};  // 使用两个地址用于双向通信

// 创建LED对象
LED statusLed(2);    // 状态指示灯
LED signalLed(4);    // 信号指示灯

// 创建摇杆对象
Joystick throttleStick(A3, false, 50, 0, 820);    // 油门摇杆（上下），不自动回中，最大值820
Joystick directionStick(A1, true, 50, 0, 1023);   // 方向摇杆（左右），自动回中，全范围

// 控制数据结构
struct ControlData {
    int8_t throttle;    // 油门值 (-100 到 100)
    int8_t direction;   // 方向值 (-100 到 100)
} controlData;

unsigned long lastAckTime = 0;      // 上次收到ACK的时间
unsigned long lastDebugTime = 0;    // 上次打印调试信息的时间
const unsigned long ackTimeout = 1000;  // ACK超时时间
const unsigned long debugInterval = 1000;  // 调试打印间隔（1秒）
bool firstConnection = true;  // 标志是否是第一次连接

void setup() {
    DEBUG_BEGIN(9600);
    DEBUG_INFO("遥控器启动");
    
    // 初始化摇杆
    throttleStick.begin();
    directionStick.begin();
    
    if (!radio.begin()) {
        DEBUG_ERROR("Radio hardware not responding!");
        while (1) {}
    }
    
    // NRF24L01+配置
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(3, 5);
    radio.enableAckPayload();
    
    // 设置通道并验证
    radio.setChannel(76);
    uint8_t actualChannel = radio.getChannel();
    DEBUG_PRINTF("设置通道: 76, 实际通道: %d\n", actualChannel);
    if (actualChannel != 76) {
        DEBUG_ERROR("通道设置失败！");
    }
    
    radio.openWritingPipe(address[0]);
    radio.openReadingPipe(1, address[1]);
    
    // 开机指示
    statusLed.setState(LED::ON);
    DEBUG_INFO("初始化完成");
}

void loop() {
    // 更新LED状态
    statusLed.update();
    signalLed.update();
    
    // 读取摇杆数据
    controlData.throttle = throttleStick.getMapped();
    controlData.direction = directionStick.getMapped();
    
    // 每秒打印一次摇杆值
    if (millis() - lastDebugTime >= debugInterval) {
        DEBUG_PRINTF("油门: %d, 方向: %d\n", controlData.throttle, controlData.direction);
        DEBUG_PRINTF("油门原始值: %d, 方向原始值: %d\n", 
            throttleStick.getRaw(), 
            directionStick.getRaw());
        lastDebugTime = millis();
    }
    
    // 发送数据并等待ACK
    bool success = radio.write(&controlData, sizeof(controlData));
    
    if (success) {
        if (radio.isAckPayloadAvailable()) {
            char ack;
            radio.read(&ack, sizeof(ack));
            if (ack == 'A') {
                lastAckTime = millis();
                if (firstConnection) {
                    signalLed.setState(LED::DOUBLE_FLASH);
                    firstConnection = false;
                } else {
                    signalLed.setState(LED::ON);
                }
            }
        } else {
            signalLed.setState(LED::CONTINUOUS_FLASH);
        }
    } else {
        signalLed.setState(LED::CONTINUOUS_FLASH);
    }
    
    // 检查ACK超时
    if (millis() - lastAckTime > ackTimeout) {
        signalLed.setState(LED::CONTINUOUS_FLASH);
    }
    
    delay(20);  // 50Hz发送频率
} 