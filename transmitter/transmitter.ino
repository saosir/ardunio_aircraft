#include "src/LED.h"
#include "src/Button.h"
#include "src/Joystick.h"
#include "src/Debug.h"
#include "src/Buzzer.h"
#include <SPI.h>
#include <RF24.h>

// RF24 配置
RF24 radio(7, 8);  // CE, CSN 引脚
const byte address[][6] = {"1Node", "2Node"};  // 使用两个地址用于双向通信

// 创建LED对象
LED statusLed(4);    // 状态指示灯
LED signalLed(5);    // 信号指示灯

// 创建摇杆对象
Joystick throttleStick(A4, true, 50, 0, 1023);    // 油门摇杆（上下），改为自动回中
Joystick directionStick(A5, true, 50, 0, 1023);   // 方向摇杆（左右），自动回中

// 添加按钮头文件和对象声明
Button controlButton(2);  // 控制按钮在2号引脚

// 创建蜂鸣器对象
Buzzer buzzer(3);

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

// 添加状态标志
bool systemEnabled = false;    // 系统是否已启动的标志

void setup() {
    DEBUG_BEGIN(9600);
    DEBUG_INFO("遥控器启动");
    
    // 初始化摇杆
    throttleStick.begin();
    directionStick.begin();
    
    if (!radio.begin()) {
        DEBUG_ERROR("Radio hardware not responding!");
        // while (1) {}
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
    
    // 开机时LED闪烁，表示等待启动
    statusLed.setState(LED::CONTINUOUS_FLASH);
    DEBUG_INFO("等待按下按钮启动系统");
}

void loop() {
    // 更新LED状态
    statusLed.update();
    signalLed.update();
    
    // 更新按钮状态并获取事件
    Button::ButtonEvent buttonEvent = controlButton.update();
    
    // 检查系统是否已启动
    if (!systemEnabled) {
        // 等待按钮单击才启动系统
        if (buttonEvent == Button::SINGLE_CLICK) {
            systemEnabled = true;
            statusLed.setState(LED::ON);
            DEBUG_INFO("系统已启动");
        }
        return;  // 系统未启动时不执行后续代码
    }
    // 系统启动后的正常控制代码
    int8_t rawThrottle = throttleStick.getMapped();
    rawThrottle = max(0, rawThrottle);  // 将负值截断为0
    controlData.throttle = map(rawThrottle, 0, 100, 0, 100);
    controlData.direction = directionStick.getMapped();
    
    // 发送数据并等待ACK
    bool success = radio.write(&controlData, sizeof(controlData));
    bool signalGood = false;  // 信号状态标志
    
    if (success && radio.isAckPayloadAvailable()) {
        char ack;
        radio.read(&ack, sizeof(ack));
        if (ack == 'A') {
            lastAckTime = millis();
            signalGood = true;
            
            if (firstConnection) {
                signalLed.setState(LED::DOUBLE_FLASH);
                buzzer.setState(Buzzer::DOUBLE_BEEP);
                firstConnection = false;
            } else {
                signalLed.setState(LED::ON);
            }
        }
    }
    
    // 信号不好或超时时闪烁LED
    if (!signalGood || (millis() - lastAckTime > ackTimeout)) {
        signalLed.setState(LED::CONTINUOUS_FLASH);
    }
    
    // 每秒打印一次摇杆值
    if (millis() - lastDebugTime >= debugInterval) {
        DEBUG_PRINTF("油门: %d, 方向: %d\n", controlData.throttle, controlData.direction);
        DEBUG_PRINTF("油门原始值: %d, 方向原始值: %d\n", 
            throttleStick.getRaw(), 
            directionStick.getRaw());
        lastDebugTime = millis();
    }
    
    delay(20);  // 50Hz发送频率
} 