INTERFACE [arm && pf_s5p6818]:

// on ARM, the MMIO for the uart is accessible before the MMU is fully up
EXTENSION class Kernel_uart {
    enum { Bsp_init_mode = Init_before_mmu };
};

IMPLEMENTATION [arm && serial && pf_s5p6818]:

IMPLEMENT
bool Kernel_uart::startup(unsigned int, int) {
    return Uart::startup();
}
