/**
 * ATSAMV71.cpp
 * Author: Nicholas
 */

#include "ATSAMV71.h"
#include <iostream>

#ifdef MOCK_TEST
// Mock implementation

bool GPIO::configure(Port port, int pin, Function function, Mode mode) 
{
    std::cout << "Mock configuring GPIO \n";
    std::cout << " - Port     : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin      : " << pin << "\n";   
    std::cout << " - Function : " << static_cast<int>(function) << "\n";
    std::cout << " - Mode     : " << static_cast<int>(mode) << "\n";
    std::cout << " \n";
    return true;
}

bool GPIO::set(Port port, int pin, bool pin_state) 
{
    std::cout << "Mock setting GPIO \n";  
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";  
    std::cout << " - State  : " << pin_state << "\n";  
    std::cout << " \n";
    return true;
}

bool GPIO::read(Port port, int pin) 
{
    std::cout << "Mock read GPIO \n";
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";     
    std::cout << " \n";
    return true;
}

#else

/**
 * Configures a specific GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param function Function of GPIO 
 * @param mode Mode of GPIO 
 * @return bool true if successful, false otherwise
 */

bool GPIO::configure(Port port, int pin, Function function, Mode mode) 
{
    uintptr_t pio_base_addr;
    uintptr_t pmc_pcer_offset;

    // Select which port to use
    switch(port) {
        case PIOA:
            pio_base_addr = PIOA_BASE_ADDR;
            pmc_pcer_offset = PMC_PCER0_OFFSET;
            break;
        case PIOB:
            pio_base_addr = PIOB_BASE_ADDR;
            pmc_pcer_offset = PMC_PCER0_OFFSET;
            break;
        case PIOC:
            pio_base_addr = PIOC_BASE_ADDR;
            pmc_pcer_offset = PMC_PCER1_OFFSET;
            break;
        case PIOD:
            pio_base_addr = PIOD_BASE_ADDR;
            pmc_pcer_offset = PMC_PCER1_OFFSET;
            break;
        default:
            return false; // Invalid port
    }

    // Enable peripheral clock for the selected port
    *((volatile uint32_t *)(PMC_BASE_ADDR + pmc_pcer_offset)) |= (1 << (port == PIOA ? 11 : 
                                                                       port == PIOB ? 12 : 
                                                                       port == PIOC ? 13 : 
                                                                                      14));  

    // Configure the pin mode
    if (mode == OUTPUT) {
        *((volatile uint32_t *)(pio_base_addr + PIO_OER_OFFSET)) |= (1 << pin);  // Set pin as output
    } else {
        *((volatile uint32_t *)(pio_base_addr + PIO_ODR_OFFSET)) |= (1 << pin);  // Set pin as input
    }

    std::cout << "Configuring GPIO \n";
    std::cout << " - Port     : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin      : " << pin << "\n";   
    std::cout << " - Function : " << static_cast<int>(function) << "\n";
    std::cout << " - Mode     : " << static_cast<int>(mode) << "\n";
    std::cout << " \n";
    
    return true;  
}

/**
 * Sets a specific GPIO pin to a given state.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param pin_state State to set the pin (true for high, false for low)
 * @return bool true if successful, false otherwise
 */
bool GPIO::set(Port port, int pin, bool pin_state) 
{
    uintptr_t pio_base_addr;

    // Select which port to use
    switch(port) {
        case PIOA:
            pio_base_addr = PIOA_BASE_ADDR;
            break;
        case PIOB:
            pio_base_addr = PIOB_BASE_ADDR;
            break;
        case PIOC:
            pio_base_addr = PIOC_BASE_ADDR;
            break;
        case PIOD:
            pio_base_addr = PIOD_BASE_ADDR;
            break;
        default:
            return false; // Invalid port
    }

    // Set pin state
    if (pin_state) {
        *((volatile uint32_t *)(pio_base_addr + PIO_SODR_OFFSET)) |= (1 << pin);  // Set pin high
    } else {
        *((volatile uint32_t *)(pio_base_addr + PIO_CODR_OFFSET)) |= (1 << pin);  // Set pin low
    }

    std::cout << "Setting GPIO \n";  
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";  
    std::cout << " - State  : " << pin_state << "\n";  
    std::cout << " \n";
    
    return true;   
}

/**
 * Reads the current state of a specific GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @return bool true if pin is high, false if pin is low
 */
bool GPIO::read(Port port, int pin) 
{
    uintptr_t pio_base_addr;

    // Select which port to use
    switch(port) {
        case PIOA:
            pio_base_addr = PIOA_BASE_ADDR;
            break;
        case PIOB:
            pio_base_addr = PIOB_BASE_ADDR;
            break;
        case PIOC:
            pio_base_addr = PIOC_BASE_ADDR;
            break;
        case PIOD:
            pio_base_addr = PIOD_BASE_ADDR;
            break;
        default:
            return false; // Invalid port
    }

    // Read pin state
    bool pin_state = (*((volatile uint32_t *)(pio_base_addr + PIO_PDSR_OFFSET)) & (1 << pin)) != 0;

    
    std::cout << "Read GPIO \n";
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";     
    std::cout << " \n";

    return pin_state;  
}


#endif


