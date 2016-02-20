#include <map>

#include "parse_device.hpp"
#include "base_device.hpp"

typedef int (*create_device)(const Json::Value &d_value);
Json::Value root;
// device type <---> create device function
static std::map<std::string, create_device> create_device_map;
// device name <---> device
static std::map<std::string, BaseDevice> device_map;

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

static int create_uart(const Json::Value &uart)
{
    printf("%s\n", __func__);
    return 0;
}

void init_create_map(void)
{
    create_device_map.insert(make_pair(std::string("uart"),
                                       create_uart));
}

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
        if (value["name"] == Json::nullValue)
            printf("%s\n", warning_msg[0].c_str());
        
        if (value["device_type"] == Json::nullValue) {
            printf("%s\n", error_msg[0].c_str());
            break;
        }
        (*create_device_map[value["device_type"].asString()])(value);
    } break;
    default:
        break;
    }

    return 0;
}
