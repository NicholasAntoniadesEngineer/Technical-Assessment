/**
 * @file 74hc595pw.h
 * @author Nicholas Antoniades
 * @date July 11, 2023
 * @brief 74HC595 is an 8-bit serial-in, serial or parallel-out shift register
 */

#ifndef 74HC595PW_H
#define 74HC595PW_H

#include <stdint.h>
#include <string.h>
#include "74hc165pw.h"

#define HC595_DELAY 0

typedef struct
{
    struct platform_pin_t const *DS_pin;
    struct platform_pin_t const *OE_pin;
    struct platform_pin_t const *SHCP_pin;
    struct platform_pin_t const *STCP_pin;
}74HC595pw_config_t;

typedef struct
{
    74HC595pw_config_t config;
    uint8_t output_parallel_value;
}74HC595pw_state_t;

STATUS 74hc595pw_init_GPIO(74HC595pw_state_t *state);
STATUS 74hc595pw_init(74HC595pw_state_t *state, 74HC595pw_config_t const *config);
STATUS 74hc595pw_shift_bit(74HC595pw_state_t *state, uint8_t bit);
STATUS 74hc595pw_shift_byte(74HC595pw_state_t *state, uint8_t byte);
STATUS 74hc595pw_clear_shift_register(74HC595pw_state_t *state);
STATUS 74hc595pw_output_parallel(74HC595pw_state_t *state, uint8_t value);
STATUS 74hc595pw_enable_outputs(74HC595pw_state_t *state);
STATUS 74hc595pw_disable_outputs(74HC595pw_state_t *state);
STATUS 74hc595pw_latch_low(74HC595pw_state_t *state);
STATUS 74hc595pw_latch_high(74HC595pw_state_t *state);
STATUS 74hc595pw_hc165_test(74HC165_state_t current_state_74HC165, 74HC595pw_state_t current_state_74HC595);
void 74hc595pw_get_output_parallel(74HC595pw_state_t *state, uint8_t *value);

#endif /* 74HC595PW_H */

