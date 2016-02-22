#include "base_device.hpp"

BaseDevice::BaseDevice(const Json::Value &device)
{
    this->name = device[DEVICE_NAME].asString();
    this->deviceType = device[DEVICE_TYPE].asString();
    
    return;
}

bool BaseDevice::GetTestResult(void)
{
    return this->testResult;
}

BaseDevice::~BaseDevice()
{
}
