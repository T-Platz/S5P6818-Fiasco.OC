#ifndef L4_DRIVERS_UART_S5P6818_H__
#define L4_DRIVERS_UART_S5P6818_H__

#include "uart_base.h"

namespace L4 {
    class Uart_s5p6818 : public Uart {
    public:
        Uart_s5p6818() {}
        bool startup(Io_register_block const *) override;
        void shutdown() override;
        bool change_mode(Transfer_mode m, Baud_rate r) override;
        int get_char(bool blocking = true) const override;
        int char_avail() const override;
        inline void out_char(char c) const;
        int write(char const *s, unsigned long count) const override;
        void fifo_reset();

    protected:
        void ack_rx_irq() const;
        void wait_for_empty_tx_fifo() const;
        void wait_for_non_full_tx_fifo() const;
        unsigned is_rx_fifo_non_empty() const;
    };
};

#endif
