#ifndef PARSE_DEVICE_HPP
#define PARSE_DEVICE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <json/json.h>

extern Json::Value root;

// open_jsonfile
int open_jsonfile(const std::string path, Json::Value *value);
// parse_device
int parse_device(Json::Value &value);

#endif