/**
 * @file ATSAMV71.h
 * @brief Header file for GPIO control on ATSAMV71 microcontroller.
 *
 * Defines the types, constants, and function prototypes for configuring, setting, and reading 
 * GPIO pins on the ATSAMV71. Includes memory addresses and offsets for relevant registers.
 * 
 * @date 29 April 2024
 * @author Nicholas Antoniades
 */

#ifndef ATSAMV71_H
#define ATSAMV71_H

#include <iostream>

// Port enumeration
typedef enum {
    PIOA,
    PIOB,
    PIOC,
    PIOD
} Port;

// Function enumeration
typedef enum {
    PERIPHERAL_A,
    PERIPHERAL_B,
    UART0,
    AF,
    Input,
    Output
} Function;

// Mode enumeration
typedef enum {
    INPUT,
    OUTPUT,
    Tx,
    Rx,
    AF1,
    PullUp,
    NoPull
} Mode;

// Base addresses for PMC and PIO
#define PMC_BASE_ADDR 0x400E0600U    // Base address for PMC (Power Management Controller)
#define PIOA_BASE_ADDR 0x400E0E00U   // Base address for PIOA (Parallel I/O Controller A)
#define PIOB_BASE_ADDR 0x400E1000U   // Base address for PIOB (Parallel I/O Controller B)
#define PIOC_BASE_ADDR 0x400E1200U   // Base address for PIOC (Parallel I/O Controller C)
#define PIOD_BASE_ADDR 0x400E1400U   // Base address for PIOD (Parallel I/O Controller D)

// Offsets for PMC and PIO registers
#define PMC_PCER0_OFFSET 0x0010U     // Offset for PMC_PCER0 (Peripheral Clock Enable Register 0)
#define PMC_PCER1_OFFSET 0x0100U     // Offset for PMC_PCER1 (Peripheral Clock Enable Register 1)

#define PIO_OER_OFFSET 0x0010U       // Offset for PIO_OER (Output Enable Register)
#define PIO_ODR_OFFSET 0x0014U       // Offset for PIO_ODR (Output Disable Register)
#define PIO_SODR_OFFSET 0x0030U      // Offset for PIO_SODR (Set Output Data Register)
#define PIO_CODR_OFFSET 0x0034U      // Offset for PIO_CODR (Clear Output Data Register)
#define PIO_PDSR_OFFSET 0x003CU      // Offset for PIO_PDSR (Pin Data Status Register)

// Macro to access a register
#define REG32(addr) (*(volatile uint32_t *)(addr))

class GPIO {
public:
    // Public functions for configuring, setting, and reading GPIO pins
    static bool configure(Port port, int pin, Function function, Mode mode);
    static bool set(Port port, int pin, bool pin_state);
    static bool read(Port port, int pin);

private:
    // Private functions for mock implementation
    static bool _mock_GPIO_configure(Port port, int pin, Function function, Mode mode);
    static bool _mock_GPIO_set(Port port, int pin, bool pin_state);
    static bool _mock_GPIO_read(Port port, int pin);

    // Private functions for real hardware implementation
    static bool _real_GPIO_configure(Port port, int pin, Function function, Mode mode);
    static bool _real_GPIO_set(Port port, int pin, bool pin_state);
    static bool _real_GPIO_read(Port port, int pin);

    // Helper functions to get base addresses and offsets
    static uintptr_t _get_pio_base_addr(Port port);
    static uintptr_t _get_pmc_pcer_offset(Port port);
};

#endif // ATSAMV71_H
