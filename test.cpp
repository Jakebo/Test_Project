#include "test.hpp"
#include "parse_device.hpp"
#include "common.hpp"

static const  char *OK = "\033[32mOK\033[0m";
static const char *FAIL = "\033[31mFAIL\033[0m";

//
// test_single
//   Test the device accroding the device name input
//
static int test_single(void)
{
    std::string device;
    bool result;
    
    while (true) {
        std::cout << "Input device name>> ";
        std::cin >> device;

        result = device_map[device]->TestDevice();
        printf("%s...[%s]\n", device.c_str(),
               result ? OK : FAIL);
               
    }
    
    return 0;
}

//
// test_sequence
//   Test all the devices one by on
//   @return Return the count of testing failed
//
static int test_sequence(void)
{
    bool result;
    int  failCount = 0;
    std::map<std::string, BaseDevice *>::iterator it;
    
    for (it = device_map.begin();
         it != device_map.end();
         ++it) {
        result = (it->second)->TestDevice();
        printf("%s...[%s]\n",
               (it->first).c_str(),
               result ? OK : FAIL);
        result ? : ++failCount;
    }

    return failCount;
}

//
// test_process
//   Start test device
//
int test_process(void)
{
    if (opts.auto_test) {
        if (test_sequence() == 0)
            printf("\033[32mINFO: ########## TEST SUCCESS ###########\033[0m\n");
        else
            printf("\033[31mERROR: ######### TEST FAILED ############\033[0m\n");
    } else {
        test_single();
    }
    
    return 0;
}
