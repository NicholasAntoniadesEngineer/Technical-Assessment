/**
 * ATSAMV71.h
 * Author: Nicholas
 */

#ifndef ATSAMV71_H
#define ATSAMV71_H

#include <iostream>

#define PMC_BASE_ADDR 0x400E0600U    // Base address for PMC (Power Management Controller)
#define PIOA_BASE_ADDR 0x400E0E00U   // Base address for PIOA (Parallel I/O Controller A)
#define PIOB_BASE_ADDR 0x400E1000U   // Base address for PIOB (Parallel I/O Controller B)
#define PIOC_BASE_ADDR 0x400E1200U   // Base address for PIOC (Parallel I/O Controller C)
#define PIOD_BASE_ADDR 0x400E1400U   // Base address for PIOD (Parallel I/O Controller D)

#define PMC_PCER0_OFFSET 0x0010U     // Offset for PMC_PCER0 (Peripheral Clock Enable Register 0)
#define PMC_PCER1_OFFSET 0x0014U     // Offset for PMC_PCER1 (Peripheral Clock Enable Register 1)

#define PIO_OER_OFFSET 0x0010U       // Offset for PIO_OER (Output Enable Register)
#define PIO_ODR_OFFSET 0x0014U       // Offset for PIO_ODR (Output Disable Register)
#define PIO_SODR_OFFSET 0x0030U      // Offset for PIO_SODR (Set Output Data Register)
#define PIO_CODR_OFFSET 0x0034U      // Offset for PIO_CODR (Clear Output Data Register)
#define PIO_PDSR_OFFSET 0x003CU      // Offset for PIO_PDSR (Pin Data Status Register)

typedef enum {
    PIOA,
    PIOB,
    PIOC,
    PIOD
} Port;

typedef enum {
    INPUT,
    OUTPUT
} Mode;

typedef enum {
    PERIPHERAL_A,
    PERIPHERAL_B,
    // Add more peripheral functions as needed
} Function;

class GPIO {
public:
    bool configure(Port port, int pin, Function function, Mode mode);
    bool set(Port port, int pin, bool pin_state);
    bool read(Port port, int pin);
};

#endif