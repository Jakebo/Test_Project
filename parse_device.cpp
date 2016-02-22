#include <map>

#include "parse_device.hpp"
#include "uart_device.hpp"

// Root of json file(global)
Json::Value root;
// device type <---> create device instance function
std::map<std::string, create_device> create_device_map;
// device name <---> device instance
std::map<std::string, BaseDevice *> device_map;

//
// open_jsonfile
//   Open an json file and parse the root of this file
//   @path the path of json file
//   @root store the root of this file
//
int open_jsonfile(const std::string path, Json::Value *root)
{
    FILE *file = fopen(path.c_str(), "rb");
    if (!file)
        return -1;

    // To calculate the file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);

    // Get the text from file
    std::string text;
    char *buffer = new char[size + 1];
    buffer[size] = 0;
    fseek(file, 0, SEEK_SET);
    if (fread(buffer, 1, size, file) == (unsigned long)size)
        text = buffer;

    Json::Reader reader(Json::Features::strictMode());
    if (!reader.parse(text, *root)) {
        printf("Failed to parse %s file: \n %s\n",
               path.c_str(),
               reader.getFormattedErrorMessages().c_str());
        return -1;
    }
    
    fclose(file);
    delete []buffer;
    return 0;
}

//
// device_map_insert
//   Take an instance into the global  device map
//
static int device_map_insert(const Json::Value &value,
                             BaseDevice *device)
{
    device_map.insert(make_pair(value[DEVICE_NAME].asString(), device));
    return 0;
}

//
// create_uart
//   Create an uart instance, and insert into device map
//
static int create_uart(const Json::Value &uart)
{
    UartDevice *uartDevice = new UartDevice(uart);
    
    device_map_insert(uart, uartDevice);

    return 0;
}

//
// init_create_map
// Create the global map of creating function
//
void init_create_map(void)
{
    create_device_map.insert(make_pair(std::string("uart"),
                                       create_uart));
}

//
// parse_device
//   Get the devices that described in a root of json file
//
int parse_device(Json::Value &value)
{
    switch (value.type()) {
    case Json::nullValue:
        break;
    case Json::arrayValue:
    {
        // Travel the elements in array
        int size = value.size();
        for (int index = 0; index < size; ++index)
            parse_device(value[index]);        
    } break;
    case Json::objectValue:
    {
        // Create device instance according device type
        if (value[DEVICE_NAME] == Json::nullValue)
            dbg_print("WARNING: Should to specify the name for device\n");
        // Should specify a device type in json elements
        if (value[DEVICE_TYPE] == Json::nullValue) {
            printf("ERROR: Need to specify the device type for device\n");
            break;
        }

        dbg_print("INFO: Got device [%s]\n",
                  value[DEVICE_NAME].asString().c_str());
        // Create device instance according device type
        (*create_device_map[value[DEVICE_TYPE].asString()])(value);
    } break;
    default:
        break;
    }

    return 0;
}

//
// delete_devices
//   Free the memories that allocated for device instance,
//   store in device_map
//
int delete_devices(void)
{
    std::map<std::string, BaseDevice *>::iterator it = device_map.begin();

    for (; it != device_map.end(); ++it)
        delete it->second;
    
    return 0;
}

//
// display_devices
//
void display_devices(void)
{
    printf("=================================================\n");
    printf(">>> The devices list parsed from: %s\n",
           opts.jsonFile.c_str());
    printf("=================================================\n");

    int index = 1;
    std::map<std::string, BaseDevice *>::iterator it;
    for (it = device_map.begin();
         it != device_map.end();
         ++it, ++index) {
        printf("> %d. %-18s", index, it->first.c_str());
        printf("> %d. %s\n", ++index, (++it)->first.c_str());
    }
    printf("-------------------------------------------------\n");
}
