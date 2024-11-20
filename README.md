# Arduino 固定翼航模

## 项目说明

- 固定翼航模，两个空心杯电机驱动，通过双电机差速控制飞行
- 遥控器为Arduino UNO，接收机为Arduino nano，通过nrf24l01无线模块通信

## 项目结构

### 共用代码 (shared)
- LED控制库
- 其他共用功能

### 遥控器 (transmitter)
- 基于Arduino UNO开发板
- 使用nrf24l01无线模块发送控制信号
- 包含按键控制功能

### 接收机 (receiver)
- 基于Arduino Nano开发板
- 使用nrf24l01无线模块接收信号
- 控制双电机差速实现飞行控制

## 开发环境
- Arduino IDE
- 依赖库：
  - RF24
  - SPI

## 编译说明
- 遥控器程序：打开 transmitter/transmitter.ino
- 接收机程序：打开 receiver/receiver.ino
- 