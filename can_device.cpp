#include "can_device.hpp"

CanDevice::CanDevice(const Json::Value &can)
    : BaseDevice(can)
{
    
}

bool CanDevice::TestDevice(void)
{
    return true;
}
