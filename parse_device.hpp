#ifndef PARSE_DEVICE_HPP
#define PARSE_DEVICE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <json/json.h>

#include "base_device.hpp"

// Point to create function
typedef int (*create_device)(const Json::Value &d_value);

// Root of json file(global)
extern Json::Value root;
// device type <---> create device instance function
extern std::map<std::string, create_device> create_device_map;
// device name <---> device instance
extern std::map<std::string, BaseDevice *> device_map;

// open_jsonfile
int open_jsonfile(const std::string path, Json::Value *root);
// init_create_map
void init_create_map(void);
// parse_device
int parse_device(Json::Value &value);
// delete_devices
int delete_devices(void);
// display_devices
void display_devices(void);

#endif
