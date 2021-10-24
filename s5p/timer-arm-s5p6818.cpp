INTERFACE [arm && pf_s5p6818]:

#include "kmem.h"
#include "mmio_register_block.h"
#include "io.h"

EXTENSION class Timer : private Mmio_register_block {
public:
    enum {
        TCFG0      = 0x00,
        TCFG1      = 0x04,
        TCON       = 0x08,
        TCNTB0     = 0x0c,
        TCMPB0     = 0x10,
        TCNTO0     = 0x14,
        TCNTB1     = 0x18,
        TCMPB1     = 0x1c,
        TCNTO1     = 0x20,
        TCNTB2     = 0x24,
        TCMPB2     = 0x28,
        TCNTO2     = 0x2c,
        TCNTB3     = 0x30,
        TCMPB3     = 0x34,
        TCNTO3     = 0x38,
        TCNTB4     = 0x3c,
        TCNTO4     = 0x40,
        TINT_CSTAT = 0x44,
    };

private:
    enum { Timer_nr = 1 };

    static Static_object<Timer> _timer;
};

// -----------------------------------------------------------------------

INTERFACE [arm && pf_s5p6818]: 

EXTENSION class Timer
{
public:
    enum { Reload_value = 1000 };
};

// -----------------------------------------------------------------------

IMPLEMENTATION [arm && pf_s5p6818]:

#include "config.h"
#include "kip.h"
#include "io.h"

#include <cstdio>

Static_object<Timer> Timer::_timer;

PUBLIC static unsigned Timer::irq() {
    return 32 + 23 + Timer_nr;
}

PUBLIC static void Timer::configure(Cpu_number n) {}

PUBLIC Timer::Timer() : Mmio_register_block(Kmem::mmio_remap(Mem_layout::Timer_phys_base)) {
    /*
     * S5P6818 user's manual page 886
     */
    
    unsigned shift = (Timer_nr == 0) ? 0 : (Timer_nr * 4 + 4);
    
    // enable auto reload
    r<int unsigned>(TCON).set(1 << (shift + 3));
    
    // set TCNTB0 and TCMPB0
    r<int unsigned>(TCNTB0 + Timer_nr * 0xc) = Reload_value;
    r<int unsigned>(TCMPB0 + Timer_nr * 0xc) = Reload_value;

    // enable and diable manual update
    r<int unsigned>(TCON).set(1 << (shift + 1));
    r<int unsigned>(TCON).clear(1 << (shift + 1));
    
    // set the inverter bit
    r<int unsigned>(TCON).set(1 << (shift + 2));

    // enable interrupts
    r<unsigned int>(TINT_CSTAT).set(1 << Timer_nr);
    
    // start timer
    r<unsigned int>(TCON).set(1 << shift);
}

IMPLEMENT void Timer::init(Cpu_number cpu) {
    if (cpu == Cpu_number::boot_cpu())
        _timer.construct();
}

PUBLIC static inline void Timer::acknowledge() {
    // clear interrupt bit
    _timer->r<unsigned int>(TINT_CSTAT).set(1 << (5 + Timer_nr));
}

IMPLEMENT inline void Timer::update_one_shot(Unsigned64 wakeup) {
    (void) wakeup;
}

IMPLEMENT inline NEEDS["config.h", "kip.h"] Unsigned64 Timer::system_clock() {
    if (Config::Scheduler_one_shot)
        return 0;
    return Kip::k()->clock;
}
