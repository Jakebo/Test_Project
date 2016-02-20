#ifndef BASE_DEVCIE_HPP
#define BASE_DEVICE_HPP

#include "common.hpp"

class BaseDevice {
public:
    BaseDevice(const Json::Value &device);
    
private:
    std::string name;
    std::string deviceType;
};

#endif
