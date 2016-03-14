#ifndef MTD_DEVICE_HPP
#define MTD_DEVICE_HPP

#include "common.hpp"
#include "base_device.hpp"

class MtdDevice : public BaseDevice {
public:
    MtdDevice(const Json::Value &mtd);
    bool TestDevice(void);
    bool MtdRWTest(const std::string &mtd);

    ~MtdDevice();
    
private:
    std::string node;
    std::string testStr = "MYiR MTD devcie Read&Write test";
};

#endif
