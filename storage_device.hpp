#ifndef STORAGE_DEVICE_HPP
#define STORAGE_DEVICE_HPP

#include "common.hpp"
#include "base_device.hpp"

class StorageDevice : public BaseDevice {
public:
    StorageDevice(const Json::Value &storage);
    std::string StorageMountPoint(const std::string &device);
    bool StorageRWTest(const std::string &path);
    bool TestDevice(void);
    
    ~StorageDevice();

private:
    std::string node;
    std::string testStr = "MYiR storage test";
};

#endif
