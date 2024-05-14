/**
 * ATSAMV71.cpp
 * Author: Nicholas
 */

#include "../include/samv71j19b.h"
#include "../include/pio/samv71j19b.h"
#include "ATSAMV71.h"
#include <iostream>

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
    // Select which port to use
    
    // Enable peripheral clock for the selected port
    
    // Configure the pin mode

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
    // Select which port to use
    
    // Set pin state

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
    // Select which port to use
    
    // Read pin state
    
    std::cout << "Read GPIO \n";
    std::cout << " - Port   : " << static_cast<int>(port) << "\n";
    std::cout << " - Pin    : " << pin << "\n";     
    std::cout << " \n";

    return true;  // Return actual pin state
}




