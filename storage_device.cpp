#include "storage_device.hpp"

#include <fstream>
#include <string.h>

StorageDevice::StorageDevice(const Json::Value &storage)
    : BaseDevice(storage)
{
    if (storage[DEVICE_NODE] != Json::nullValue)
        this->node = storage[DEVICE_NODE].asString();
    else
        this->node = "/dev/" + storage[DEVICE_NAME].asString();
}

//
// StorageMountPoint
//   Get the mountpoint from /proc/mounts
//     if the corresponding mountpoint is not exists this file,
//     return an empty string
//
std::string StorageDevice::StorageMountPoint(const std::string &device)
{
    std::ifstream ifs;
    char line[1024];

    ifs.open("/proc/mounts");

    while (!ifs.eof()) {
        memset(line, '\0', sizeof(line));
        ifs.getline(line, sizeof(line), '\n');
        std::string tmp = line;
        if (tmp.find(this->node, 0) == std::string::npos)
            continue;

        size_t pos = tmp.find(" ", this->node.length() + 1);
        if (pos == std::string::npos)
            continue;

        ifs.close();
        
        return tmp.substr(this->node.length() + 1,
                          pos - this->node.length() - 1);
    }

    ifs.close();
    
    return "";
}

bool StorageDevice::StorageRWTest(const std::string &path)
{
    std::string newFile = path + "/test.txt";
    std::fstream fs;
    char readBack[128] = { '\0' };

    fs.open(newFile, std::fstream::trunc | std::ios_base::out | std::ios_base::in);

    fs.write(this->testStr.data(), this->testStr.length());
    fs.seekg(0, std::fstream::beg);
    fs.read(readBack, this->testStr.length());

    fs.close();
    
    if (readBack != this->testStr)
        return false;
    
    return true;
}

bool StorageDevice::TestDevice(void)
{
    this->testResult = false;
    
    if (!file_exists(this->node.data())) {
        dbg_print("ERROR: %s is not exists\n",
                  this->node.data());
        return false;
    }

    std::string mountPath = this->StorageMountPoint(this->node);
    if (mountPath.length() < 1)
        // @TODO: try to mount the storage device
        ;

    this->testResult = this->StorageRWTest(mountPath);
    return this->testResult;
}

StorageDevice::~StorageDevice()
{
}
