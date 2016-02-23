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
    

private:
    std::string deviceNode;
    std::string baudrate;
    std::string parity;
    int  databit;
    int  stopbit;
    bool isRS485Mode = false;

    int uartFd = -1;;
};
    
#endif
