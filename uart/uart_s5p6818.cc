#include "uart_s5p6818.h"
#include "poll_timeout_counter.h"

namespace L4 {
    enum {
        ULCON    = 0x0,  // line control register
        UCON     = 0x4,  // control register
        UFCON    = 0x8,  // FIFO control register
        UMCON    = 0xc,  // modem control register
        UTRSTAT  = 0x10, // TX/RX status register
        UERSTAT  = 0x14, // RX error status register
        UFSTAT   = 0x18, // FIFO status register
        UMSTAT   = 0x1c, // modem status register
        UTXH     = 0x20, // transmit buffer register (little endian, 0x23 for BE)
        URXH     = 0x24, // receive buffer register (little endian, 0x27 for BE)
        UBRDIV   = 0x28, // baud rate divisor register
        UFRACVAL = 0x2c,
        UINTP    = 0x30, // interrupt pending register
        UINTS    = 0x34, // interrupt source register
        UINTM    = 0x38, // interrupt mask register

        ULCON_8N1_MODE = 0x3,

        UCON_MODE_RECEIVE_IRQ_POLL  = 1 << 0,
        UCON_MODE_TRANSMIT_IRQ_POLL = 1 << 2,
        UCON_SEND_BREAK_SIGNAL      = 1 << 4,
        UCON_LOOPBACK_MODE          = 1 << 5,
        UCON_RX_ERROR_STATUS_IRQ_EN = 1 << 6,
        UCON_RX_TIME_OUT_EN         = 1 << 7,
        UCON_MODE =   UCON_MODE_RECEIVE_IRQ_POLL
                      | UCON_MODE_TRANSMIT_IRQ_POLL
                      | UCON_RX_TIME_OUT_EN,

        UFCON_ENABLE        = 0x3, // enable FIFO
        UFCON_RX_FIFO_RESET = 0x0, // RX FIFO reset
        UFCON_TX_FIFO_RESET = 1 << 2, // TX FIFO reset

        UMCON_AFC = 1 << 4,

        UTRSTAT_Rx_RDY = 1 << 0,
        UTRSTAT_Tx_RDY = 1 << 1,

        UINT_RXD   = 1 << 0,
        UINT_ERROR = 1 << 1,
        UINT_TXD   = 1 << 2,
        UINT_MODEM = 1 << 3,

        UFSTAT_Rx_COUNT_MASK = 0x000000ff,
        UFSTAT_Tx_COUNT_MASK = 0x00ff0000,
        UFSTAT_RxFULL        = (1 << 8),
        UFSTAT_TxFULL        = (1 << 24),
    };

    void Uart_s5p6818::fifo_reset() {
        _regs->write<unsigned int>(UFCON, UFCON_RX_FIFO_RESET | UFCON_TX_FIFO_RESET);
        Poll_timeout_counter i(5000000);
        while (i.test(_regs->read<unsigned int>(UFCON) & (UFCON_RX_FIFO_RESET | UFCON_TX_FIFO_RESET)))
            ;
    }

    bool Uart_s5p6818::startup(Io_register_block const *regs) {
        _regs = regs;

        fifo_reset();

        /* enable FIFOs, auto clear RX FIFO */
        _regs->write<unsigned int>(UFCON, UFCON_ENABLE);
        _regs->write<unsigned int>(UMCON, UFCON_RX_FIFO_RESET);
        _regs->write<unsigned int>(ULCON,  ULCON_8N1_MODE);

        _regs->write<unsigned int>(UCON, 0x245); // no interrupts, no DMA, pure polling
        _regs->write<unsigned int>(UINTM, ~UINT_RXD); // mask all but receive irq
        _regs->write<unsigned int>(UINTP, ~0); // clear all pending irqs

        return true;
    }

    void Uart_s5p6818::shutdown() {}

    bool Uart_s5p6818::change_mode(Transfer_mode, Baud_rate r) {
        if (r != 115200)
            return false;

        return true;
    }

    int Uart_s5p6818::get_char(bool blocking) const {
        while (!char_avail())
            if (!blocking)
                return -1;

        int c = _regs->read<unsigned int>(URXH) & 0xff;
        return c;
    }

    int Uart_s5p6818::char_avail() const {
        return is_rx_fifo_non_empty();
    }

    void Uart_s5p6818::out_char(char c) const {
        wait_for_non_full_tx_fifo();
        _regs->write<unsigned int>(UTXH, c);
    }

    int Uart_s5p6818::write(char const *s, unsigned long count) const {
        unsigned long c = count;
        while (c--)
            out_char(*s++);
        wait_for_empty_tx_fifo();

        return count;
    }

    void Uart_s5p6818::wait_for_empty_tx_fifo() const {
        Poll_timeout_counter i(5000000);
        while (i.test(_regs->read<unsigned int>(UFSTAT) & (UFSTAT_Tx_COUNT_MASK | UFSTAT_TxFULL)))
            ;
    }

    void Uart_s5p6818::wait_for_non_full_tx_fifo() const {
        Poll_timeout_counter i(5000000);
        while (i.test(_regs->read<unsigned int>(UFSTAT) & UFSTAT_TxFULL))
            ;
    }

    unsigned Uart_s5p6818::is_rx_fifo_non_empty() const {
        return _regs->read<unsigned int>(UFSTAT) & (UFSTAT_Rx_COUNT_MASK | UFSTAT_RxFULL);
    }

    void Uart_s5p6818::ack_rx_irq() const {
        _regs->write<unsigned int>(UINTP, UINT_RXD);
    }
};
