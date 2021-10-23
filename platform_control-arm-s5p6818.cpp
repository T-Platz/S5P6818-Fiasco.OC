IMPLEMENTATION [arm && mp && pf_s5p6818]:

#include "cpu.h"
#include "mem.h"
#include "mmio_register_block.h"
#include "kmem.h"

#include <cstdio>

PUBLIC static void Platform_control::boot_ap_cpus(Address phys_tramp_mp_addr) {
    for (int i = 0; i < 8; ++i) {
        unsigned coreid[8] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7 };
        int r = cpu_on(coreid[i], phys_tramp_mp_addr);
        if (r) {
            if (r != Psci_already_on)
                printf("CPU%d boot-up error: %d\n", i, r);
            continue;
        }
    }
}
