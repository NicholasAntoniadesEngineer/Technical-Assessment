/**
 * @file driver_74hc595pw.c
 * @author Nicholas Antoniades
 * @date July 11, 2023
 * @brief 74HC595 8-bit serial-in, serial or parallel-out shift register with output latches.
 */

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "r2.h"
#include "driver_74hc595pw.h"
#include "driver_74hc165pw.h"
#include "driver_platform.h"

/**
 * @brief App level example test function for interfacing with 74HC165 and 74HC595 devices.
 *
 * This function initializes the GPIO pins for both devices, enables the outputs
 * for the 74HC595, and then performs a loop where it outputs parallel data to the
 * 74HC595 and reads parallel inputs from the 74HC165.
 */
STATUS driver_74hc595pw_hc165_test(driver_74HC165_state_t current_state_74HC165, driver_74HC595pw_state_t current_state_74HC595) {
    driver_74hc165pw_init_GPIO(&current_state_74HC165);
    driver_74hc595pw_init_GPIO(&current_state_74HC595);
    driver_74hc595pw_enable_outputs(&current_state_74HC595);

    for(int i = 0; i < 256; i++) {
        driver_74hc595pw_output_parallel(&current_state_74HC595, i);
        driver_74hc165pw_read_parallel_inputs(&current_state_74HC165);
        hc165_byte_pair read_data = current_state_74HC165.read_data;
        (void)read_data;
        HAL_Delay(5);
    }
    return OK;
}

/**
 * @brief Initialize GPIO for the 74HC595 Shift Register
 * @param state 74HC165 driver state with pin and port details
 */
STATUS driver_74hc595pw_init_GPIO(driver_74HC595pw_state_t *state)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = state->config.OE_pin->number | state->config.SHCP_pin->number | state->config.STCP_pin->number | state->config.DS_pin->number;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(state->config.OE_pin->port, &GPIO_InitStruct);

    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStruct.Pin = state->config.DS_pin->number;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(state->config.DS_pin->port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(state->config.OE_pin->port, state->config.OE_pin->number, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(state->config.SHCP_pin->port, state->config.SHCP_pin->number, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(state->config.STCP_pin->port, state->config.STCP_pin->number, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(state->config.DS_pin->port, state->config.DS_pin->number, GPIO_PIN_RESET);
    return OK;
}

STATUS driver_74hc595pw_init(driver_74HC595pw_state_t *state, driver_74HC595pw_config_t const *config)
{
    state->config = *config;

    // Initialise 74HC595 shift register
    driver_74hc595pw_init_GPIO(state);
    driver_74hc595pw_enable_outputs(state);
    return OK;
}

/**
 * @brief 74HC595 driver shift a single bit into the shift register.
 * @param state Pointer to the state structure.
 * @param bit The bit to shift.
 */
STATUS driver_74hc595pw_shift_bit(driver_74HC595pw_state_t *state, uint8_t write_bit)
{
    HAL_GPIO_WritePin(state->config.SHCP_pin->port, state->config.SHCP_pin->number, GPIO_PIN_RESET);
    HAL_Delay(HC595_DELAY);

    GPIO_PinState pin_state = (write_bit != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(state->config.DS_pin->port, state->config.DS_pin->number, pin_state);

    /* Generate clock pulse on the shift register clock input.
     * When the clock signal goes from low to high (a rising edge),
     * the shift register shifts the data one position to the right.
     * When the clock signal goes from high to low (a falling edge),
     * the shift register holds the current data.
     */

    HAL_GPIO_WritePin(state->config.SHCP_pin->port, state->config.SHCP_pin->number, GPIO_PIN_SET);
    HAL_Delay(HC595_DELAY);

    state->output_parallel_value = ((state->output_parallel_value << 1) | (write_bit != 0)) & 0xFFU;

    return OK;
}

/**
 * @brief 74HC595 driver shift a byte into the shift register.
 * @param state Pointer to the state structure.
 * @param byte The byte to shift.
 */
STATUS driver_74hc595pw_shift_byte(driver_74HC595pw_state_t *state, uint8_t byte)
{
    for (int i = 7; i >= 0; i--) {
        driver_74hc595pw_shift_bit(state, byte & (1 << i));
    }
    HAL_GPIO_WritePin(state->config.DS_pin->port, state->config.DS_pin->number, GPIO_PIN_SET);
    return OK;
}

/**
 * @brief 74HC595 driver latch the data in the shift register to the output register.
 * @param state Pointer to the state structure.
 */
STATUS driver_74hc595pw_latch_low(driver_74HC595pw_state_t *state)
{
    HAL_GPIO_WritePin(state->config.STCP_pin->port, state->config.STCP_pin->number, GPIO_PIN_RESET);
    return OK;
}

/**
 * @brief 74HC595 driver latch the data in the shift register to the output register.
 * @param state Pointer to the state structure.
 */
STATUS driver_74hc595pw_latch_high(driver_74HC595pw_state_t *state)
{
    HAL_GPIO_WritePin(state->config.STCP_pin->port, state->config.STCP_pin->number, GPIO_PIN_SET);
    return OK;
}

/**
 * @brief 74HC595 driver clear the shift register.
 * @param state Pointer to the state structure.
 */
STATUS driver_74hc595pw_clear_shift_register(driver_74HC595pw_state_t *state)
{
    /* The 74HC595 shift register can be cleared (set all bits to 0) by setting the
     * MR (Master reset) pin to low. The first line sets the MR pin to low to clear the register,
     * and the second line sets it back to high to allow normal operation.
     */

    HAL_Delay(HC595_DELAY);
    HAL_GPIO_WritePin(state->config.OE_pin->port, state->config.OE_pin->number, GPIO_PIN_RESET);
    HAL_Delay(HC595_DELAY);
    HAL_GPIO_WritePin(state->config.OE_pin->port, state->config.OE_pin->number, GPIO_PIN_SET);
    return OK;
}

/**
 * @brief 74HC595 driver enable the outputs.
 * @param state Pointer to the state structure.
 */
STATUS driver_74hc595pw_enable_outputs(driver_74HC595pw_state_t *state)
{
    HAL_GPIO_WritePin(state->config.OE_pin->port, state->config.OE_pin->number, GPIO_PIN_SET);
    return OK;
}

/**
 * @brief 74HC595 driver disable the outputs.
 * @param state Pointer to the state structure.
 */
STATUS driver_74hc595pw_disable_outputs(driver_74HC595pw_state_t *state)
{
    HAL_GPIO_WritePin(state->config.OE_pin->port, state->config.OE_pin->number, GPIO_PIN_RESET);
    return OK;
}

/**
 * @brief 74HC595 driver output an 8-bit value on the parallel data output pins.
 * @param state Pointer to the state structure.
 * @param value The value to output.
 */
STATUS driver_74hc595pw_output_parallel(driver_74HC595pw_state_t *state, uint8_t value)
{

    driver_74hc595pw_latch_low(state);

    driver_74hc595pw_shift_byte(state, value);

    driver_74hc595pw_latch_high(state);
    return OK;
}

void driver_74hc595pw_get_output_parallel(driver_74HC595pw_state_t *state, uint8_t *value)
{
    *value = state->output_parallel_value;
}
