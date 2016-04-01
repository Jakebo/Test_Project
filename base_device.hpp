#ifndef BASE_DEVICE_HPP
#define BASE_DEVICE_HPP

#include "common.hpp"

class BaseDevice {
public:
    BaseDevice(const Json::Value &device);
    bool GetTestResult(void);
    virtual bool TestDevice(void) = 0;

    // If you didn't implemented the virtual destructor, the
    // default destructor is non-virtual, so you will get below
    // warning while compiling:
    // warning: deleting object of polymorphic class type
    // ‘BaseDevice’ which has non-virtual destructor might
    // cause undefined behaviour [-Wdelete-non-virtual-dtor]
    virtual ~BaseDevice();
    
protected:
    std::string name;
    std::string deviceType;

    bool testResult = false;
};

#endif
