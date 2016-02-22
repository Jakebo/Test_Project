#include "uart_device.hpp"

UartDevice::UartDevice(const Json::Value &uart)
    : BaseDevice(uart)
{
    this->deviceNode = uart[DEVICE_NODE].asString();
    this->baudrate = uart[DEVICE_BAUDRATE].asInt();
    this->databit  = uart[DEVICE_DATABIT].asInt();
    this->parity   = uart[DEVICE_PARITY].asString();
    this->stopbit  = uart[DEVICE_STOPBIT].asInt();
    
    return;
}

bool UartDevice::TestDevice(void)
{
    std::cout << __func__ << std::endl;
    return false;
}

int UartDevice::OpenUart(void)
{
    return uartFd;
}