/**
 * ATSAMV71.h
 * Author: Nicholas
 */

#ifndef ATSAMV71_H
#define ATSAMV71_H

#include <iostream>

enum class Port { PA, PB, PC };
enum class Function { Input, Output, AF, UART0 };
enum class Mode { NoPull, PullUp, PullDown, Rx, Tx, AF1 };

class GPIO {
public:
    static bool configure(Port port, int pin, Function function, Mode mode);
    static bool set(Port port, int pin, bool pin_state);
    static bool read(Port port, int pin);
};

#endif