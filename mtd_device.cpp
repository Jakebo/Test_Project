#include "mtd_device.hpp"

#include <fcntl.h>
#include <string.h>

MtdDevice::MtdDevice(const Json::Value &mtd)
    : BaseDevice(mtd)
{
    if (mtd[DEVICE_NODE] != Json::nullValue)
        this->node = mtd[DEVICE_NODE].asString();
    else
        dbg_print("ERROR: Should specify a mtd device node\n");
    if (mtd[DEVICE_TESTSTR] != Json::nullValue)
        this->testStr = mtd[DEVICE_TESTSTR].asString();
}

bool MtdDevice::MtdRWTest(const std::string &mtd)
{
    std::string command = "flash_erase " + mtd + " 0 64 > /dev/null";
    int ret = 0;
    int mtdFd;

    this->testResult = false;
    
    dbg_print("INFO: Flashing %s...\n", mtd.data());
    ret = system(command.data());
    if ((ret = WEXITSTATUS(ret)) != 0) {
        dbg_print("ERROR: Failed to flash %s\n", mtd.data());
        return false;
    }

    mtdFd = open(mtd.data(), O_RDWR);
    if (mtdFd < 0) {
        dbg_print("ERROR: Failed to open %s\n", mtd.data());
        return false;
    }

    ret = write(mtdFd, this->testStr.data(), this->testStr.length());
    if (ret < (signed int)this->testStr.length()) {
        dbg_print("ERROR: Failed to write to %s\n", mtd.data());
        return false;
    }

    // Read back
    char *readBack = new char[this->testStr.length() + 1];
    
    lseek(mtdFd, SEEK_SET, 0);
    memset(readBack, '\0', this->testStr.length() + 1);

    read(mtdFd, readBack, this->testStr.length());

    printf("%d, %s\n", this->testStr.length(), readBack);
    if (this->testStr != readBack) {
        delete []readBack;
        return false;
    } else {
        delete []readBack;
        this->testResult = true;
        return true;
    }
}    

bool MtdDevice::TestDevice(void)
{
    if (this->node.length() == 0)
        return false;
    
    return this->MtdRWTest(this->node);
}

MtdDevice::~MtdDevice()
{
}
