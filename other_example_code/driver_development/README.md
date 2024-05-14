
# Overview

This driver provides the functionality to interface with VectorNav device via UART communication.

#Dependencies

- console_commands.h: Header file for console commands.
- driver_uart.h: Header file for UART driver.
- driver_vectornav.h: Header file for VectorNav driver.


# Usage

1. Initialize the VectorNav driver using driver_vectornav_init.
2. Configure the VectorNav driver using driver_vectornav_configure.
3. Implement event callback function driver_vectornav_eventcallback to handle incoming messages.
4. Send commands or read data using appropriate driver functions.
5. For specific settings or commands, refer to the API reference.


## Initialization and Configuration:

- driver_vectornav_init: Initialize the VectorNav driver.
- driver_vectornav_configure: Configure the VectorNav driver.

Sending Commands and Reading Data:

- driver_vectornav_send_byte: Send a command to the VectorNav device.
- driver_vectornav_read_byte: Read a byte from the VectorNav device.

## Configuration Settings:

- driver_vectornav_set_output_data_freq: Set the output data frequency.
- driver_vectornav_set_vectoranv_baud_rate: Set the VectorNav baud rate.
- driver_vectornav_set_uart_baud_rate: Set the UART baud rate.

## Register Access:

- driver_vectornav_read_register: Read the value of a register.
- driver_vectornav_write_register: Write data values to a register.
- driver_vectornav_write_settings: Write current register settings into non-volatile memory.

## Miscellaneous:

- driver_vectornav_set_asynchronous_output: Configure asynchronous output settings.
- driver_vectornav_output_pause: Pause asynchronous output messages.
- driver_vectornav_output_enable_port_1: Resume asynchronous output messages.
- driver_vectornav_binary_output_poll: Poll sensor measurements in binary output protocol.

