#include "uart_device.hpp"

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

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
    dbg_print("Start test %s\n", this->name.c_str());

    if (this->uartFd < 0)
        if (this->UartInit() != 0)
            return false;
    
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

    tcflush(this->uartFd, TCIFLUSH);
    // Enable the setting now
    if ((tcsetattr(this->uartFd, TCSANOW, &cfg)) != 0) {
        dbg_print("Failed to set attribute to %s\n",
                  this->name.c_str());;
        return -1;
    }

    return 0;
}

UartDevice::~UartDevice()
{
    if (this->uartFd > 0)
        close(this->uartFd);
}
