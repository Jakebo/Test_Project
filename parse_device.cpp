#include <iostream>

#include "parse_device.hpp"

Json::Value root;

int open_jsonfile(const std::string path, Json::Value *value)
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
    if (!reader.parse(text, *value)) {
        printf("Failed to parse %s file: \n %s\n",
               path.c_str(),
               reader.getFormattedErrorMessages().c_str());
        return -1;
    }
    
    fclose(file);
    delete []buffer;
    return 0;
}

int parse_device(Json::Value &value)
{
    switch (value.type()) {
    case Json::arrayValue:
        int size = value.size();
        for (int index = 0; index < size; ++index)
            parse_device(value[index]);
        
        break;
    case Json::objectValue:
        break;
    }

    return 0;
}

