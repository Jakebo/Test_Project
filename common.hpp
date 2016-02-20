#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <json/json.h>

extern const std::string error_msg[];
extern const std::string warning_msg[];

extern const std::string DEVICE_NAME;
extern const std::string DEVICE_TYPE;
extern const std::string DEVICE_NODE;
extern const std::string DEVICE_BAUDRATE;
extern const std::string DEVICE_DATABIT;
extern const std::string DEVICE_PARITY;
extern const std::string DEVICE_STOPBIT;

template <class Key, class T>
inline std::pair<Key, T> make_pair(const Key &key,
                                   const T t)
{
    return std::pair<Key, T>(key, t);
}

#endif
