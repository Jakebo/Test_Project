#include "uart_device.hpp"

#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

#ifndef TIOCSRS485
    #define TIOCSRS485     0x542F
#endif

#ifndef TIOCGRS485
    #define TIOCGRS485     0x542E
#endif

UartDevice::UartDevice(const Json::Value &uart)
    : BaseDevice(uart)
{
    this->deviceNode = uart[DEVICE_NODE].asString();
    this->baudrate = uart[DEVICE_BAUDRATE].asInt();
    this->databit  = uart[DEVICE_DATABIT].asInt();
    this->parity   = uart[DEVICE_PARITY].asString();
    this->stopbit  = uart[DEVICE_STOPBIT].asInt();

    if (uart[DEVICE_RS485MODE] != Json::nullValue)
        this->isRS485Mode = uart[DEVICE_RS485MODE].asBool();
    if (uart[DEVICE_TESTSTR] != Json::nullValue)
        this->testStr = uart[DEVICE_TESTSTR].asString();
    if (uart[DEVICE_LOOP] != Json::nullValue)
        this->loop = uart[DEVICE_LOOP].asInt();

    return;
}

bool UartDevice::TestDevice(void)
{
    this->testResult = false;

    if (this->uartFd < 0)
        if (this->UartInit() != 0)
            return false;

    if (this->UartTestTransmit() != 0)
        return false;

    this->testResult = true;
    
    return true;
}

bool UartDevice::Uart485Enable(bool enable)
{
    struct serial_rs485 rs485conf;
    int res;

    dbg_print("INFO: Enable rs485 for %s\n", this->deviceNode.data());
    /* Get configure from device */
    res = ioctl(this->uartFd, TIOCGRS485, &rs485conf);
    if (res < 0) {
        dbg_print("ERROR: Ioctl error on getting 485 configure %s",
                  this->deviceNode.data());
        return false;
    }

    /* Set enable/disable to configure */
    if (enable) {   // Enable rs485 mode
        rs485conf.flags |= SER_RS485_ENABLED;
    } else {        // Disable rs485 mode
        rs485conf.flags &= ~(SER_RS485_ENABLED);
    }

    rs485conf.delay_rts_before_send = 0x00000004;

    /* Set configure to device */
    res = ioctl(this->uartFd, TIOCSRS485, &rs485conf);
    if (res < 0) {
        dbg_print("ERROR: Ioctl error on setting 485 configure %s",
                  this->deviceNode.data());
        return false;
    }

    
    return true;
}

int UartDevice::UartInit(void)
{
    if (this->uartFd > 0)
        close(this->uartFd);
    
    this->uartFd = open(this->deviceNode.c_str(),
                        O_RDWR);
    if (this->uartFd < 0) {
        dbg_print("ERROR: Failed to open %s\n",
                  this->name.c_str());
        return -1;
    }

    return this->UartSetting();
}

int UartDevice::UartSetting(void)
{
    struct termios cfg;
    if (tcgetattr(this->uartFd, &cfg) != 0) {
        dbg_print("ERROR: Failed to get attribute from %s\n",
                  this->name.c_str());
        return -1;
    }
    cfmakeraw(&cfg);

    cfg.c_cflag &= ~CSIZE;

    // Set buadrate
    int speed = 0;
    switch (this->baudrate) {
    case 2400:
        speed = B2400;
        break;
    case 4800:
        speed = B4800;
        break;
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
    case 115200:
        speed = B115200;
        break;
    }
    cfsetispeed(&cfg, speed);
    cfsetospeed(&cfg, speed);

    // Set databit
    switch (this->databit) {
    case 7:
        cfg.c_cflag |= CS7;
        break;
    default:
    case 8:
        cfg.c_cflag |= CS8;
        break;
    }

    // Set parity
    switch (this->parity.c_str()[0]) {
    case 'o':
    case 'O':
        cfg.c_cflag |= (PARODD | PARENB);
        cfg.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E':
        cfg.c_cflag |= PARENB;
        cfg.c_cflag &= ~PARODD;
        cfg.c_iflag |= INPCK;
        break;
    case 's':
    case 'S':
        cfg.c_cflag &= ~(PARENB | CSTOPB);
        break;
    default:
    case 'n':
    case 'N':
        cfg.c_cflag &= ~PARENB;
        cfg.c_iflag &= ~INPCK;
        break;
    }

    // Set stopbit
    switch (this->stopbit) {
    default:
    case 1:
        cfg.c_cflag &= ~CSTOPB;
        break;
    case 2:
        cfg.c_cflag |= CSTOPB;
        break;
    }

//    cfg.c_cc[VMIN] = this->testStr.length() - 1;
    
    tcflush(this->uartFd, TCIFLUSH);
    // Enable the setting now
    if ((tcsetattr(this->uartFd, TCSANOW, &cfg)) != 0) {
        dbg_print("Failed to set attribute to %s\n",
                  this->name.c_str());;
        return -1;
    }

    // @TODO: Add rs485 support
    if (this->isRS485Mode)
        if (!this->Uart485Enable(true))
            return -1;
    
    return 0;
}

int UartDevice::UartCheckString(const char *base, const char *buf)
{
    static int idx = 0;
    static bool first = true;;
    int baseLen = strlen(base);
    int bufLen = strlen(buf);
    int bufIdx = 0;

    while (bufIdx < bufLen) {
        if (buf[bufIdx] != base[idx]) {
            for (idx = 0; idx < baseLen; ++idx)
                if (buf[bufLen - 1] == base[idx])
                    break;
            if (idx == baseLen) {
                idx = 0;
                return -1;
            }
            idx = (idx + 1) % baseLen;
            if (first) {
                first = false;
                return 0;
            } else
                return -1;
        }

        ++idx;
        idx = idx % baseLen;
        ++bufIdx;
    }

    return 0;
}

int UartDevice::UartTestTransmit(void)
{
    char recvBuf[1024] = { 0 };
    int  tries = 0;
    long xmitCnt = 0;
    long recvCnt = 0;
    int  ret;

    struct timeval tv;
    fd_set fds;

    tv.tv_sec = this->timeout;
    tv.tv_usec = 0;

    while (loop == -1 || loop--) {
        // Transmit data
        if ((ret = write(this->uartFd,
                         this->testStr.data(),
                         this->testStr.length())) < 0) {
            ++tries;
            dbg_print("WARNING: Transmit data on %s failed, retry %d%s times\n",
                      this->name.c_str(),
                      tries,
                      (tries == 2) ? "nd" : "th");
            if (tries > 3)
                dbg_print("ERROR: Transmit data on %s failed, retry times exceed"
                          "%d, Cancel transmitting this packet and receive data directly\n",
                          this->name.c_str(),
                          tries);
            else
                continue;
        }
        tries = 0;
        xmitCnt += ret;
        dbg_print("INFO: transmit: %s, %d\n", testStr.data(), xmitCnt);

        usleep(100000);
        
        // Receive data
        memset(recvBuf, '\0', sizeof(recvBuf));
        
        if (this->timeout > 0) {
            FD_ZERO(&fds);
            FD_SET(this->uartFd, &fds);

            ret = select(this->uartFd + 1, &fds, NULL, NULL, &tv);
            if (ret == -1 || ret == 0) {
                ++tries;
                dbg_print("WARNING: Receive data on %s failed, retry %d%s times\n",
                          this->name.c_str(),
                          tries,
                          tries == 2 ? "nd" : "th");
            }
            if (tries > 3) {
                dbg_print("ERROR: Receive data on %s failed, retry times exceed %d,"
                          "Cancel receiving a packet and send data again\n",
                          this->name.c_str(),
                          tries);
                continue;
            }
        }

        tries = 0;
        ret = read(this->uartFd, recvBuf, sizeof(recvBuf));
        if (ret < 0) {
            dbg_print("ERROR: Read data from %s failed, Skip this reading and send data again\n",
                      this->name.c_str());
            continue;
        }

        recvCnt += ret;
        dbg_print("INFO: Receive:  %s, %d\n", recvBuf, recvCnt);

        // Verify data
        if (this->UartCheckString(this->testStr.data(), recvBuf) != 0) {
            // TODO
        }
    }

    return 0;
};

UartDevice::~UartDevice()
{
    if (this->uartFd > 0)
        close(this->uartFd);
}
