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
    return true;
}
