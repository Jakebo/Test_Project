#ifndef UART_DEVICE_HPP
#define UART_DEVICE_HPP

#include "common.hpp"
#include "base_device.hpp"

class UartDevice : public BaseDevice {
public:
    UartDevice(const Json::Value &uart);
    bool TestDevice(void);

    int  OpenUart(void);

private:
    std::string deviceNode;
    std::string baudrate;
    std::string parity;
    int databit;
    int stopbit;

    int uartFd = -1;;
};
    
#endif
