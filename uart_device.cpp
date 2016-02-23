#include "uart_device.hpp"

#include <fcntl.h>

UartDevice::UartDevice(const Json::Value &uart)
    : BaseDevice(uart)
{
    this->deviceNode = uart[DEVICE_NODE].asString();
    this->baudrate = uart[DEVICE_BAUDRATE].asInt();
    this->databit  = uart[DEVICE_DATABIT].asInt();
    this->parity   = uart[DEVICE_PARITY].asString();
    this->stopbit  = uart[DEVICE_STOPBIT].asInt();

    if (uart[DEVICE_RS485MODE] != Json::nullValue)
        this->isRS485Mode = uart[DEVICE_RS485MODE].asBool();
    
    return;
}

bool UartDevice::TestDevice(void)
{
    dbg_print("Start test %s\n", this->name.c_str());

    if (this->uartFd < 0)
        if (this->UartInit() != 0)
            return false;
    
    return false;
}

int UartDevice::UartInit(void)
{
    this->uartFd = open(this->deviceNode.c_str(),
                        O_RDWR);
    if (this->uartFd < 0)
        return -1;
    
    return this->UartSetting();
}

int UartDevice::UartSetting(void)
{
    return 0;
}
