"""
@file BMI160.py
@brief BMI160 python driver for initial integration with a Raspberry Pi

This file contains the implementation for configuring, setting, and reading 
data from the BMI160 sensor using SPI on a Raspberry Pi.

@date April 2022
@version 1.0
@authour Nicholas Antoniades
"""

import spidev
from time import sleep
import RPi.GPIO as GPIO

# BMI160 addresses and commands
Initial_read                 = 0x7F
BMI160_CMD_SOFT_RESET        = 0xB6
BMI160_CMD_ACC_MODE_NORMAL   = 0x11
BMI160_CMD_GYR_MODE_NORMAL   = 0x15
BMI160_RA_PMU_STATUS         = 0x03
BMI160_RA_CHIP_ID            = 0x00
BMI160_RA_CMD                = 0x7E
BMI160_RA_GYRO_CONF          = 0x42
BMI160_RA_GYRO_RANGE         = 0x43
BMI160_RA_GYRO_X_L           = 0x0C
BMI160_RA_ACCEL_X_L          = 0x12
BMI160_RA_ACCEL_CONF         = 0x40
BMI160_RA_ACCEL_RANGE        = 0x41
BMI160_ACCEL_RANGE_16G       = 0x0C
BMI160_ACCEL_RATE_100HZ      = 0x08 
BMI160_GYRO_RANGE_1000       = 0x01   
BMI160_GYRO_RATE_100HZ       = 0x08   
BMI160_ACC_PMU_STATUS_BIT    = 4
BMI160_ACC_PMU_STATUS_LEN    = 2
BMI160_GYR_PMU_STATUS_BIT    = 2
BMI160_GYR_PMU_STATUS_LEN    = 2

def init_spi():
    '''Initializes SPI and CS pin.'''
    global spi, CS1_pin
    # Initialize CS pin
    GPIO.setmode(GPIO.BCM)
    CS1_pin = 24  # Set GPIO24 to be CS pin
    GPIO.setup(CS1_pin, GPIO.OUT)
    GPIO.output(CS1_pin, 0)  # Set CS bit low for SPI transmission 

    # Initialize SPI
    CSL = 0                     # Chip select pin. Set to 0 or 1.
    spi_channel = 1             # Set SPI channel, either 0 or 1.
    spi = spidev.SpiDev()       # Enable SPI
    spi.open(spi_channel, CSL)  # Open a connection to the device
    spi.max_speed_hz = 9600     # Set SPI speed  
    spi.mode = 0b00             # Mode 0b00 has clock inactive low

def reg_read(device, reg_address): 
    '''Reads a single byte from a single address on the SPI device.'''
    # Set CS bit low for transmission
    GPIO.output(device, 0) 
    # Adding Read bit to the register address
    Read_command = reg_address | 0x80     
    # SPI read data from register 
    spi.xfer([Read_command])   
    # Write zeros to keep the clk ticking for the slave response
    data = spi.xfer([0x0, 0x0])  
    # Set CS bit high for end of transmission
    GPIO.output(device, 1) 
    return data

def reg_read_6(device, reg_address): 
    '''Reads 6 bytes from a single address on the SPI device.'''
    # Set CS bit low for transmission
    GPIO.output(device, 0) 
    # Adding Read bit to the register address
    Read_command = reg_address | 0x80     
    # SPI read data from register 
    spi.xfer([Read_command])   
    # Write zeros to keep the clk ticking for the slave response
    data = spi.xfer([0x0] * 6)  
    # Set CS bit high for end of transmission
    GPIO.output(device, 1) 
    return data

def reg_read_12(device, reg_address): 
    '''Reads 12 bytes from a single address on the SPI device.'''
    # Set CS bit low for transmission
    GPIO.output(device, 0) 
    # Adding Read bit to the register address
    Read_command = reg_address | 0x80     
    # SPI read data from register 
    spi.xfer([Read_command])   
    # Write zeros to keep the clk ticking for the slave response
    data = spi.xfer([0x0] * 12)  
    # Set CS bit high for end of transmission
    GPIO.output(device, 1) 
    return data   

def reg_write(device, reg_address, data):
    '''Writes a single byte to a single address on the SPI device.'''
    # Set CS bit low for transmission
    GPIO.output(device, 0) 
    # SPI write data to register
    spi.xfer([reg_address, data])
    # Write zeros to keep the clk ticking for the slave response
    data = spi.xfer([0x0, 0x0])  
    # Set CS bit high for end of transmission
    GPIO.output(device, 1) 
    return data

def reg_read_bits(device, reg, pos, length):
    '''Reads specific bits from a register.'''
    b = reg_read(device, reg)
    mask = (1 << length) - 1
    b[0] >>= pos
    b[0] &= mask
    return b[0]

def reg_write_bits(device, reg, data, pos, length):
    '''Writes specific bits to a register.'''
    b = reg_read(device, reg)
    mask = ((1 << length) - 1) << pos
    data <<= pos  # shift data into correct position
    data &= mask  # zero all non-important bits in data
    b[0] &= ~mask  # zero all important bits in existing byte
    b[0] |= data  # combine data with existing byte
    reg_write(device, reg, b[0])

def BMI160_init():    
    '''Initializes the connection with the BMI160.'''  
    # Issue a soft-reset to bring the device into a clean state
    reg_write(CS1_pin, BMI160_RA_CMD, BMI160_CMD_SOFT_RESET)
    sleep(0.1)
    
    # Issue a dummy-read to force the device into SPI comms mode
    reg_read(CS1_pin, 0x7F)

    # Power up the accelerometer
    reg_write(CS1_pin, BMI160_RA_CMD, BMI160_CMD_ACC_MODE_NORMAL)
    sleep(0.1)

    # Wait for power-up to complete
    while (0x1 != reg_read_bits(CS1_pin, BMI160_RA_PMU_STATUS, BMI160_ACC_PMU_STATUS_BIT, BMI160_ACC_PMU_STATUS_LEN)):
        pass

    # Power up the gyroscope
    reg_write(CS1_pin, BMI160_RA_CMD, BMI160_CMD_GYR_MODE_NORMAL)
    sleep(0.1)
    
    # Wait for power-up to complete 
    while (0x1 != reg_read_bits(CS1_pin, BMI160_RA_PMU_STATUS, BMI160_GYR_PMU_STATUS_BIT, BMI160_GYR_PMU_STATUS_LEN)):
        pass

    # Configure gyroscope
    reg_write_bits(CS1_pin, BMI160_RA_GYRO_RANGE, BMI160_GYRO_RANGE_1000, BMI160_GYRO_RANGE_SEL_BIT, BMI160_GYRO_RANGE_SEL_LEN)
    reg_write_bits(CS1_pin, BMI160_RA_GYRO_CONF, BMI160_GYRO_RATE_100HZ, BMI160_GYRO_RATE_SEL_BIT, BMI160_GYRO_RATE_SEL_LEN)
    
    # Configure accelerometer
    reg_write_bits(CS1_pin, BMI160_RA_ACCEL_RANGE, BMI160_ACCEL_RANGE_16G, BMI160_ACCEL_RANGE_SEL_BIT, BMI160_ACCEL_RANGE_SEL_LEN)
    reg_write_bits(CS1_pin, BMI160_RA_ACCEL_CONF, BMI160_ACCEL_RATE_100HZ, BMI160_ACCEL_RATE_SEL_BIT, BMI160_ACCEL_RATE_SEL_LEN)

def BMI160_testConnection(): 
    '''Tests the connection with the BMI160 by requesting the device ID.'''
    returnVal = reg_read(CS1_pin, Initial_read)
    sleep(0.001)
    returnVal = reg_read(CS1_pin, BMI160_RA_CHIP_ID)
    # The device ID returned should be 0xD1/0b11010001
    return 1 if returnVal[0] == 0xD1 else 0

def BMI160_getAcceleration():   
    '''Fetches the 3-axis acceleration data.'''
    data = [0, 0, 0]
    buffer = reg_read_6(CS1_pin, BMI160_RA_ACCEL_X_L)
    
    data[0] = ((int(buffer[1]) << 8) | buffer[0])  # aX
    data[1] = ((int(buffer[3]) << 8) | buffer[2])  # aY
    data[2] = ((int(buffer[5]) << 8) | buffer[4])  # aZ
    return data

def BMI160_getMotion6Packet(): 
    '''Gets raw 6-axis motion sensor readings (accel/gyro).'''
    data = [0, 0, 0, 0, 0, 0]
    buffer = reg_read_12(CS1_pin, BMI160_RA_GYRO_X_L)
    
    data[0] = ((int(buffer[1]) << 8) | buffer[0])   # gX
    data[1] = ((int(buffer[3]) << 8) | buffer[2])   # gY
    data[2] = ((int(buffer[5]) << 8) | buffer[4])   # gZ
    data[3] = ((int(buffer[7]) << 8) | buffer[6])   # aX
    data[4] = ((int(buffer[9]) << 8) | buffer[8])   # aY
    data[5] = ((int(buffer[11]) << 8) | buffer[10]) # aZ
    return data

if __name__ == "__main__":
    init_spi()
    BMI160_init()  
    print(BMI160_testConnection())

    try:
        while True:   
            data = BMI160_getMotion6Packet()
            if data:
                print(data[3:])  # Print accelerometer data
            sleep(0.1)
    except KeyboardInterrupt:
        GPIO.cleanup()
