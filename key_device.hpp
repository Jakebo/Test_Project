#ifndef KEY_DEVICE_HPP
#define KEY_DEVICE_HPP

#include "common.hpp"
#include "base_device.hpp"

class KeyDevice : public BaseDevice {
public:
    KeyDevice(const Json::Value &keys);
    int  KeyInit(void);
    int  KeyGetCodeIndex(const int code);
    bool TestDevice(void);
    
    ~KeyDevice();
    
private:
    int keyFd = -1;
    int loop  = 1;
    int keyCnt = 0;
    int keyCodes[32] = { 0 };
    int keyPressTimes[32] = { 0 };
    int keyPass = 0;
    std::string keyInputNode = "/dev/input/event0";
};

#endif
