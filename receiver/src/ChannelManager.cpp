#include "ChannelManager.h"

// 预定义通道（与发送端完全一致）
const uint8_t ChannelManager::CHANNELS[CHANNEL_COUNT] = {76, 66, 86, 56};

ChannelManager::ChannelManager(RF24& radio) 
    : _radio(radio)
    , _autoChannelEnabled(true)
    , _currentChannel(DEFAULT_CHANNEL)
    , _channelIndex(0)
    , _lastReceiveTime(0)
{
}

void ChannelManager::setAutoChannelEnabled(bool enabled) {
    _autoChannelEnabled = enabled;
}

bool ChannelManager::isAutoChannelEnabled() const {
    return _autoChannelEnabled;
}

uint8_t ChannelManager::getCurrentChannel() const {
    return _currentChannel;
}

void ChannelManager::switchToNextChannel() {
    if (!_autoChannelEnabled) return;
    
    // 在预定义的通道中循环切换
    _channelIndex = (_channelIndex + 1) % CHANNEL_COUNT;
    _currentChannel = CHANNELS[_channelIndex];
    
    // 应用新通道
    _radio.stopListening();
    _radio.setChannel(_currentChannel);
    _radio.startListening();
    
    // 打印调试信息
    Serial.print(F("切换到新通道: "));
    Serial.println(_currentChannel);
}

void ChannelManager::begin() {
    _radio.setChannel(_currentChannel);
    _lastReceiveTime = millis();
    
    Serial.print(F("初始通道: "));
    Serial.println(_currentChannel);
    Serial.print(F("智能调频: "));
    Serial.println(_autoChannelEnabled ? F("已启用") : F("已禁用"));
}

void ChannelManager::update() {
    if (_autoChannelEnabled && (millis() - _lastReceiveTime > RECEIVE_TIMEOUT)) {
        switchToNextChannel();
        _lastReceiveTime = millis();  // 重置计时器
    }
}

void ChannelManager::resetTimeout() {
    _lastReceiveTime = millis();
} 