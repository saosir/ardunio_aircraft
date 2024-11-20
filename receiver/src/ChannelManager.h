#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <RF24.h>

class ChannelManager {
public:
    ChannelManager(RF24& radio);
    
    // 设置是否启用智能调频
    void setAutoChannelEnabled(bool enabled);
    bool isAutoChannelEnabled() const;
    
    // 获取当前通道
    uint8_t getCurrentChannel() const;
    
    // 切换到下一个通道
    void switchToNextChannel();
    
    // 初始化通道管理器
    void begin();
    
    // 检查是否需要切换通道（在loop中调用）
    void update();
    
    // 重置接收超时计时器（收到数据时调用）
    void resetTimeout();

private:
    RF24& _radio;
    bool _autoChannelEnabled;
    uint8_t _currentChannel;
    uint8_t _channelIndex;
    unsigned long _lastReceiveTime;
    
    // 预定义的几个通道（需要和发送端完全一致）
    static const uint8_t CHANNEL_COUNT = 4;
    static const uint8_t CHANNELS[CHANNEL_COUNT];
    static const uint8_t DEFAULT_CHANNEL = 76;
    static const unsigned long RECEIVE_TIMEOUT = 3000; // 3秒无数据就切换通道
};

#endif 