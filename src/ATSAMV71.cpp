/**
 * @file ATSAMV71.cpp
 * @brief GPIO configuration and control implementation for ATSAMV71.
 *
 * This file provides the implementation for configuring, setting, and reading GPIO pins 
 * on the ATSAMV71 microcontroller. It includes both a mock implementation for testing 
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
 * @brief Mock implementation of GPIO pin configuration.
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
    std::cout << "Mock configuring GPIO \n";
    std::cout << " - Port     : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin      : " << pin << "\n";   
    std::cout << " - Function : " << static_cast<int>(function) << "\n";
    std::cout << " - Mode     : " << static_cast<int>(mode) << "\n";
    std::cout << " \n";
    return true;
}

/**
 * @brief Mock implementation of setting a GPIO pin state.
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
    std::cout << "Mock setting GPIO \n";  
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";  
    std::cout << " - State  : " << pin_state << "\n";  
    std::cout << " \n";
    return true;
}

/**
 * @brief Mock implementation of reading a GPIO pin state.
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
    std::cout << "Mock read GPIO \n";
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";     
    std::cout << " \n";
    return true;
}

//==============================================================================
// Private functions for real hardware implementation (used in deployment)
//==============================================================================

/**
 * @brief Helper function to get the base address of the PIO controller for a given port.
 * 
 * @param port Port number of GPIO
 * @return uintptr_t Base address of the PIO controller
 */
uintptr_t GPIO::_get_pio_base_addr(Port port) 
{
    switch(port) 
    {
        case PIOA: return PIOA_BASE_ADDR;
        case PIOB: return PIOB_BASE_ADDR;
        case PIOC: return PIOC_BASE_ADDR;
        case PIOD: return PIOD_BASE_ADDR;
        default: return 0; // Invalid port
    }
}

/**
 * @brief Helper function to get the offset of the PMC_PCER register for a given port.
 * 
 * @param port Port number of GPIO
 * @return uintptr_t Offset of the PMC_PCER register
 */
uintptr_t GPIO::_get_pmc_pcer_offset(Port port) 
{
    switch(port) 
    {
        case PIOA:
        case PIOB: return PMC_PCER0_OFFSET;
        case PIOC:
        case PIOD: return PMC_PCER1_OFFSET;
        default: return 0; // Invalid port
    }
}

/**
 * @brief Configures a specific GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param function Function of GPIO 
 * @param mode Mode of GPIO 
 * @return bool true if successful, false otherwise
 */
bool GPIO::_real_GPIO_configure(Port port, int pin, Function function, Mode mode) 
{
    uintptr_t pio_base_addr = _get_pio_base_addr(port);
    uintptr_t pmc_pcer_offset = _get_pmc_pcer_offset(port);

    if (pio_base_addr == 0 || pmc_pcer_offset == 0) return false; // Invalid port

    // Determine the bit position for enabling the peripheral clock
    int bit_position = (port == PIOA ? 11 : port == PIOB ? 12 : port == PIOC ? 13 : 0);

    // If the bit position is 0, return false
    if (bit_position == 0) return false;

    // Enable peripheral clock for the selected peripheral
    REG32(PMC_BASE_ADDR + pmc_pcer_offset) |= (1 << bit_position);

    // Configure the peripheral mode
    if (mode == OUTPUT) {
        REG32(pio_base_addr + PIO_OER_OFFSET) |= (1 << pin);  // Set pin as output
    } else {
        REG32(pio_base_addr + PIO_ODR_OFFSET) |= (1 << pin);  // Set pin as input
    }
    
    return true;  
}


/**
 * @brief Sets a specific GPIO pin to a given state.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param pin_state State to set the pin (true for high, false for low)
 * @return bool true if successful, false otherwise
 */
bool GPIO::_real_GPIO_set(Port port, int pin, bool pin_state) 
{
    uintptr_t pio_base_addr = _get_pio_base_addr(port);

    if (pio_base_addr == 0) return false; // Invalid port

    // Set pin state
    if (pin_state) {
        REG32(pio_base_addr + PIO_SODR_OFFSET) |= (1 << pin);  // Set pin high
    } else {
        REG32(pio_base_addr + PIO_CODR_OFFSET) |= (1 << pin);  // Set pin low
    }
    
    return true;   
}

/**
 * @brief Reads the current state of a specific GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @return bool true if pin is high, false if pin is low
 */
bool GPIO::_real_GPIO_read(Port port, int pin) 
{
    uintptr_t pio_base_addr = _get_pio_base_addr(port);

    if (pio_base_addr == 0) return false; // Invalid port

    // Read pin state
    bool pin_state = (REG32(pio_base_addr + PIO_PDSR_OFFSET) & (1 << pin)) != 0;

    return pin_state;  
}
