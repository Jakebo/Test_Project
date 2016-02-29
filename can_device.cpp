#include "can_device.hpp"

CanDevice::CanDevice(const Json::Value &can)
    : BaseDevice(can)
{
    baudrate = can[DEVICE_BAUDRATE].asUInt();
    if (can[DEVICE_LOOP] != Json::nullValue)
        loop     = can[DEVICE_LOOP].asInt();
}

int CanDevice::CanInit(void)
{
    return 0;
}

bool CanDevice::TestDevice(void)
{
    return true;
}
