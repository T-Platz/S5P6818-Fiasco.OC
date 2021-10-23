INTERFACE [arm && pf_s5p6818]:

EXTENSION class Mem_layout {
public:
    enum Phys_layout_s5p6818: Address {
        // starts with GICD_CTRL
        // page 475 of the S5P6818 user's manual
        Gic_dist_phys_base   = 0xc0009000,
        
        // starts with GICC_CTRL
        // page 476
        Gic_cpu_phys_base    = 0xc000a000,
        
        // page 890
        Timer_phys_base      = 0xc0017000,
        
        // page 501
        Watchdog_phys_base   = 0xc0019000,
        
        // channel 0 is used
        // page 971
        Uart_phys_base       = 0xc00a1000,
    };
};
