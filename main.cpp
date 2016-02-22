#include <unistd.h>
#include <iostream>

#include "common.hpp"
#include "parse_device.hpp"
#include "test.hpp"

void display_usage(const char *name)
{
    std::cout << name << " - test functions on board" << std::endl;

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char opt;
    std::string optstring = "ad:q";

    opt = getopt(argc, argv, optstring.c_str());
    while (opt != -1) {
        switch (opt) {
        case 'a':
            opts.auto_test = true;
            break;
        case 'd':
            opts.jsonFile = optarg;
            break;
        case 'q':
            opts.quiet = true;
            break;
        default:
            break;
        }

        opt = getopt(argc, argv, optstring.c_str());
    }

    if (opts.jsonFile.length() == 0)
        display_usage(argv[0]);

    init_create_map();

    if (open_jsonfile(opts.jsonFile, &root) != 0)
        return 1;
    
    parse_device(root);
    display_devices();

    test_process();
    
    return 0;
}
