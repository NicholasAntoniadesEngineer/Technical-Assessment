/**
 * ATSAMV71.cpp
 * Author: Nicholas
 */

#include <iostream>
#include "ATSAMV71.h"

/**
 * Configures a specific GPIO pin.
 * 
 * @param port Port number of GPIO
 * @param pin Pin number of GPIO
 * @param function function of GPIO 
 * @param mode mode of GPIO 
 * @return bool true if successful false otherwise
 */
bool GPIO::configure(Port port, int pin, Function function, Mode mode) 
{
    // This is a placeholder actual hardware interfacing logic here.
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
 * @return bool true if successful false otherwise
 */
bool GPIO::set(Port port, int pin, bool pin_state) 
{
    // Placeholder for actual pin setting logic
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
 * @return bool true if successful false otherwise
 */
bool GPIO::read(Port port, int pin) {

    bool pin_state = true;

    // Placeholder for actual pin reading logic
    std::cout << "Read GPIO \n";
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";   
    std::cout << " - State  : " << pin_state << "\n";  
    std::cout << " \n";

    return pin_state;  // Indicate a fictitious "high" signal
}