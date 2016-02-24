#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <stdarg.h>
#include <json/json.h>

extern const std::string DEVICE_NAME;
extern const std::string DEVICE_TYPE;
extern const std::string DEVICE_NODE;
extern const std::string DEVICE_BAUDRATE;
extern const std::string DEVICE_DATABIT;
extern const std::string DEVICE_PARITY;
extern const std::string DEVICE_STOPBIT;
extern const std::string DEVICE_RS485MODE;
extern const std::string DEVICE_TESTSTR;

struct options {
    bool quiet = false;
    bool auto_test  = false;
    std::string jsonFile;
};

extern struct options opts;

template <class Key, class T>
inline std::pair<Key, T> make_pair(const Key &key,
                                   const T t)
{
    return std::pair<Key, T>(key, t);
}

inline int dbg_print(const char *fmt, ...)
{
    if (!opts.quiet) {
        int ret;
        va_list args;

        va_start(args, fmt);
        ret = vfprintf(stdout, fmt, args);
        va_end(args);

        return ret;
    }

    return 0;
}

#endif
