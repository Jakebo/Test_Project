#include "test.hpp"
#include "common.hpp"

int test_process(void)
{
    return 0;
}

int test(void)
{
    if (test_process() == 0)
        printf("ERROR: ######### TEST FAILED ###########\n");
    else
        printf("ERROR: ######### TEST SUCCESS ##########\n");
    
    return 0;
}
