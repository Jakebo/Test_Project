#include "key_device.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

KeyDevice::KeyDevice(const Json::Value &key)
    : BaseDevice(key)
{
    if (key[DEVICE_NODE] != Json::nullValue)
        this->keyInputNode = key[DEVICE_NODE].asString();
    if (key[DEVICE_LOOP] != Json::nullValue)
        this->loop = key[DEVICE_LOOP].asInt();
    if (key[DEVICE_CODES] != Json::nullValue) {
        int size = key[DEVICE_CODES].size();
        for (int index = 0; index < size; ++index) {
            this->keyCodes[index] =
                key[DEVICE_CODES][index][DEVICE_CODE].asInt();
            ++this->keyCnt;
        }
    } else {
        dbg_print("INFO: Didn't specify keycodes\n");
    }
}

int KeyDevice::KeyInit(void)
{
    if (this->keyFd > 0)
        close(this->keyFd);

    this->keyFd = open(this->keyInputNode.data(), O_RDONLY);
    if (this->keyFd < 0)
        return -1;

    return 0;
}

int KeyDevice::KeyGetCodeIndex(const int code)
{
    for (int idx = 0; idx < this->keyCnt; ++idx)
        if (this->keyCodes[idx] == code)
            return idx;

    return -1;
}

bool KeyDevice::TestDevice(void)
{
    struct input_event ie;
    int idx = 0;

    this->keyPass = 0;
    
    if (this->keyCnt < 1)
        return true;

    if (this->keyFd < 0)
        if (this->KeyInit() < 0)
            return false;

    dbg_print("INFO: Press buttons each %d times\n", this->loop);
    
    while (true) {
        if (read(this->keyFd, &ie, sizeof(ie)) < 0) {
            dbg_print("ERROR: Failed to get an input event for %s\n",
                      this->name.data());
            return false;
        }

        idx = this->KeyGetCodeIndex(ie.code);
        if (idx != -1 && ie.value == 0) {
            dbg_print("INFO: Press %d\n", ie.code);
            ++this->keyPressTimes[idx];
            if (this->keyPressTimes[idx] >= this->loop)
                this->keyPass |= (1 << idx);
        }

        if (((1 << this->keyCnt) - 1) & this->keyPass)
            break;
    }
    
    return true;
}

KeyDevice::~KeyDevice()
{
    if (this->keyFd > 0)
        close(this->keyFd);
}
