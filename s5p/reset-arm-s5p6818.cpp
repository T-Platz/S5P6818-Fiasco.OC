IMPLEMENTATION [arm && pf_s5p6818]:

#include "platform_control.h"

void __attribute__ ((noreturn)) platform_reset(void) {
    Platform_control::system_reset();

    for (;;)
        ;
}
