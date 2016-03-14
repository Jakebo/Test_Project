#ifndef UART_DEVICE_HPP
#define UART_DEVICE_HPP

#include "common.hpp"
#include "base_device.hpp"

class UartDevice : public BaseDevice {
public:
    UartDevice(const Json::Value &uart);
    bool TestDevice(void);

    int  UartInit(void);
    int  UartSetting(void);
    bool Uart485Enable(bool enable);
    int  UartCheckString(const char *base, const char *buf);
    int  UartTestTransmit(void);
    
    ~UartDevice();
private:
    std::string deviceNode;
    int baudrate;
    std::string parity;
    int  databit;
    int  stopbit;
    bool isRS485Mode = false;
    int  loop = -1;
    int  timeout = -1;
    std::string testStr = "0123456789";

    int uartFd = -1;
};
    
#endif
