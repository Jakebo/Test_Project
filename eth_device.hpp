#ifndef ETH_DEVICE_HPP
#define ETH_DEVICE_HPP

#include "common.hpp"
#include "base_device.hpp"

#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/ip.h>

class EthDevice : public BaseDevice {
public:
    EthDevice(const Json::Value &eth);
    int EthInit(void);
    int EthSetIP(const std::string &ip);
    bool TestDevice(void);

    ~EthDevice(void);
private:
    int loop  = -1;
    std::string ipAddr;
    std::string destIP;
};

#endif
