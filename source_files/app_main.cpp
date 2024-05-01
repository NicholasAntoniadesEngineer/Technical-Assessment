/**
 * app_main.cpp
 * Author: Nicholas
 */

#include <iostream>
#include "ATSAMV71.h"

/**
 * Main configuration function that sets up all GPIOs.
 * 
 * @return bool true if all configurations are successful, false otherwise
 */
bool ATSAMV71_configure() 
{
    std::cout << " \n";
    std::cout << "------------------------------\n";
    std::cout << "Running ATSAMV71 configuration\n";
    std::cout << "------------------------------\n";

    // Configure PA1 as UART0 Tx
    if (!GPIO::configure(Port::PA, 1, Function::UART0, Mode::Tx)) return false;

    // Configure PA9 as UART0 Rx
    if (!GPIO::configure(Port::PA, 9, Function::UART0, Mode::Rx)) return false;

    // Configure PB1 as AFEC 1 Channel 0
    if (!GPIO::configure(Port::PB, 1, Function::AF, Mode::AF1)) return false;

    // Configure PB12 as input with pull-up
    if (!GPIO::configure(Port::PB, 12, Function::Input, Mode::PullUp)) return false;

    // Configure PC9 as output
    if (!GPIO::configure(Port::PC, 9, Function::Output, Mode::NoPull)) return false;

    // Configure PC10 as output
    if (!GPIO::configure(Port::PC, 10, Function::Output, Mode::NoPull)) return false;

    std::cout << "------------------------------\n";
    std::cout << "Configuration Successful\n";
    std::cout << "------------------------------\n";

    return true;
}

bool ATSAMV71_run() 
{
    std::cout << " \n";
    std::cout << "------------------------------\n";
    std::cout << "Running ATSAMV71 app\n";
    std::cout << "------------------------------\n";

    // Read PB12 state
    GPIO::read(Port::PB, 12);

    // Set PC9 to high
    GPIO::set(Port::PC, 9, true);

    // Set PC10 to low
    GPIO::set(Port::PC, 10, false);

    return true; 
}

/**
 * Entry point of the program.
 * 
 * @return int The exit status of the program, 0 on successful configuration
 */
int main() 
{
    if (ATSAMV71_configure()) 
    {
        ATSAMV71_run();
    }
    else
    {
        std::cout << "------------------------------\n";
        std::cout << "Configuration Failed\n";
        std::cout << "------------------------------\n";
        return 1;
    }

    return 0;
}
