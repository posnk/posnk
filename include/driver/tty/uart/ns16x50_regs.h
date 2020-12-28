#ifndef POSNK_NS16X50_REGS_H
#define POSNK_NS16X50_REGS_H

#ifdef UART_16550
#define NS16X50_HAVE_FIFO
#endif
#ifdef UART_16750
#define NS16X50_HAVE_FIFO
#define NS16X50_HAVE_LOWPOWER
#define NS16X50_HAVE_64BYTE
#endif

/**
 * Receive buffer register (Read-only)
 */
#define NS16X50_REG_RHR    (0x00)

/**
 * Transmit buffer register (Write-only)
 */
#define NS16X50_REG_THR    (0x00)

/**
 * Interrupt Enable register (Read/Write)
 */
#define NS16X50_REG_IER    (0x01)

#ifdef NS16X50_HAVE_FIFO
/**
 * FIFO Control Register (Write-only)
 */
#define NS16X50_REG_FCR    (0x02)
#endif

/**
 * Interrupt Status Register (Read-only)
 */
#define NS16X50_REG_ISR    (0x02)

/**
 * Line Control Register (Read/Write)
 */
#define NS16X50_REG_LCR    (0x03)

/**
 * Modem Control Register (Read/Write)
 */
#define NS16X50_REG_MCR    (0x04)

/**
 * Line Status Register (Read/Write)
 */
#define NS16X50_REG_LSR    (0x05)

/**
 * Modem Status Register (Read/Write)
 */
#define NS16X50_REG_MSR    (0x06)

/**
 * Scratchpad Register (Read/Write)
 */
#define NS16X50_REG_SPR    (0x07)

/**
 * Divisor Latch Low Register (Read/Write)
 */
#define NS16X50_REG_DLL    (0x10)

/**
NS16X50_FCR_RX_TRIG(
 * Divisor Latch High Register (Read/Write)
 */
#define NS16X50_REG_DLH    (0x11)

/********************************* IER bits ***********************************/

/**
 * Enable receive buffer full interrupt
 */
#define NS16X50_IER_ERBF     (1u << 0u)

/**
 * Enable transmit buffer empty interrupt
 */
#define NS16X50_IER_ETBEI    (1u << 1u)

/**
 * Enable receive line status interrupt
 */
#define NS16X50_IER_ELSI     (1u << 2u)

/**
 * Enable MODEM status interrupt
 */
#define NS16X50_IER_EDSSI    (1u << 3u)

#ifdef NS16X50_HAVE_LOWPOWER
/**
 * Enable sleep mode
 */
#define NS16X50_IER_SLEEP    (1u << 4u)

/**
 * Enable low-power mode
 */
#define NS16X50_IER_LOWPWR   (1u << 5u)
#endif

/********************************* ISR bits ***********************************/

/**
 * If this bit is set, there are no interrupts pending
 */
#define NS16X50_ISR_NO_INT   (1u << 0u)

/**
 * Macro extracting the ISR ID
 */
#define NS16X50_ISR_ID(v)    (((v) >> 1u)&7u)

/**
 * Line status interrupt
 */
#define NS16X50_ISR_ID_LSR     (3u)

/**
 * Receive data ready
 */
#define NS16X50_ISR_ID_RXRDY   (2u)

/**
 * Receive timeout
 */
#define NS16X50_ISR_ID_RXTO    (6u)

/**
 * Transmit buffer empty
 */
#define NS16X50_ISR_ID_TXRDY   (1u)

/**
 * Modem status interrupt
 */
#define NS16X50_ISR_ID_MSR     (0u)

#ifdef NS16X50_HAVE_64BYTE
/**
 * Set if the FIFO is in 64 byte mode, if clear, the FIFO is in 16 byte mode.
 */
#define NS16X50_ISR_64BYTE   (1u << 5u)
#endif

#ifdef NS16X50_HAVE_FIFO
/**
 * Set if the FIFO is enabled
 */
#define NS16X50_ISR_FIFO_EN  (3u << 6u)
#endif

#ifdef NS16X50_HAVE_FIFO
/********************************* FCR bits ***********************************/

/**
 * Set to enable FIFO
 */
#define NS16X50_FCR_FIFO_EN  (1u << 0u)

/**
 * Set to reset the RX FIFO
 */
#define NS16X50_FCR_RX_RST   (1u << 1u)

/**
 * Set to reset the TX FIFO
 */
#define NS16X50_FCR_TX_RST   (1u << 2u)

/**
 * Set to enable DMA mode.
 */
#define NS16X50_FCR_DMA_MODE (1u << 3u)

#ifdef NS16X50_HAVE_64BYTE
/**
 * Set the FIFO to be 64 bytes long instead of 16 bytes
 */
#define NS16X50_FCR_64BYTE   (1u << 5u)
#endif

/**
 * Set the trigger level for the RX FIFO
 */
#define NS16X50_FCR_RX_TRIG(v) (((v)&3u)<<6u)

/**
 * Set trigger level to 1 byte, when FIFO length is 16 bytes.
 */
#define NS16X50_FCR_RX_TRIG16_1   (NS16X50_FCR_RX_TRIG(0))

/**
 * Set trigger level to 4 bytes, when FIFO length is 16 bytes.
 */
#define NS16X50_FCR_RX_TRIG16_4   (NS16X50_FCR_RX_TRIG(1))

/**
 * Set trigger level to 8 bytes, when FIFO length is 16 bytes.
 */
#define NS16X50_FCR_RX_TRIG16_8   (NS16X50_FCR_RX_TRIG(2))

/**
 * Set trigger level to 14 bytes, when FIFO length is 16 bytes.
 */
#define NS16X50_FCR_RX_TRIG16_14  (NS16X50_FCR_RX_TRIG(3))

#ifdef NS16X50_HAVE_64BYTE
/**
 * Set trigger level to 1 byte, when FIFO length is 64 bytes.
 */
#define NS16X50_FCR_RX_TRIG64_1   (NS16X50_FCR_RX_TRIG(0))

/**
 * Set trigger level to 16 bytes, when FIFO length is 64 bytes.
 */
#define NS16X50_FCR_RX_TRIG64_16  (NS16X50_FCR_RX_TRIG(1))

/**
 * Set trigger level to 32 bytes, when FIFO length is 64 bytes.
 */
#define NS16X50_FCR_RX_TRIG64_32  (NS16X50_FCR_RX_TRIG(2))

/**
 * Set trigger level to 56 bytes, when FIFO length is 64 bytes.
 */
#define NS16X50_FCR_RX_TRIG64_56  (NS16X50_FCR_RX_TRIG(3))
#endif

#endif
/********************************* LCR bits ***********************************/

/**
 * Set long stop bit
 */
#define NS16X50_LCR_STB       (1u << 2u)

/**
 * Enable parity
 */
#define NS16X50_LCR_PEN       (1u << 3u)

/**
 * Set parity to be even or space
 */
#define NS16X50_LCR_EPS       (1u << 4u)

/**
 * Set parity bit to be sticky
 */
#define NS16X50_LCR_STICK     (1u << 5u)

/**
 * So long as this bit is set the transmitter will force the line to 0.
 */
#define NS16X50_LCR_BREAK     (1u << 6u)

/**
 * Select divisor latch registers
 */
#define NS16X50_LCR_DLAB      (1u << 7u)

/**
 * Word length is 5
 */
#define NS16X50_LCR_WL5       (0)

/**
 * Word length is 6
 */
#define NS16X50_LCR_WL6       (1)

/**
 * Word length is 7
 */
#define NS16X50_LCR_WL7       (2)

/**
 * Word length is 8
 */
#define NS16X50_LCR_WL8       (3)

/**
 * Do not use parity
 */
#define NS16X50_LCR_PAR_NONE  (0)

/**
 * Use odd parity
 */
#define NS16X50_LCR_PAR_ODD   (NS16X50_LCR_PEN)

/**
 * Use even parity
 */
#define NS16X50_LCR_PAR_EVEN  (NS16X50_LCR_PEN | NS16X50_LCR_EPS)

/**
 * Use mark parity (parity forced to 1)
 */
#define NS16X50_LCR_PAR_MARK  (NS16X50_LCR_PEN | NS16X50_LCR_STICK)

/**
 * Use space parity (parity forced to 0)
 */
#define NS16X50_LCR_PAR_SPACE (NS16X50_LCR_PEN | NS16X50_LCR_EPS \
                             | NS16X50_LCR_STICK)

/********************************* MCR bits ***********************************/

#define NS16X50_MCR_nDTR      (1u << 0u)
#define NS16X50_MCR_nRTR      (1u << 1u)
#define NS16X50_MCR_nOUT1     (1u << 2u)
#define NS16X50_MCR_nOUT2     (1u << 3u)
#define NS16X50_MCR_LOOPBACK  (1u << 4u)
#define NS16X50_MCR_AFE       (1u << 5u)

/********************************* LSR bits ***********************************/

/**
 * Set when data is available.
 */
#define NS16X50_LSR_DR        (1u << 0u)

/**
 * Set when a receive overrun has occurred.
 */
#define NS16X50_LSR_OE        (1u << 1u)

/**
 * Set when a receive parity error has occured.
 */
#define NS16X50_LSR_PE        (1u << 2u)

/**
 * Set when a receive framing error has occurred.
 */
#define NS16X50_LSR_FE        (1u << 3u)

/**
 * Set when a break signal was received.
 */
#define NS16X50_LSR_BI        (1u << 4u)

#define NS16X50_LSR_ERROR     (0x1Fu)

/**
 * Set when transmit buffer is empty.
 */
#define NS16X50_LSR_THRE      (1u << 5u)

/**
 * Set when transmit FIFO and buffer are empty.
 */
#define NS16X50_LSR_TEMT      (1u << 6u)

#ifdef NS16X50_HAVE_FIFO
/**
 * Set when there are error bytes in the receive FIFO
 */
#define NS16X50_LSR_FERR      (1u << 7u)
#endif

/********************************* MSR bits ***********************************/

/**
 * Set if the nCTS line has changed since last MSR read
 */
#define NS16X50_MSR_DCTS      (1u << 0u)

/**
 * Set if the nDSR line has changed since last MSR read
 */
#define NS16X50_MSR_DDSR      (1u << 1u)

/**
 * Set if the nRI line has changed since last MSR read
 */
#define NS16X50_MSR_DRI       (1u << 2u)

/**
 * Set if the nDCD line has changed since last MSR read
 */
#define NS16X50_MSR_DDCD      (1u << 3u)

/**
 * Set if the nCTS line was logic 0
 */
#define NS16X50_MSR_CTS       (1u << 4u)

/**
 * Set if the nDSR line was logic 0
 */
#define NS16X50_MSR_DSR       (1u << 5u)

/**
 * Set if the nRI line was logic 0
 */
#define NS16X50_MSR_RI        (1u << 6u)

/**
 * Set if the nDCD line was logic 0
 */
#define NS16X50_MSR_DCD       (1u << 7u)

#endif //POSNK_NS16X50_REGS_H
