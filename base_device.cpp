#include "base_device.hpp"

BaseDevice::BaseDevice(const Json::Value &device)
{
    this->name = device[DEVICE_NAME].asString();
    this->deviceType = device[DEVICE_TYPE].asString();
    
    return;
}
/*
bool BaseDevice::TestDevice(void)
{
    return true;
}
*/
BaseDevice::~BaseDevice()
{
}
