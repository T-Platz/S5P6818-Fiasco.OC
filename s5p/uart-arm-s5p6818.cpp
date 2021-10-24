IMPLEMENTATION [arm && pf_s5p6818]:

#include "koptions.h"
#include "uart_s5p6818.h"

IMPLEMENT L4::Uart *Uart::uart() {
    static L4::Uart_s5p6818 uart;
    return &uart;
}

IMPLEMENT Address Uart::base() const {
    return Koptions::o()->uart.base_address;
}

IMPLEMENT int Uart::irq() const {
    return Koptions::o()->uart.irqno;
}
