#include "eth_device.hpp"

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
    struct hostent *host;
    struct protoent *protocol;
    int  cache_size = 50 * 1024;
    
    if (this->ethFd > 0)
        close(this->ethFd);

    // Get icmp protocol
    if ((protocol = getprotobyname("icmp")) == NULL) {
        dbg_print("ERROR: Failed to get icmp protocol for %s\n",
                  this->name.data());
        return -1;
    }

    // Create a socket with icmp protocol
    if ((this->ethFd = socket(AF_INET,
                                  SOCK_RAW,
                                  protocol->p_proto)) < 0) {
        dbg_print("ERROR: Failed to create icmp socket for %s\n",
                  this->name.data());
        return -1;
    }

    // Set to current permit
    setuid(getuid());

    // Set ip address
    this->EthSetIP(this->ipAddr);
    
    // Set the cache to 50k for avoiding overflow,
    // If the destination address is an broadcast address
    // or a multi-cast address, will receive lots of ACK
    setsockopt(this->ethFd, SOL_SOCKET, SO_RCVBUF,
                   &cache_size, sizeof(cache_size));

    bzero(&this->dest_sin, sizeof(this->dest_sin));
    this->dest_sin.sin_family = AF_INET;

    if (inet_addr(this->destIP.data()) == INADDR_NONE) {
        // Destination address is a hostname
        if ((host = gethostbyname(this->destIP.data())) == NULL) {
            dbg_print("ERROR: Get host by name for %s failed\n",
                      this->name.data());
            return -1;
        }
    } else {
        this->dest_sin.sin_addr.s_addr = inet_addr(destIP.data());
    }

    this->pid = getpid();

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

int EthDevice::EthPackChkSum(const struct icmp &pack,
                             const int len)
{
    int nLeft = len;
    int sum = 0;
    unsigned short *w = (unsigned short *)&pack;
    unsigned short checkSum = 0;

    while (nLeft > 1) {
        sum += *w++;
        nLeft -= 2;
    }

    if (nLeft == 1)
        sum += *((unsigned char *)w);

    sum = (sum >> 16) + (sum & 0xFFFFF);
    sum += (sum >> 16);
    checkSum = ~sum;
    
    return checkSum;
}

int EthDevice::EthPack(struct icmp &pack,
                       const int number)
{
    int packSize;
    struct timeval *val;

    pack.icmp_type = ICMP_ECHO;
    pack.icmp_code  = 1;
    pack.icmp_cksum = 0;
    pack.icmp_seq = number;
    pack.icmp_id  = this->pid;

    packSize = 8 + sizeof(pack);
    val = (struct timeval *)pack.icmp_data;
    gettimeofday(val, NULL);

    pack.icmp_cksum = this->EthPackChkSum(pack, sizeof(pack));

    return packSize;
}

int EthDevice::EthSendPacket(struct sockaddr_in &dest_sin)
{
    struct icmp sendPack;
    int packSize;

    packSize = this->EthPack(sendPack, loop);
    
    if (sendto(this->ethFd, &sendPack, packSize, 0,
               (struct sockaddr *)&dest_sin,
               sizeof(dest_sin)) < 0) {
        dbg_print("ERROR: Send to %s failed (%s)\n",
                  inet_ntoa(dest_sin.sin_addr),
                  this->name.data());
        return -1;
    }
    
    return 0;
}

int EthDevice::EthRecvPacket(void)
{
    return 0;
}

bool EthDevice::TestDevice(void)
{
    while (loop) {
        this->EthSendPacket(this->dest_sin);
        this->EthRecvPacket();
    }
    
    return true;
}

EthDevice::~EthDevice()
{
    if (this->ethFd > 0)
        close(this->ethFd);
}
