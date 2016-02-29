#ifndef CAN_DEVICE_HPP
#define CAN_DEVICE_HPP

#include "common.hpp"
#include "base_device.hpp"

#include <linux/can.h>

typedef uint32_t can_baudrate_t;

class CanDevice : public BaseDevice {
public:
    CanDevice(const Json::Value &can);
    int CanInit(void);
    bool TestDevice(void);

private:
    can_baudrate_t *baudrate;
    int loop = -1;
};

#endif
