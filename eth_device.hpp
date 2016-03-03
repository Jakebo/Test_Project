#ifndef ETH_DEVICE_HPP
#define ETH_DEVICE_HPP

#include "common.hpp"
#include "base_device.hpp"

#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>

class EthDevice : public BaseDevice {
public:
    EthDevice(const Json::Value &eth);
    int EthInit(void);
    int EthSetIP(const std::string &ip);
    bool TestDevice(void);
    int  EthSendPacket(struct sockaddr_in &dest_sin);
    int  EthRecvPacket(void);
    int  EthPack(struct icmp &pack, const int number);
    int  EthPackChkSum(const struct icmp &pack, const int len);

    ~EthDevice(void);
private:
    int loop  = -1;
    int ethFd = -1;
    std::string ipAddr;
    std::string destIP;
    pid_t       pid;
    struct sockaddr_in dest_sin;

};

#endif
