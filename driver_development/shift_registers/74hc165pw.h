/**
 * @file 74hc165pw.h
 * @brief Driver header for the 74HC165PW 8-bit parallel-in/serial-out shift register.
 * @author Nicholas Antoniades
 * @date July 11, 2023
 */

#pragma once

#include <stdint.h>
#include <string.h>
#include "bsp/bsp_gpio.h"

#define HC165_NUM_BITS 8

struct hc165_data_t
{
    uint8_t parallel_data;  /**< Data from the parallel data lines */
    uint8_t serial_data;    /**< Data that has been fed in serially via the DS pin */
};

struct hc165_config_t
{
    struct bsp_pin_t PL;   /**< Parallel load pin */
    struct bsp_pin_t CP;   /**< Clock pin */
    struct bsp_pin_t Q7;   /**< Serial data output pin */
};

struct hc165_state_t
{
    struct hc165_config_t config;
    uint8_t read_bit;                  /**< Binary value of bit read on last clock cycle */
    struct hc165_data_t read_data;     /**< Value pair representing the parallel and serial data bytes */
};

STATUS hc165_init(struct hc165_state_t *state, struct hc165_config_t const *config);
STATUS hc165_shift_bit(struct hc165_state_t *state);
STATUS hc165_read_parallel_inputs(struct hc165_state_t *state);
STATUS hc165_latch_low(struct hc165_state_t *state);
STATUS hc165_latch_high(struct hc165_state_t *state);

