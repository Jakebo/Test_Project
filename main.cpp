#include <unistd.h>
#include <iostream>

#include "parse_device.hpp"

void display_usage(const char *name)
{
    std::cout << name << " - test functions on board" << std::endl;

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char opt;
    std::string optstring = "qd:";
    std::string d_file = "";

    opt = getopt(argc, argv, optstring.c_str());
    while (opt != -1) {
        switch (opt) {
        case 'd':
            d_file = optarg;
            break;
        default:
            break;
        }

        opt = getopt(argc, argv, optstring.c_str());
    }

    if (d_file.length() == 0)
        display_usage(argv[0]);

    init_create_map();

    if (open_jsonfile(d_file, &root) == 0)
        parse_device(root);

    return 0;
}
