/**
 * @file ATSAMV71.cpp
 * @brief GPIO configuration and control implementation for ATSAMV71.
 *
 * This file provides the implementation for configuring, setting, and reading GPIO pins 
 * on the ATSAMV71 microcontroller. It includes both a _mock implementation for testing 
 * and a hardware-specific implementation for actual deployment.
 * 
 * @author Nicholas Antoniades
 * @date 29 April 2024
 */

#include "ATSAMV71.h"
#include <iostream>


//==============================================================================
// Public functions for GPIO operations (used by external code)
//==============================================================================

/**
 * @brief Configures a GPIO pin.
 * 
 * This function selects either the mock or real implementation based on the 
 * compile-time flag MOCK_TEST. It configures the mode and function of a GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param function Function of GPIO 
 * @param mode Mode of GPIO 
 * @return bool true if successful, false otherwise
 */
bool GPIO::configure(Port port, int pin, Function function, Mode mode) 
{
#ifdef MOCK_TEST
    return _mock_GPIO_configure(port, pin, function, mode);
#else
    return _real_GPIO_configure(port, pin, function, mode);
#endif
}

/**
 * @brief Sets a GPIO pin state.
 * 
 * This function selects either the mock or real implementation based on the 
 * compile-time flag MOCK_TEST. It sets the state (high or low) of a GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param pin_state State to set the pin (true for high, false for low)
 * @return bool true if successful, false otherwise
 */
bool GPIO::set(Port port, int pin, bool pin_state) 
{
#ifdef MOCK_TEST
    return _mock_GPIO_set(port, pin, pin_state);
#else
    return _real_GPIO_set(port, pin, pin_state);
#endif
}

/**
 * @brief Reads the state of a GPIO pin.
 * 
 * This function selects either the mock or real implementation based on the 
 * compile-time flag MOCK_TEST. It reads the current state (high or low) of a GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @return bool true if pin is high, false if pin is low
 */
bool GPIO::read(Port port, int pin) 
{
#ifdef MOCK_TEST
    return _mock_GPIO_read(port, pin);
#else
    return _real_GPIO_read(port, pin);
#endif
}


//==============================================================================
// Private functions for mock implementation (used for testing purposes)
//==============================================================================

/**
 * @brief mock implementation of GPIO pin configuration.
 * 
 * This function simulates configuring a GPIO pin for testing purposes.
 * It prints the configuration details to the console.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param function Function of GPIO 
 * @param mode Mode of GPIO 
 * @return bool true
 */
bool GPIO::_mock_GPIO_configure(Port port, int pin, Function function, Mode mode) 
{
    std::cout << "_mock configuring GPIO \n";
    std::cout << " - Port     : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin      : " << pin << "\n";   
    std::cout << " - Function : " << static_cast<int>(function) << "\n";
    std::cout << " - Mode     : " << static_cast<int>(mode) << "\n";
    std::cout << " \n";
    return true;
}

/**
 * @brief mock implementation of setting a GPIO pin state.
 * 
 * This function simulates setting a GPIO pin state for testing purposes.
 * It prints the pin state details to the console.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param pin_state State to set the pin (true for high, false for low)
 * @return bool true
 */
bool GPIO::_mock_GPIO_set(Port port, int pin, bool pin_state) 
{
    std::cout << "_mock setting GPIO \n";  
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";  
    std::cout << " - State  : " << pin_state << "\n";  
    std::cout << " \n";
    return true;
}

/**
 * @brief mock implementation of reading a GPIO pin state.
 * 
 * This function simulates reading a GPIO pin state for testing purposes.
 * It prints the pin state details to the console.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @return bool true
 */
bool GPIO::_mock_GPIO_read(Port port, int pin) 
{
    std::cout << "_mock read GPIO \n";
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";     
    std::cout << " \n";
    return true;
}

//==============================================================================
// Private functions for real hardware implementation (used in deployment)
//==============================================================================

/**
 * Configures a specific GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param function Function of GPIO 
 * @param mode Mode of GPIO 
 * @return bool true if successful, false otherwise
 */
bool GPIO::_real_GPIO_configure(Port port, int pin, Function function, Mode mode) 
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
                                                                       port == PIOC ? 13 : 14));  

    // Configure the pin mode
    if (mode == OUTPUT) {
        *((volatile uint32_t *)(pio_base_addr + PIO_OER_OFFSET)) |= (1 << pin);  // Set pin as output
    } else {
        *((volatile uint32_t *)(pio_base_addr + PIO_ODR_OFFSET)) |= (1 << pin);  // Set pin as input
    }
    
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
bool GPIO::_real_GPIO_set(Port port, int pin, bool pin_state) 
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
    
    return true;   
}

/**
 * Reads the current state of a specific GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @return bool true if pin is high, false if pin is low
 */
bool GPIO::_real_GPIO_read(Port port, int pin) 
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

    return pin_state;  
}
