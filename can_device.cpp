#include "can_device.hpp"

#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

CanDevice::CanDevice(const Json::Value &can)
    : BaseDevice(can)
{
    this->baudrate = can[DEVICE_BAUDRATE].asUInt();
    if (can[DEVICE_LOOP] != Json::nullValue)
        this->loop = can[DEVICE_LOOP].asInt();
}

int CanDevice::CanInit(void)
{
    struct ifreq ifr;
    can_baudrate_t *can_baudrate;

    if (this->canFd > 0)
        close(this->canFd);
    
    this->canFd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (this->canFd) {
        dbg_print("ERROR: Failed to create socketcan for %s\n",
                  this->name.data());
        return -1;
    }

    strncpy(ifr.ifr_name, this->name.data(), IFNAMSIZ);
    if (ioctl(this->canFd, SIOCGIFINDEX, &ifr)) {
        dbg_print("ERROR: Could not get the interface index of %s\n",
                  this->name.data());
        return -1;
    }

    can_baudrate = (can_baudrate_t *)&ifr.ifr_ifru;
    *can_baudrate = this->baudrate;
    if (ioctl(this->canFd, SIOCSCANBAUDRATE, &ifr)) {
        dbg_print("ERROR: Could not set the baudrate for %s\n",
                  this->name.data());
        return -1;
    }
    
    return 0;
}

bool CanDevice::TestDevice(void)
{
    if (this->canFd < 0)
        if (CanInit() != 0)
            return false;
    
    return true;
}

CanDevice::~CanDevice()
{
    if (this->canFd > 0)
        close(this->canFd);
}
