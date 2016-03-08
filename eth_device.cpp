#include "eth_device.hpp"
#include "ping.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>

EthDevice::EthDevice(const Json::Value &eth)
    : BaseDevice(eth)
{
    this->ipAddr = eth[DEVICE_IPADDR].asString();
    this->destIP = eth[DEVICE_DESTIP].asString();
    
    if (eth[DEVICE_LOOP] != Json::nullValue)
        this->loop = eth[DEVICE_LOOP].asInt();
}

int EthDevice::EthInit(void)
{
    // Set ip address
    this->EthSetIP(this->ipAddr);
    
    return 0;
}

int EthDevice::EthSetIP(const std::string &ip)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in *sin;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        dbg_print("ERROR: Failed to create socket for %s\n",
                  this->name.data());
        return -1;
    }
    
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, this->name.data());

    sin = (struct sockaddr_in *)&(ifr.ifr_addr);
    sin->sin_addr.s_addr = inet_addr(this->ipAddr.data());
    sin->sin_family = AF_INET;

    // Set ip address
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
        dbg_print("ERROR: Failed to set address for %s\n",
                  this->name.data());
        return -1;
    }

    // Get & Set flags of interface
    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
        dbg_print("ERROR: Failed to get flags of %s\n",
                  this->name.data());
        return -1;
    }
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
        dbg_print("ERROR: Failed to set flags to %s\n",
                  this->name.data());
        return -1;
    }

    // Sleep two seconds for waiting eth PHY ready */
    sleep(2);

    return 0;
}

bool EthDevice::TestDevice(void)
{
    int failCount = 0;
    int total = this->loop;

    this->EthInit();
    
    while (--(this->loop)) {
        if (ping(this->destIP.data()) == 0) {
            dbg_print("INFO: Use %s to ping %s is OK\n",
                      this->name.data(),
                      this->destIP.data());
        } else {
            ++failCount;
            dbg_print("ERROR: Use %s to ping %s is failed\n",
                      this->name.data(),
                      this->destIP.data());
        }
    }

    dbg_print("INFO: Use %s to ping %d times, success: %d, fail: %d\n",
              this->name.data(), total, total - failCount, failCount);
    
    if (failCount > 0)
        return false;

    return true;
}

EthDevice::~EthDevice()
{

}
